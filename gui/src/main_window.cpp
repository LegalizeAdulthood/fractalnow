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

#include "error.h"
#include "floating_point.h"
#include "fractalnow.h"
#include "fractal.h"
#include "fractal_addend_function.h"
#include "fractal_coloring.h"
#include "fractal_config.h"
#include "fractal_counting_function.h"
#include "fractal_formula.h"
#include "fractal_rendering_parameters.h"
#include "fractal_transfer_function.h"
#include "misc.h"

#include <clocale>
#include <iostream>

#include <QAction>
#include <QApplication>
#include <QDockWidget>
#include <QFileDialog>
#include <QFormLayout>
#include <QGridLayout>
#include <QMenuBar>
#include <QMessageBox>
#include <QSettings>
#include <QTextCodec>
#include <QTimer>
#include <QThread>
#include <QToolBar>
#include <QUrl>

void initDefaultFractal(Fractal &fractal)
{
	InitFractal(&fractal, FRAC_MANDELBROT, 2, 0.4+I*0.25, -0.7, 0, 3, 3, 1E3, 1000);
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
	InitRenderingParameters(&render, 1, ColorFromUint32(0x0), CF_SMOOTH,
				CM_ITERATIONCOUNT, AF_TRIANGLEINEQUALITY, 1,
				IM_LINEAR, TF_LOG, 4.5E-1, 0.2, gradient);
}

inline void initDefaultConfig(FractalConfig &config)
{
	initDefaultFractal(config.fractal);
	initDefaultRenderingParameters(config.render);
}

void MainWindow::loadSettings()
{
	QSettings settings;

	imageDir = settings.value("imageDir", QDesktopServices::storageLocation(
			QDesktopServices::PicturesLocation)).toString();
	configDir = settings.value("configDir", QDesktopServices::storageLocation(
			QDesktopServices::DocumentsLocation)).toString();
	gradientDir = settings.value("gradientDir", QDesktopServices::storageLocation(
			QDesktopServices::DocumentsLocation)).toString();
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
	QTextCodec::setCodecForCStrings(QTextCodec::codecForName("UTF-8"));
	QTextCodec::setCodecForTr(QTextCodec::codecForName("UTF-8"));
	setlocale(LC_NUMERIC, "C");

	/* Load settings. */
	loadSettings();

	FractalConfig config;
	Fractal &fractal = config.fractal;
	RenderingParameters &render = config.render;

	/* Read arguments on command-line. */
	CommandLineArguments args(argc, argv);
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
	}
	if (args.renderingFileName) {
		/* Read rendering file. */
		RenderingParameters newRender;
		if (ReadRenderingFile(&newRender, args.renderingFileName)) {
			FractalNow_message(stderr, T_QUIET, "Failed to read rendering file. Falling back to default.\n");
			initDefaultRenderingParameters(newRender);
		}
		ResetRenderingParameters(&config, newRender);
	}
	if (args.gradientFileName) {
		/* Read gradient file. */
		Gradient newGradient;
		if (ReadGradientFile(&newGradient, args.gradientFileName)) {
			FractalNow_message(stderr, T_QUIET, "Failed to read gradient file. Falling back to default.\n");
			initDefaultGradient(newGradient);
		}
		ResetGradient(&render, newGradient);
	}
	if (config.render.bytesPerComponent == 2) {
		QMessageBox::critical(this, tr("Gradient color depth is 16 bits."),
			tr("16-bits gradient will be converted to 8-bits gradient."));
		ResetGradient(&config.render, Gradient8(&config.render.gradient));
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
		explorerWidth = roundF(fractal.spanX * explorerHeight / fractal.spanY);
	} else if (explorerHeight == 0) {
		/* If only height was omitted, compute it to keep ratio. */
		explorerHeight = roundF(fractal.spanY * explorerWidth / fractal.spanX);
	} else {
		/* If both width and height were specified (or set do default),
		 * adapt spanX and spanY.
		 */
		if (explorerHeight / (double)explorerWidth < 1) {
			fractal.spanX = explorerWidth * fractal.spanY / explorerHeight;
		} else {
			fractal.spanY = explorerHeight * fractal.spanX / explorerWidth;
		}
		/* Reinit fractal, since we changed spanX or spanY. */
		InitFractal(&fractal, fractal.fractalFormula, fractal.p, fractal.c,
				fractal.centerX, fractal.centerY,
				fractal.spanX, fractal.spanY, 
				fractal.escapeRadius, fractal.maxIter);
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
	connect(fractalConfigWidget->pParamReSpinBox, SIGNAL(valueChanged(double)),
		fractalExplorer, SLOT(setPParamRe(double)));
	connect(fractalConfigWidget->pParamImSpinBox, SIGNAL(valueChanged(double)),
		fractalExplorer, SLOT(setPParamIm(double)));
	connect(fractalConfigWidget->cParamReSpinBox, SIGNAL(valueChanged(double)),
		fractalExplorer, SLOT(setCParamRe(double)));
	connect(fractalConfigWidget->cParamImSpinBox, SIGNAL(valueChanged(double)),
		fractalExplorer, SLOT(setCParamIm(double)));
	connect(fractalConfigWidget->centerXSpinBox, SIGNAL(valueChanged(double)),
		fractalExplorer, SLOT(setCenterX(double)));
	connect(fractalConfigWidget->centerYSpinBox, SIGNAL(valueChanged(double)),
		fractalExplorer, SLOT(setCenterY(double)));
	connect(fractalConfigWidget->spanXSpinBox, SIGNAL(valueChanged(double)),
		fractalExplorer, SLOT(setSpanX(double)));
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
	connect(fractalRenderingWidget->countingFunctionComboBox, SIGNAL(currentIndexChanged(int)),
		fractalExplorer, SLOT(setCountingFunction(int)));
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

	QWidget *otherParametersWidget = new QWidget;
	QFormLayout *otherParamLayout = new QFormLayout;
	preferredImageWidthSpinBox = new QSpinBox;
	preferredImageWidthSpinBox->setRange(2, 2048);
	preferredImageWidthSpinBox->setButtonSymbols(QAbstractSpinBox::NoButtons);
	preferredImageWidthSpinBox->setValue((int)lastPreferredExplorerWidth);
	connect(preferredImageWidthSpinBox, SIGNAL(editingFinished()), this,
		SLOT(onPreferredImageWidthChanged()));
	preferredImageHeightSpinBox = new QSpinBox;
	preferredImageHeightSpinBox->setRange(2, 2048);
	preferredImageHeightSpinBox->setButtonSymbols(QAbstractSpinBox::NoButtons);
	preferredImageHeightSpinBox->setValue((int)lastPreferredExplorerHeight);
	connect(preferredImageHeightSpinBox, SIGNAL(editingFinished()), this,
		SLOT(onPreferredImageHeightChanged()));
	solidGuessingCheckBox = new QCheckBox;
	solidGuessingCheckBox->setChecked(fractalExplorer->getSolidGuessingEnabled());
	connect(solidGuessingCheckBox, SIGNAL(clicked(bool)), fractalExplorer, SLOT(setSolidGuessingEnabled(bool)));
	useCacheCheckBox = new QCheckBox;
	useCacheCheckBox->setChecked(fractalExplorer->getFractalCacheEnabled());
	connect(useCacheCheckBox, SIGNAL(clicked(bool)), fractalExplorer, SLOT(useFractalCache(bool)));
	cacheSizeSpinBox = new QSpinBox;
	cacheSizeSpinBox->setRange(0, 10000000);
	cacheSizeSpinBox->setButtonSymbols(QAbstractSpinBox::NoButtons);
	cacheSizeSpinBox->setValue(fractalExplorer->getFractalCacheSize());
	cacheSizeSpinBox->setEnabled(useCacheCheckBox->isChecked());
	connect(cacheSizeSpinBox, SIGNAL(editingFinished()), this, SLOT(onCacheSizeChanged()));
	connect(useCacheCheckBox, SIGNAL(clicked(bool)), cacheSizeSpinBox, SLOT(setEnabled(bool)));
	otherParamLayout->addRow(tr("Preferred image width:"), preferredImageWidthSpinBox);
	otherParamLayout->addRow(tr("Preferred image height:"), preferredImageHeightSpinBox);
	otherParamLayout->addRow(tr("Solid guessing:"), solidGuessingCheckBox);
	otherParamLayout->addRow(tr("Use cache:"), useCacheCheckBox);
	otherParamLayout->addRow(tr("Cache size:"), cacheSizeSpinBox);
	connect(fractalExplorer, SIGNAL(fractalCacheSizeChanged(int)),
		cacheSizeSpinBox, SLOT(setValue(int)));
	otherParametersWidget->setLayout(otherParamLayout);
	
	/* Create dock widgets. */
	fractalDock = new QDockWidget(tr("Fractal configuration"), this);
	renderDock = new QDockWidget(tr("Rendering parameters"), this);
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
}

void MainWindow::aboutQt()
{
	QApplication::aboutQt();
}

void MainWindow::aboutQFractalNow()
{
	QString applicationName(QApplication::applicationName());
	QString applicationVersion(QApplication::applicationVersion());
	QString homepage = "<a href=\"http://sourceforge.net/projects/fractalnow/\">"
				"sourceforge.net/projects/fractalnow</a>";

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
	} else {
		fractalExplorer->resize(preferredImageWidthSpinBox->value(),
					preferredImageHeightSpinBox->value());
		fractalExplorer->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
	}
	centralWidget()->updateGeometry();
}

void MainWindow::loadConfigFile(const char *fileName)
{
	FractalConfig newConfig;
	/* Set locale each time, because apparently some Qt
	 * methods change locale.
	 * For example a single call to QPixmap(QString filename)
	 * changes back locale...
	 */
	setlocale(LC_NUMERIC, "C");
	if (ReadFractalConfigFile(&newConfig, fileName)) {
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

void MainWindow::loadFractalFile(const char *fileName)
{
	Fractal newFractal;
	setlocale(LC_NUMERIC, "C");
	if (ReadFractalFile(&newFractal, fileName)) {
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

void MainWindow::loadRenderingFile(const char *fileName)
{
	RenderingParameters newRender;
	setlocale(LC_NUMERIC, "C");
	if (ReadRenderingFile(&newRender, fileName)) {
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

void MainWindow::loadGradientFile(const char *fileName)
{
	Gradient newGradient;
	setlocale(LC_NUMERIC, "C");
	if (ReadGradientFile(&newGradient, fileName)) {
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
		loadConfigFile(fileName.toStdString().c_str());
	}
}

void MainWindow::saveConfigFile()
{
	QString fileName = QFileDialog::getSaveFileName(this, tr("Save Config File"),
				configDir + "/config.config",
				tr("Configuration (*.config)"));

	if (!fileName.isEmpty()) {
		configDir = QFileInfo(fileName).absolutePath();
		FractalConfig fractalConfig = CopyFractalConfig(&fractalExplorer->getFractalConfig());
		Fractal &fractal = fractalConfig.fractal;
		fractal.spanY = fractal.spanX;
		/* Reinit fractal, since we changed spanY. */
		InitFractal(&fractal, fractal.fractalFormula, fractal.p, fractal.c,
				fractal.centerX, fractal.centerY,
				fractal.spanX, fractal.spanY, 
				fractal.escapeRadius, fractal.maxIter);
		if (WriteFractalConfigFile(&fractalConfig, fileName.toStdString().c_str())) {
			QMessageBox::critical(this, tr("Failed to write configuration file"),
				tr("Error occured while writing configuration file."));
		}
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
		loadGradientFile(fileName.toStdString().c_str());
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

enum MainWindow::FileType MainWindow::getFileType(const char *fileName)
{
	enum MainWindow::FileType res = MainWindow::UNKNOWN_FILE;

	if (isSupportedFractalConfigFile(fileName)) {
		res = MainWindow::CONFIG_FILE;
	} else if (isSupportedFractalFile(fileName)) {
		res = MainWindow::FRACTAL_FILE;
	} else if (isSupportedRenderingFile(fileName)) {
		res  = MainWindow::RENDER_FILE;
	} else if (isSupportedGradientFile(fileName)) {
		res = MainWindow::GRADIENT_FILE;
	}

	return res;
}

void MainWindow::openFile(QString fileName) {
	const char *cFileName = fileName.toStdString().c_str();
	enum MainWindow::FileType fileType = getFileType(cFileName);

	switch (fileType) {
	case MainWindow::CONFIG_FILE:
		loadConfigFile(cFileName);
		break;
	case MainWindow::FRACTAL_FILE:
		loadFractalFile(cFileName);
		break;
	case MainWindow::RENDER_FILE:
		loadRenderingFile(cFileName);
		break;
	case MainWindow::GRADIENT_FILE:
		loadGradientFile(cFileName);
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
			const char *cFileName = fileName.toStdString().c_str();
			if (getFileType(cFileName) != MainWindow::UNKNOWN_FILE) {
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
