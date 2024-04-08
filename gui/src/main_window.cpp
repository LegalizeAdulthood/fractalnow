/*
 *  main_window.cpp -- part of FractalNow
 *
 *  Copyright (c) 2012 Marc Pegon <pe.marc@free.fr>
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU Lesser General Public License as published by
 *  the Free Software Foundation; either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.
 */
 
#include "main_window.h"
#include "main.h"
#include "mpfr_spin_box.h"

#include <clocale>
#include <iostream>
#include <limits>

#include <QAction>
#include <QApplication>
#include <QDockWidget>
#include <QFileDialog>
#include <QFormLayout>
#include <QGridLayout>
#include <QInputDialog>
#include <QMenuBar>
#include <QMessageBox>
#include <QMimeData>
#include <QSettings>
#include <QTextCodec>
#include <QTimer>
#include <QThread>
#include <QToolBar>
#include <QToolButton>
#include <QUrl>

void initDefaultFractal(Fractal &fractal)
{
	InitFractal(&fractal, FRAC_MANDELBROT, cbuild_l(2,0), cbuild_l(0.4,0.25),
				-0.7, 0, 3, 3, 1E3, 1000);
}

void initDefaultGradient(Gradient &gradient)
{
	uint32_t hex_color[5] = { 0x39A0, 0xFFFFFF, 0xFFFE43, 0xBF0800, 0x39A0 };
	Color color[5];
	for (uint_fast32_t i = 0; i < 5; ++i) {
		color[i] = ColorFromUint32(hex_color[i]);
	}
	GenerateGradient2(&gradient, 5, color, DEFAULT_GRADIENT_TRANSITIONS);
}

void initDefaultRenderingParameters(RenderingParameters &render)
{
	Gradient gradient;
	initDefaultGradient(gradient);

	/* Init rendering parameters. */
	InitRenderingParameters(&render, 1, ColorFromUint32(0x0), IC_SMOOTH,
				CM_ITERATIONCOUNT, AF_TRIANGLEINEQUALITY, 1,
				IM_NONE, TF_LOG, 4.5E-1, 0.2, gradient);
}

inline void initDefaultConfig(FractalConfig &config)
{
	initDefaultFractal(config.fractal);
	initDefaultRenderingParameters(config.render);
}

void MainWindow::loadSettings()
{
	QSettings settings;

	imageDir = settings.value("imageDir", QStandardPaths::writableLocation(
			QStandardPaths::PicturesLocation)).toString();
	configDir = settings.value("configDir", QStandardPaths::writableLocation(
			QStandardPaths::DocumentsLocation)).toString();
	gradientDir = settings.value("gradientDir", QStandardPaths::writableLocation(
			QStandardPaths::DocumentsLocation)).toString();
	adaptExplorerSize = settings.value("adaptExplorerSize", false).toBool();
	lastPreferredExplorerWidth = settings.value("preferredExplorerWidth",
		DEFAULT_EXPLORER_WIDTH).toUInt();
	lastPreferredExplorerHeight = settings.value("preferredExplorerHeight",
		DEFAULT_EXPLORER_HEIGHT).toUInt();
	lastWindowWidth = settings.value("windowWidth", width()).toUInt();
	lastWindowHeight = settings.value("windowHeight", height()).toUInt();
	useCache = settings.value("useCache", true).toBool();
	cacheSize = settings.value("cacheSize",
			(unsigned int)DEFAULT_FRACTAL_CACHE_SIZE).toUInt();
	solidGuessing = settings.value("solidGuessing", true).toBool();
	fractalnow_mp_precision = settings.value("MPFRPrec",
		(unsigned int)DEFAULT_MP_PRECISION).toUInt();
}

void MainWindow::saveSettings()
{
	QSettings settings;
	settings.setValue("imageDir", imageDir);
	settings.setValue("configDir", configDir);
	settings.setValue("gradientDir", gradientDir);
	settings.setValue("adaptExplorerSize",
		adaptExplorerToWindowAction->isChecked());
	settings.setValue("preferredExplorerWidth", preferredImageWidthSpinBox->value());
	settings.setValue("preferredExplorerHeight", preferredImageHeightSpinBox->value());
	settings.setValue("windowWidth", width());
	settings.setValue("windowHeight", height());
	settings.setValue("useCache", fractalExplorer->getFractalCacheEnabled());
	settings.setValue("cacheSize", fractalExplorer->getFractalCacheSize());
	settings.setValue("solidGuessing", fractalExplorer->getSolidGuessingEnabled());
	settings.setValue("MPFRPrec", MPFloatPrecisionSpinBox->value());
}

MainWindow::MainWindow(int argc, char *argv[])
{
	/* Set icon */
	QString iconSizes[12] = {"16x16", "22x22", "24x24", "32x32", "36x36",
		"48x48", "64x64", "72x72", "96x96", "128x128", "192x192", "256x256"};
	QIcon icon;
	for (int i = 0; i < 12; ++i) {
		icon.addFile(":icons/icon" + iconSizes[i] + ".png");
	}
	this->setWindowIcon(icon);
	this->setWindowTitle(QApplication::applicationName());

	setAcceptDrops(true);

	/* Set C numeric locale and codecs for strings. */
	setlocale(LC_NUMERIC, "C");

	/* Load settings. */
	loadSettings();

	/* Read arguments on command-line. */
	CommandLineArguments args(argc, argv);
	SetMPFloatPrecision(args.MPFloatPrecision);

	FractalConfig config;
	Fractal &fractal = config.fractal;
	RenderingParameters &render = config.render;

	/* Deal with arguments on command-line. */
	initDefaultConfig(config);
	if (args.fractalConfigFileName) {
		/* Read config file. */
		FreeFractalConfig(config);
		if (ReadFractalConfigFile(&config, args.fractalConfigFileName)) {
			FractalNow_message(stderr, T_QUIET, "Failed to read config file. Falling back to default.\n");
			initDefaultConfig(config);
		}
	}
	if (args.fractalFileName) {
		/* Read fractal file. */
		Fractal newFractal;
		if (ReadFractalFile(&newFractal, args.fractalFileName)) {
			FractalNow_message(stderr, T_QUIET, "Failed to read fractal file. Falling back to default.\n");
			initDefaultFractal(newFractal);
		}
		ResetFractal(&config, newFractal);
		FreeFractal(newFractal);
	}
	if (args.renderingFileName) {
		/* Read rendering file. */
		RenderingParameters newRender;
		if (ReadRenderingFile(&newRender, args.renderingFileName)) {
			FractalNow_message(stderr, T_QUIET, "Failed to read rendering file. Falling back to default.\n");
			initDefaultRenderingParameters(newRender);
		}
		ResetRenderingParameters(&config, newRender);
		FreeRenderingParameters(newRender);
	}
	if (args.gradientFileName) {
		/* Read gradient file. */
		Gradient newGradient;
		if (ReadGradientFile(&newGradient, args.gradientFileName)) {
			FractalNow_message(stderr, T_QUIET, "Failed to read gradient file. Falling back to default.\n");
			initDefaultGradient(newGradient);
		}
		ResetGradient(&render, newGradient);
		FreeGradient(newGradient);
	}
	if (config.render.bytesPerComponent == 2) {
		QMessageBox::critical(this, tr("Gradient color depth is 16 bits."),
			tr("16-bits gradient will be converted to 8-bits gradient."));
		Gradient newGradient = Gradient8(&config.render.gradient);
		ResetGradient(&config.render, newGradient);
		FreeGradient(newGradient);
	}

	uint_fast32_t explorerWidth = args.width;
	uint_fast32_t explorerHeight = args.height;
	if (explorerWidth == 0 && explorerHeight == 0) {
		/* If both width and height were omitted : set to default. */
		explorerWidth = lastPreferredExplorerWidth;
		explorerHeight = lastPreferredExplorerHeight;
	} 
	if (explorerWidth == 0) {
		/* If only width was omitted, compute it to keep ratio. */
		mpfr_t spanRatio;
		mpfr_init(spanRatio);
		mpfr_div(spanRatio, fractal.spanX, fractal.spanY, MPFR_RNDN);
		explorerWidth = roundl(mpfr_get_ld(spanRatio, MPFR_RNDN) * explorerHeight);
		mpfr_clear(spanRatio);
	} else if (explorerHeight == 0) {
		/* If only height was omitted, compute it to keep ratio. */
		mpfr_t spanRatio;
		mpfr_init(spanRatio);
		mpfr_div(spanRatio, fractal.spanX, fractal.spanY, MPFR_RNDN);
		explorerHeight = roundl(explorerWidth / mpfr_get_ld(spanRatio,MPFR_RNDN));
		mpfr_clear(spanRatio);
	} else {
		/* If both width and height were specified (or set do default),
		 * adapt spanX and spanY.
		 */
		mpfr_t spanX, spanY;
		mpfr_init(spanX);
		mpfr_init(spanY);
		if (explorerHeight / (double)explorerWidth < 1) {
			mpfr_mul_d(spanX, fractal.spanY, explorerWidth / (double)explorerHeight, MPFR_RNDN);
			mpfr_set(spanY, fractal.spanY, MPFR_RNDN);
		} else {
			mpfr_set(spanX, fractal.spanX, MPFR_RNDN);
			mpfr_mul_d(spanY, fractal.spanX, explorerHeight / (double)explorerWidth, MPFR_RNDN);
		}
		/* Reinit fractal, since we changed spanX or spanY. */
		Fractal newFractal;
		InitFractal2(&newFractal, fractal.fractalFormula, fractal.p, fractal.c,
				fractal.centerX, fractal.centerY, spanX, spanY,
				fractal.escapeRadius, fractal.maxIter);
		ResetFractal(&config, newFractal);
		FreeFractal(newFractal);
		mpfr_clear(spanX);
		mpfr_clear(spanY);
	}

	if (args.nbThreads <= 0) {
		/* Number of threads not specified. */
		if (QThread::idealThreadCount() > 0) {
			fractalExplorerNbThreads = QThread::idealThreadCount();

			/* To export image we decide to use more threads that the actual
			 * number of cores, because some parts of image may need much
			 * more calculations than others, so we may have one thread
			 * taking much more time that the others, which is not good
			 * (action finishes when all thread have finished).
			 * In short, with more threads, we "average" the time taken by each
			 * thread.
			 * However, having more threads than number of cores is not good
			 * for real-time, which is why fractal explorer uses exactly ideal
			 * thread count.
			 */
			 exportImageNbThreads = 4*QThread::idealThreadCount();
		} else {
			fractalExplorerNbThreads = DEFAULT_NB_THREADS;
			exportImageNbThreads = DEFAULT_NB_THREADS;
		}
	} else {
		fractalExplorerNbThreads = args.nbThreads;
		exportImageNbThreads = args.nbThreads;
	}

	/* Create fractalImage label which will be the central widget. */
	fractalExplorer = new FractalExplorer(config, explorerWidth,
					explorerHeight, args.minAntiAliasingSize,
					args.maxAntiAliasingSize, args.antiAliasingSizeIteration,
					args.quadInterpolationSize,
					args.colorDissimilarityThreshold,
					args.adaptiveAAMThreshold,
					fractalExplorerNbThreads);
	fractalExplorer->resizeFractalCache(cacheSize);
	fractalExplorer->useFractalCache(useCache);
	fractalExplorer->setSolidGuessingEnabled(solidGuessing);
	fractalExplorer->setFocus();

	/* Make it the central widget with correct size policies. */
	QWidget *centralWidget = new QWidget;
	QGridLayout *centralLayout = new QGridLayout;
	centralLayout->addWidget(fractalExplorer, 1, 1);
	if (adaptExplorerSize) {
		fractalExplorer->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
	} else {
		fractalExplorer->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
	}
	centralWidget->setLayout(centralLayout);
	centralWidget->updateGeometry();
	this->setCentralWidget(centralWidget);

	/* Create fractal config widget. */
	fractalConfigWidget = new FractalConfigWidget(fractal);
	/* Connect the fractal config widget spin boxes to slots. */
	connect(fractalExplorer, SIGNAL(fractalChanged(const Fractal &)),
		fractalConfigWidget, SLOT(updateBoxesValues(const Fractal &)));
	connect(fractalConfigWidget->fractalFormulaComboBox, SIGNAL(currentIndexChanged(int)),
		fractalExplorer, SLOT(setFractalFormula(int)));
	connect(fractalConfigWidget->pParamReSpinBox, SIGNAL(valueChanged(const mpfr_t *)),
		fractalExplorer, SLOT(setPParamRe(const mpfr_t *)));
	connect(fractalConfigWidget->pParamImSpinBox, SIGNAL(valueChanged(const mpfr_t *)),
		fractalExplorer, SLOT(setPParamIm(const mpfr_t *)));
	connect(fractalConfigWidget->cParamReSpinBox, SIGNAL(valueChanged(const mpfr_t *)),
		fractalExplorer, SLOT(setCParamRe(const mpfr_t *)));
	connect(fractalConfigWidget->cParamImSpinBox, SIGNAL(valueChanged(const mpfr_t *)),
		fractalExplorer, SLOT(setCParamIm(const mpfr_t *)));
	connect(fractalConfigWidget->centerXSpinBox, SIGNAL(valueChanged(const mpfr_t *)),
		fractalExplorer, SLOT(setCenterX(const mpfr_t *)));
	connect(fractalConfigWidget->centerYSpinBox, SIGNAL(valueChanged(const mpfr_t *)),
		fractalExplorer, SLOT(setCenterY(const mpfr_t *)));
	connect(fractalConfigWidget->spanXSpinBox, SIGNAL(valueChanged(const mpfr_t *)),
		fractalExplorer, SLOT(setSpanX(const mpfr_t *)));
	connect(fractalConfigWidget->bailoutRadiusSpinBox, SIGNAL(valueChanged(double)),
		fractalExplorer, SLOT(setBailoutRadius(double)));
	connect(fractalConfigWidget->maxIterationsSpinBox, SIGNAL(valueChanged(int)),
		fractalExplorer, SLOT(setMaxIterations(int)));

	/* Create rendering config widget.*/
	fractalRenderingWidget = new FractalRenderingWidget(render);
	/* Connect the fractal rendering widget combo and spin boxes to slots. */
	connect(fractalExplorer, SIGNAL(renderingParametersChanged(const RenderingParameters &)),
		fractalRenderingWidget, SLOT(updateBoxesValues(const RenderingParameters &)));
	connect(fractalRenderingWidget->addendFunctionComboBox, SIGNAL(currentIndexChanged(int)),
		fractalExplorer, SLOT(setAddendFunction(int)));
	connect(fractalRenderingWidget->stripeDensitySpinBox, SIGNAL(valueChanged(int)),
		fractalExplorer, SLOT(setStripeDensity(int)));
	connect(fractalRenderingWidget->iterationCountComboBox, SIGNAL(currentIndexChanged(int)),
		fractalExplorer, SLOT(setIterationCount(int)));
	connect(fractalRenderingWidget->coloringMethodComboBox, SIGNAL(currentIndexChanged(int)),
		fractalExplorer, SLOT(setColoringMethod(int)));
	connect(fractalRenderingWidget->interpolationMethodComboBox, SIGNAL(currentIndexChanged(int)),
		fractalExplorer, SLOT(setInterpolationMethod(int)));
	connect(fractalRenderingWidget->transferFunctionComboBox, SIGNAL(currentIndexChanged(int)),
		fractalExplorer, SLOT(setTransferFunction(int)));
	connect(fractalRenderingWidget->colorScalingSpinBox, SIGNAL(valueChanged(double)),
		fractalExplorer, SLOT(setColorScaling(double)));
	connect(fractalRenderingWidget->colorOffsetSpinBox, SIGNAL(valueChanged(double)),
		fractalExplorer, SLOT(setColorOffset(double)));
	connect(fractalRenderingWidget->spaceColorButton, SIGNAL(currentColorChanged(const QColor &)),
		fractalExplorer, SLOT(setSpaceColor(const QColor &)));
	connect(fractalRenderingWidget->gradientBox, SIGNAL(gradientStopsChanged(
		const QGradientStops&)), fractalExplorer, SLOT(setGradient(const QGradientStops&)));
	
	/* Free fractal config (copied by fractal explorer). */
	FreeFractalConfig(config);

	/* Create "other" dock widget. */
	/* Create content and connect signals to slots first. */
	QWidget *otherParametersWidget = new QWidget;
	QFormLayout *otherParamLayout = new QFormLayout;
	preferredImageWidthSpinBox = new QSpinBox;
	preferredImageWidthSpinBox->setRange(2, 2048);
	preferredImageWidthSpinBox->setButtonSymbols(QAbstractSpinBox::NoButtons);
	connect(preferredImageWidthSpinBox, SIGNAL(editingFinished()), this,
		SLOT(onPreferredImageWidthChanged()));
	preferredImageHeightSpinBox = new QSpinBox;
	preferredImageHeightSpinBox->setRange(2, 2048);
	preferredImageHeightSpinBox->setButtonSymbols(QAbstractSpinBox::NoButtons);
	connect(preferredImageHeightSpinBox, SIGNAL(editingFinished()), this,
		SLOT(onPreferredImageHeightChanged()));
	solidGuessingCheckBox = new QCheckBox;
	connect(solidGuessingCheckBox, SIGNAL(toggled(bool)),
		fractalExplorer, SLOT(setSolidGuessingEnabled(bool)));
	useCacheCheckBox = new QCheckBox;
	connect(useCacheCheckBox, SIGNAL(toggled(bool)), fractalExplorer, SLOT(useFractalCache(bool)));
	cacheSizeSpinBox = new QSpinBox;
	cacheSizeSpinBox->setRange(0, 10000000);
	cacheSizeSpinBox->setButtonSymbols(QAbstractSpinBox::NoButtons);
	cacheSizeSpinBox->setEnabled(fractalExplorer->getFractalCacheEnabled());
	connect(cacheSizeSpinBox, SIGNAL(editingFinished()), this, SLOT(onCacheSizeChanged()));
	connect(useCacheCheckBox, SIGNAL(toggled(bool)), cacheSizeSpinBox, SLOT(setEnabled(bool)));
	floatTypeComboBox = new QComboBox;
	for (uint_fast32_t i = 0; i < nbFloatPrecisions; ++i) {
		floatTypeComboBox->addItem(floatPrecisionDescStr[i]);
	}
	connect(floatTypeComboBox, SIGNAL(currentIndexChanged(int)),
		fractalExplorer, SLOT(setFloatPrecision(int)));
	QHBoxLayout *hBoxLayout = new QHBoxLayout;
	MPFloatPrecisionSpinBox = new QSpinBox;
	MPFloatPrecisionSpinBox->setRange(MPFR_PREC_MIN,
		std::min<long long int>(std::numeric_limits<int>::max(),(long long int)MPFR_PREC_MAX));
	MPFloatPrecisionSpinBox->setButtonSymbols(QAbstractSpinBox::NoButtons);
	MPFloatPrecisionSpinBox->setReadOnly(true);
	QToolButton *editMPFloatPrecisionButton = new QToolButton();
	editMPFloatPrecisionButton->setText(tr("..."));
	connect(editMPFloatPrecisionButton, SIGNAL(clicked()), this, SLOT(editMPFloatPrecision()));
	hBoxLayout->addWidget(MPFloatPrecisionSpinBox);
	hBoxLayout->addWidget(editMPFloatPrecisionButton);
	hBoxLayout->setStretch(1, 0);
	editMPFloatPrecisionWidget = new QWidget();
	editMPFloatPrecisionWidget->setLayout(hBoxLayout);
#ifdef _ENABLE_MP_FLOATS
	editMPFloatPrecisionWidget->setEnabled(args.floatPrecision == FP_MP);
#else
	editMPFloatPrecisionWidget->setEnabled(false);
#endif
	connect(floatTypeComboBox, SIGNAL(currentIndexChanged(int)),
		this, SLOT(onFloatTypeChanged(int)));
	connect(fractalExplorer, SIGNAL(fractalCacheSizeChanged(int)),
		cacheSizeSpinBox, SLOT(setValue(int)));
	/* Set widgets values... */
	preferredImageWidthSpinBox->setValue((int)lastPreferredExplorerWidth);
	preferredImageHeightSpinBox->setValue((int)lastPreferredExplorerHeight);
	solidGuessingCheckBox->setChecked(fractalExplorer->getSolidGuessingEnabled());
	useCacheCheckBox->setChecked(fractalExplorer->getFractalCacheEnabled());
	cacheSizeSpinBox->setValue(fractalExplorer->getFractalCacheSize());
	floatTypeComboBox->setCurrentIndex((int)args.floatPrecision);
	MPFloatPrecisionSpinBox->setValue((int)GetMPFloatPrecision());
	/* Add widgets to layout. */
	otherParamLayout->addRow(tr("Preferred image width:"), preferredImageWidthSpinBox);
	otherParamLayout->addRow(tr("Preferred image height:"), preferredImageHeightSpinBox);
	otherParamLayout->addRow(tr("Solid guessing:"), solidGuessingCheckBox);
	otherParamLayout->addRow(tr("Use cache:"), useCacheCheckBox);
	otherParamLayout->addRow(tr("Cache size:"), cacheSizeSpinBox);
	otherParamLayout->addRow(tr("Float type:"), floatTypeComboBox);
	otherParamLayout->addRow(tr("MP Float precision:"), editMPFloatPrecisionWidget);
	/* Set layout. */
	otherParametersWidget->setLayout(otherParamLayout);
	
	/* Create dock widgets. */
	fractalDock = new QDockWidget(tr("Fractal"), this);
	renderDock = new QDockWidget(tr("Rendering"), this);
	otherDock = new QDockWidget(tr("Other"), this);
	/* Set dock widget properties. */
	fractalDock->setWidget(fractalConfigWidget);
	fractalDock->setAllowedAreas(Qt::LeftDockWidgetArea | Qt::RightDockWidgetArea);
	renderDock->setWidget(fractalRenderingWidget);
	renderDock->setAllowedAreas(Qt::LeftDockWidgetArea | Qt::RightDockWidgetArea);
	otherDock->setWidget(otherParametersWidget);
	otherDock->setAllowedAreas(Qt::LeftDockWidgetArea | Qt::RightDockWidgetArea);
	/* Add dock widgets. */
	this->addDockWidget(Qt::RightDockWidgetArea, fractalDock);
	this->addDockWidget(Qt::RightDockWidgetArea, renderDock);
	this->addDockWidget(Qt::RightDockWidgetArea, otherDock);
	setTabPosition(Qt::LeftDockWidgetArea | Qt::RightDockWidgetArea, QTabWidget::North);
	tabifyDockWidget(fractalDock, renderDock);
	tabifyDockWidget(renderDock, otherDock);
	/* Set current tabified dock to fractaldock. */
	QList<QTabBar *> tabList = findChildren<QTabBar *>();
	if (!tabList.isEmpty()) {
		QTabBar *tabBar = tabList.at(0);
		tabBar->setCurrentIndex(0);
	}

	QMenuBar *mainMenuBar = new QMenuBar(0);
	setMenuBar(mainMenuBar);
	QMenu *fileMenu = menuBar()->addMenu(tr("&File"));
	QAction *openConfigAction = new QAction(tr("&Open config"), this);
	openConfigAction->setToolTip(tr("Open configuration file"));
	openConfigAction->setShortcut(QKeySequence::Open);
	connect(openConfigAction, SIGNAL(triggered()), this, SLOT(openConfigFile()));
	fileMenu->addAction(openConfigAction);
	addAction(openConfigAction);
	QAction *saveConfigAction = new QAction(tr("&Save config"), this);
	saveConfigAction->setToolTip(tr("Save configuration file"));
	QList<QKeySequence> saveConfigShortcuts;
	saveConfigShortcuts << QKeySequence::Save << QKeySequence::SaveAs;
	saveConfigAction->setShortcuts(saveConfigShortcuts);
	connect(saveConfigAction, SIGNAL(triggered()), this, SLOT(saveConfigFile()));
	fileMenu->addAction(saveConfigAction);
	addAction(saveConfigAction);
	fileMenu->addSeparator();
	QAction *openGradientAction = new QAction(tr("Open &gradient"), this);
	openGradientAction->setToolTip(tr("Open gradient file"));
	connect(openGradientAction, SIGNAL(triggered()), this, SLOT(openGradientFile()));
	fileMenu->addAction(openGradientAction);
	addAction(openGradientAction);
	QAction *saveGradientAction = new QAction(tr("Save gradient"), this);
	saveGradientAction->setToolTip(tr("Save gradient file"));
	connect(saveGradientAction, SIGNAL(triggered()), this, SLOT(saveGradientFile()));
	fileMenu->addAction(saveGradientAction);
	addAction(saveGradientAction);
	fileMenu->addSeparator();
	QAction *exportImageAction = new QAction(tr("E&xport image"), this);
	exportImageAction->setToolTip(tr("Export fractal as image"));
	exportImageAction->setIcon(QApplication::style()->standardIcon(QStyle::SP_DriveFDIcon));
	exportImageAction->setShortcut(QKeySequence(Qt::CTRL + Qt::Key_X));
	connect(exportImageAction, SIGNAL(triggered()), this, SLOT(exportImage()));
	fileMenu->addAction(exportImageAction);
	addAction(exportImageAction);
	fileMenu->addSeparator();
	QAction *quitAction = new QAction(tr("&Quit"), this);
	quitAction->setShortcut(QKeySequence::Quit);
	connect(quitAction, SIGNAL(triggered()), this, SLOT(close()));
	fileMenu->addSeparator();
	fileMenu->addAction(quitAction);
	addAction(quitAction);

	QMenu *editMenu = menuBar()->addMenu(tr("&Edit"));
	editMenu->addAction(fractalExplorer->restoreInitialStateAction);
	editMenu->addSeparator();
	editMenu->addAction(fractalExplorer->stopDrawingAction);
	addAction(fractalExplorer->stopDrawingAction);
	editMenu->addAction(fractalExplorer->refreshAction);
	addAction(fractalExplorer->refreshAction);
	editMenu->addSeparator();
	editMenu->addAction(fractalExplorer->zoomInAction);
	addAction(fractalExplorer->zoomInAction);
	editMenu->addAction(fractalExplorer->zoomOutAction);
	addAction(fractalExplorer->zoomOutAction);
	editMenu->addSeparator();
	editMenu->addAction(fractalExplorer->moveLeftAction);
	addAction(fractalExplorer->moveLeftAction);
	editMenu->addAction(fractalExplorer->moveRightAction);
	addAction(fractalExplorer->moveRightAction);
	editMenu->addAction(fractalExplorer->moveUpAction);
	addAction(fractalExplorer->moveUpAction);
	editMenu->addAction(fractalExplorer->moveDownAction);
	addAction(fractalExplorer->moveDownAction);
	editMenu->addSeparator();
	QAction *editGradientAction = new QAction(tr("Edit &gradient"), this);
	editGradientAction->setIconText(tr("Gradient"));
	editGradientAction->setShortcut(QKeySequence(Qt::CTRL + Qt::Key_G));
	connect(editGradientAction, SIGNAL(triggered()),
		fractalRenderingWidget, SLOT(editGradient()));
	editMenu->addAction(editGradientAction);
	addAction(editGradientAction);

	toolBar = addToolBar(tr("Tool bar"));
	toolBar->setToolButtonStyle(Qt::ToolButtonTextBesideIcon);

	QMenu *viewMenu = menuBar()->addMenu(tr("&View"));
	viewMenu->addAction(fractalDock->toggleViewAction());
	addAction(fractalDock->toggleViewAction());
	viewMenu->addAction(renderDock->toggleViewAction());
	addAction(renderDock->toggleViewAction());
	viewMenu->addAction(toolBar->toggleViewAction());
	addAction(toolBar->toggleViewAction());
	viewMenu->addSeparator();
	adaptExplorerToWindowAction = new QAction(tr("Adapt to &window"), this);
	adaptExplorerToWindowAction->setToolTip(tr("Adapt fractal explorer size to window"));
	adaptExplorerToWindowAction->setCheckable(true);
	adaptExplorerToWindowAction->setChecked(false);
	adaptExplorerToWindowAction->setShortcut(Qt::Key_F);
	connect(adaptExplorerToWindowAction, SIGNAL(toggled(bool)), this,
		SLOT(adaptExplorerToWindow(bool)));
	viewMenu->addAction(adaptExplorerToWindowAction);
	addAction(adaptExplorerToWindowAction);
	switchFullScreenAction = new QAction(tr("&Full Screen mode"), this);
	switchFullScreenAction->setToolTip(tr("Switch to Full Screen mode"));
	switchFullScreenAction->setCheckable(true);
	QList<QKeySequence> switchFullScreenShortcuts;
	switchFullScreenShortcuts << QKeySequence(Qt::Key_F11) << QKeySequence(Qt::ALT + Qt::Key_Return);
	switchFullScreenAction->setShortcuts(switchFullScreenShortcuts);
	connect(switchFullScreenAction, SIGNAL(toggled(bool)), this,
		SLOT(switchFullScreenMode(bool)));
	viewMenu->addAction(switchFullScreenAction);
	addAction(switchFullScreenAction);

	QMenu *helpMenu = menuBar()->addMenu(tr("&Help"));
	QAction *aboutQtAction = new QAction(tr("About &Qt"), this);
	connect(aboutQtAction, SIGNAL(triggered()), this, SLOT(aboutQt()));
	helpMenu->addAction(aboutQtAction);
	addAction(aboutQtAction);

	QAction *aboutQFractalNowAction = new QAction(tr("&About QFractalNow"), this);
	connect(aboutQFractalNowAction, SIGNAL(triggered()), this, SLOT(aboutQFractalNow()));
	helpMenu->addAction(aboutQFractalNowAction);
	addAction(aboutQFractalNowAction);

	QAction *escapeFullScreenAction = new QAction(this);
	escapeFullScreenAction->setShortcut(QKeySequence(Qt::Key_Escape));
	addAction(escapeFullScreenAction);
	connect(escapeFullScreenAction, SIGNAL(triggered()), this, SLOT(escapeFullScreen()));

	toolBar->addAction(openConfigAction);
	toolBar->addAction(saveConfigAction);
	toolBar->addSeparator();
	toolBar->addAction(exportImageAction);
	toolBar->addSeparator();
	toolBar->addAction(editGradientAction);
	toolBar->addSeparator();
	toolBar->addAction(fractalExplorer->restoreInitialStateAction);
	toolBar->addSeparator();
	toolBar->addAction(fractalExplorer->stopDrawingAction);
	toolBar->addAction(fractalExplorer->refreshAction);
	toolBar->addSeparator();
	toolBar->addAction(adaptExplorerToWindowAction);

	if (adaptExplorerSize) {
		resize(lastWindowWidth, lastWindowHeight);
		adaptExplorerToWindowAction->trigger();
	}

	exportFractalImageDialog = new ExportFractalImageDialog(fractalExplorer->getFractalConfig(),
					exportImageNbThreads, imageDir, this);

	/* Eventually launch fractal drawing. */
	fractalExplorer->refresh();

	QTimer::singleShot(5, this, SLOT(delayedInit()));
}

void MainWindow::delayedInit()
{
	if (adaptExplorerSize) {
		fractalExplorer->restoreInitialState();
	}
}

MainWindow::~MainWindow()
{
	saveSettings();
	mpfr_free_cache();
}

void MainWindow::aboutQt()
{
	QApplication::aboutQt();
}

void MainWindow::aboutQFractalNow()
{
	QString applicationName(QApplication::applicationName());
	QString applicationVersion(QApplication::applicationVersion());
	QString homepage = "<a href=\"http://fractalnow.source.net\">"
				"fractalnow.sourceforge.net</a>";

	QString title(tr("About %1 %2").arg(applicationName).arg(applicationVersion));
	QString message;
	message += QString("<b><big> %1 v%2 </b></big><br><br>").arg(applicationName)
				.arg(applicationVersion);
	message += tr("%1 is a program to explore fractal sets easily and generate \
fractal images efficiently.").arg(applicationName) + "<br><br>";
	message += tr("Copyright (c) 2011-2012 Marc Pegon <pe.marc@free.fr>")
			+ "<br><br>";
	message += tr("Visit %1 for more information.").arg(homepage) + "<br><br>";
	message += tr("This program is licensed under the Lesser GNU General Public \
License.");
	QMessageBox::about(this, title, message);
}

void MainWindow::adaptExplorerToWindow(bool checked)
{
	if (checked) {
		fractalExplorer->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
		fractalExplorer->updateGeometry();
	} else {
		centralWidget()->updateGeometry();
		fractalExplorer->resize(preferredImageWidthSpinBox->value(),
					preferredImageHeightSpinBox->value());
		fractalExplorer->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
		fractalExplorer->updateGeometry();
	}
	centralWidget()->updateGeometry();
}

void MainWindow::loadConfigFile(QString fileName)
{
	FractalConfig newConfig;
	/* Set locale each time, because apparently some Qt
	 * methods change locale.
	 * For example a single call to QPixmap(QString filename)
	 * changes back locale...
	 */
	setlocale(LC_NUMERIC, "C");
	if (ReadFractalConfigFile(&newConfig, fileName.toStdString().c_str())) {
		QMessageBox::critical(this, tr("Failed to read config file"),
			tr("Error occured while reading config file."));
	} else {
		int finished = fractalExplorer->stopDrawing();
		UNUSED(finished);
		fractalExplorer->setFractalConfig(newConfig);
		/* Config is copied by fractal explorer. */
		FreeFractalConfig(newConfig);
	}
}

void MainWindow::loadFractalFile(QString fileName)
{
	Fractal newFractal;
	setlocale(LC_NUMERIC, "C");
	if (ReadFractalFile(&newFractal, fileName.toStdString().c_str())) {
		QMessageBox::critical(this, tr("Failed to read fractal file"),
			tr("Error occured while reading fractal file."));
	} else {
		int finished = fractalExplorer->stopDrawing();
		UNUSED(finished);
		fractalExplorer->setFractal(newFractal);
		/* Fractal is copied by fractal explorer. */
		FreeFractal(newFractal);
	}
}

void MainWindow::loadRenderingFile(QString fileName)
{
	RenderingParameters newRender;
	setlocale(LC_NUMERIC, "C");
	if (ReadRenderingFile(&newRender, fileName.toStdString().c_str())) {
		QMessageBox::critical(this, tr("Failed to read rendering file"),
			tr("Error occured while reading rendering file."));
	} else {
		int finished = fractalExplorer->stopDrawing();
		UNUSED(finished);
		fractalExplorer->setRenderingParameters(newRender);
		/* Rendering parameters are copied by fractal explorer. */
		FreeRenderingParameters(newRender);
	}
}

void MainWindow::loadGradientFile(QString fileName)
{
	Gradient newGradient;
	setlocale(LC_NUMERIC, "C");
	if (ReadGradientFile(&newGradient, fileName.toStdString().c_str())) {
		QMessageBox::critical(this, tr("Failed to read rendering file"),
			tr("Error occured while reading rendering file."));
	} else {
		int finished = fractalExplorer->stopDrawing();
		UNUSED(finished);
		fractalExplorer->setGradient(newGradient);
		/* Gradient is copied by fractal explorer. */
		FreeGradient(newGradient);
	}
}

void MainWindow::exportImage()
{
	fractalExplorer->pauseDrawing();

	exportFractalImageDialog->resetFractalConfig(fractalExplorer->getFractalConfig());
	exportFractalImageDialog->setFloatPrecision((FloatPrecision)
						floatTypeComboBox->currentIndex());
	if (exportFractalImageDialog->exec()) {
		imageDir = QFileInfo(exportFractalImageDialog->exportedFile()).absolutePath();
	}

	fractalExplorer->resumeDrawing();
}

void MainWindow::openConfigFile()
{
	QString fileName = QFileDialog::getOpenFileName(this, tr("Open Config File"),
				configDir,
				tr("Configuration (*.config)"));
	
	if (!fileName.isEmpty()) {
		configDir = QFileInfo(fileName).absolutePath();
		loadConfigFile(fileName);
	}
}

void MainWindow::saveConfigFile()
{
	QString fileName = QFileDialog::getSaveFileName(this, tr("Save Config File"),
				configDir + "/config.config",
				tr("Configuration (*.config)"));

	if (!fileName.isEmpty()) {
		configDir = QFileInfo(fileName).absolutePath();
		const Fractal &expFractal = fractalExplorer->getFractalConfig().fractal;
		//mpfr_t spanY;
		//mpfr_init(spanY);
		//mpfr_set(spanY, expFractal.spanX, MPFR_RNDN);
		Fractal fractal;
		InitFractal2(&fractal, expFractal.fractalFormula, expFractal.p, expFractal.c,
				expFractal.centerX, expFractal.centerY,
				expFractal.spanX, expFractal.spanY, 
				expFractal.escapeRadius, expFractal.maxIter);
		RenderingParameters render = CopyRenderingParameters(
						&fractalExplorer->getFractalConfig().render);
		FractalConfig fractalConfig;
		InitFractalConfig(&fractalConfig, fractal, render);
		if (WriteFractalConfigFile(&fractalConfig, fileName.toStdString().c_str())) {
			QMessageBox::critical(this, tr("Failed to write configuration file"),
				tr("Error occured while writing configuration file."));
		}
		//mpfr_clear(spanY);
		FreeFractalConfig(fractalConfig);
	}
}

void MainWindow::openGradientFile()
{
	QString fileName = QFileDialog::getOpenFileName(this, tr("Open Gradient File"),
				gradientDir, 
				tr("Gradient (*.gradient)"));
	if (!fileName.isEmpty()) {
		gradientDir = QFileInfo(fileName).absolutePath();
		loadGradientFile(fileName);
	}
}

void MainWindow::saveGradientFile()
{
	QString fileName = QFileDialog::getSaveFileName(this, tr("Save Gradient File"),
				gradientDir + "/gradient.gradient",
				tr("Gradient (*.gradient)"));

	if (!fileName.isEmpty()) {
		gradientDir = QFileInfo(fileName).absolutePath();
		if (WriteGradientFile(&fractalExplorer->getRender().gradient,
				fileName.toStdString().c_str())) {
			QMessageBox::critical(this, tr("Failed to write gradient file"),
				tr("Error occured while writing gradient file."));
		}
	}
}

void MainWindow::onPreferredImageWidthChanged()
{
	if (!adaptExplorerToWindowAction->isChecked()
		&& preferredImageWidthSpinBox->value() != fractalExplorer->width()) {
		fractalExplorer->resize(preferredImageWidthSpinBox->value(),
					preferredImageHeightSpinBox->value());
		centralWidget()->updateGeometry();
		if (sizeHint().width() > width()) {
			adjustSize();
		}
	}
}

void MainWindow::onPreferredImageHeightChanged()
{
	if (!adaptExplorerToWindowAction->isChecked()
		&& preferredImageHeightSpinBox->value() != fractalExplorer->height()) {
		fractalExplorer->resize(preferredImageWidthSpinBox->value(),
					preferredImageHeightSpinBox->value());
		centralWidget()->updateGeometry();
		if (sizeHint().height() > height()) {
			adjustSize();
		}
	}
}

void MainWindow::onCacheSizeChanged()
{
	fractalExplorer->resizeFractalCache(cacheSizeSpinBox->value());
}

void MainWindow::onFloatTypeChanged(int index)
{
#ifdef __ENABLE_MP_FLOATS
	if ((FloatPrecision)index == FP_MP) {
		editMPFloatPrecisionWidget->setEnabled(true);
	} else {
		editMPFloatPrecisionWidget->setEnabled(false);
	}
#else
	UNUSED(index);
#endif
}

void MainWindow::editMPFloatPrecision()
{
	bool ok;
	int new_mp_precision = QInputDialog::getInt(this, tr("Change value"), tr("MP Float Precision"),
				MPFloatPrecisionSpinBox->value(), MPFR_PREC_MIN,
				std::min<long long int>(std::numeric_limits<int>::max(),(long long int)MPFR_PREC_MAX),
				1, &ok);
	if (ok) {
		MPFloatPrecisionSpinBox->setValue(new_mp_precision);
		if (GetMPFloatPrecision() != new_mp_precision) {
			QMessageBox::information(this, tr("Restart application"),
				tr("You must restart application to apply changes."));
		}
	}
}

enum MainWindow::FileType MainWindow::getFileType(QString fileName)
{
	enum MainWindow::FileType res = MainWindow::UNKNOWN_FILE;
	std::string sFileName = fileName.toStdString();

	if (isSupportedFractalConfigFile(sFileName.c_str())) {
		res = MainWindow::CONFIG_FILE;
	} else if (isSupportedFractalFile(sFileName.c_str())) {
		res = MainWindow::FRACTAL_FILE;
	} else if (isSupportedRenderingFile(sFileName.c_str())) {
		res  = MainWindow::RENDER_FILE;
	} else if (isSupportedGradientFile(sFileName.c_str())) {
		res = MainWindow::GRADIENT_FILE;
	}

	return res;
}

void MainWindow::openFile(QString fileName) {
	enum MainWindow::FileType fileType = getFileType(fileName);

	switch (fileType) {
	case MainWindow::CONFIG_FILE:
		loadConfigFile(fileName);
		break;
	case MainWindow::FRACTAL_FILE:
		loadFractalFile(fileName);
		break;
	case MainWindow::RENDER_FILE:
		loadRenderingFile(fileName);
		break;
	case MainWindow::GRADIENT_FILE:
		loadGradientFile(fileName);
		break;
	default:
		/* This should never happen. */
		QMessageBox::critical(this, tr("Unexpected error"),
			tr("Unknown file type."));
		break;
	}
}

void MainWindow::dragEnterEvent(QDragEnterEvent *event)
{
	const QMimeData *mimeData = event->mimeData();

	if (mimeData->hasUrls()) {
		QList<QUrl> urls(event->mimeData()->urls());
		QString fileName(urls.at(0).toLocalFile());
		if (urls.size() == 1 && !fileName.isEmpty()) {
			if (getFileType(fileName) != MainWindow::UNKNOWN_FILE) {
				event->acceptProposedAction();
			}
		}
	}
}

void MainWindow::dropEvent(QDropEvent *event)
{
	if (event->mimeData()->hasUrls()) {
		QList<QUrl> urls(event->mimeData()->urls());
		QString path(urls.at(0).toLocalFile());
		openFile(path);
	}
}

void MainWindow::switchFullScreenMode(bool checked)
{
	static const QMargins margins = centralWidget()->layout()->contentsMargins();
	static bool fractalDockHidden = false;
	static bool renderDockHidden = false;
	static bool otherDockHidden = false;
	static bool toolBarHidden = false;

	if (checked) {
		fractalDockHidden = fractalDock->isHidden();
		renderDockHidden = renderDock->isHidden();
		otherDockHidden = otherDock->isHidden();
		toolBarHidden = toolBar->isHidden();
		fractalDock->hide();
		renderDock->hide();
		otherDock->hide();
		toolBar->hide();
		menuBar()->hide();
		if (!adaptExplorerToWindowAction->isChecked()) {
			adaptExplorerToWindow(true);
		}
		centralWidget()->layout()->setContentsMargins(0, 0, 0, 0);

		adaptExplorerToWindowAction->setDisabled(true);
		fractalDock->toggleViewAction()->setDisabled(true);
		renderDock->toggleViewAction()->setDisabled(true);
		otherDock->toggleViewAction()->setDisabled(true);
		toolBar->toggleViewAction()->setDisabled(true);

		showFullScreen();
	} else {
		if (!fractalDockHidden) {
			fractalDock->show();
		}
		if (!renderDockHidden) {
			renderDock->show();
		}
		if (!otherDockHidden) {
			otherDock->show();
		}
		if (!toolBarHidden) {
			toolBar->show();
		}
		menuBar()->show();
		if (!adaptExplorerToWindowAction->isChecked()) {
			adaptExplorerToWindow(false);
		}
		centralWidget()->layout()->setContentsMargins(margins);
		
		adaptExplorerToWindowAction->setEnabled(true);
		fractalDock->toggleViewAction()->setEnabled(true);
		renderDock->toggleViewAction()->setEnabled(true);
		otherDock->toggleViewAction()->setEnabled(true);
		toolBar->toggleViewAction()->setEnabled(true);

		showNormal();
	}
}

void MainWindow::escapeFullScreen()
{
	if (isFullScreen()) {
		switchFullScreenAction->trigger();
	}
}
