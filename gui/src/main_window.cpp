/*
 *  main_window.cpp -- part of fractal2D
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
#include "export_fractal_image_dialog.h"

#include "error.h"
#include "floating_point.h"
#include "fractal2D.h"
#include "fractal.h"
#include "fractal_addend_function.h"
#include "fractal_coloring.h"
#include "fractal_formula.h"
#include "fractal_counting_function.h"
#include "fractal_transfer_function.h"

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
#include <QTextCodec>
#include <QThread>
#include <QToolBar>

void initDefaultFractal(Fractal &fractal)
{
	InitFractal2(&fractal, FRAC_MANDELBROT, 2, 0, -0.7, 0, 3, 3, 1E3, 250);
}

void initDefaultRenderingParameters(RenderingParameters &render)
{
	/* Generate gradient. */
	uint32_t hex_color[5] = { 0xFF, 0xFFFFFF, 0xFFFF00, 0xFF0000, 0xFF };
	Color color[5];
	for (uint_fast32_t i = 0; i < 5; ++i) {
		color[i] = ColorFromUint32(hex_color[i]);
	}
	Gradient gradient;
	GenerateGradient2(&gradient, 5, color, DEFAULT_GRADIENT_TRANSITIONS);

	std::string identityName = "identity";
	std::string smoothName = "smooth";
	/* Init rendering parameters. */
	InitRenderingParameters(&render, 1, ColorFromUint32(0x0), CF_SMOOTH, CM_SIMPLE,
				AF_TRIANGLEINEQUALITY, 1, IM_LINEAR, TF_IDENTITY,
				2.5E-2, 0, gradient);
}

MainWindow::MainWindow(int argc, char *argv[])
{
	/* Set icon */
	this->setWindowIcon(QIcon(":/icons/icon256x256.png"));

	/* Set C numeric locale and codecs for strings. */
	QTextCodec::setCodecForCStrings(QTextCodec::codecForName("UTF-8"));
	QTextCodec::setCodecForTr(QTextCodec::codecForName("UTF-8"));
	setlocale(LC_NUMERIC, "C");

	Fractal fractal;
	RenderingParameters render;

	/* Read arguments on command-line. */
	CommandLineArguments args(argc, argv);
	if (!args.fractalFileName) {
		/* Build default fractal. */
		initDefaultFractal(fractal);
	} else {
		/* Read fractal file to initialize fractal. */
		if (ReadFractalFile(&fractal, args.fractalFileName)) {
			fractal2D_message(stderr, T_QUIET, "Failed to read fractal file. Falling back to default.\n");
			initDefaultFractal(fractal);
		}
	}
		
	if (!args.renderingFileName) {
		/* Build default rendering parameters. */
		initDefaultRenderingParameters(render);
	} else {
		/* Read rendering file to initialize rendering parameters. */
		if (ReadRenderingFile(&render, args.renderingFileName)) {
			fractal2D_message(stderr, T_QUIET, "Failed to read rendering file. Falling back to default.\n");
			initDefaultRenderingParameters(render);
		} else if (render.bytesPerComponent != 1) {
			fractal2D_message(stderr, T_QUIET, "Rendering files with more than 1 byte per component are "
						"not allowed. Falling back to default\n");
			initDefaultRenderingParameters(render);
		}
	}

	/* If both width and height were omitted : set to default. */
	if (args.width == 0 && args.height == 0) {
		args.width = DEFAULT_FRACTAL_IMAGE_WIDTH;
		args.height = DEFAULT_FRACTAL_IMAGE_HEIGHT;
	}
	if (args.width == 0) {
		/* If only width was omitted, compute it to keep ratio. */
		args.width = roundF(fractal.spanX * args.height / fractal.spanY);
	} else if (args.height == 0) {
		/* If only height was omitted, compute it to keep ratio. */
		args.height = roundF(fractal.spanY * args.width / fractal.spanX);
	} else {
		/* If both width and height were specified (or set do default),
		 * adapt spanX and spanY.
		 */
		if (args.height / (double)args.width < 1) {
			fractal.spanX = args.width * fractal.spanY / args.height;
		} else {
			fractal.spanY = args.height * fractal.spanX / args.width;
		}
		/* Reinit fractal, since we changed spanX or spanY. */
		InitFractal2(&fractal, fractal.fractalFormula, fractal.p, fractal.c,
				fractal.centerX, fractal.centerY,
				fractal.spanX, fractal.spanY, 
				fractal.escapeRadius, fractal.maxIter);
	}

	uint_fast32_t preferredExplorerWidth = args.width;
	uint_fast32_t preferredExplorerHeight = args.height;

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
	fractalExplorer = new FractalExplorer(fractal, render, preferredExplorerWidth,
					preferredExplorerHeight, args.minAntiAliasingSize,
					args.maxAntiAliasingSize, args.antiAliasingSizeIteration,
					fractalExplorerNbThreads);
	fractalExplorer->setFocus();

	centralWidget = new QWidget;
	QGridLayout *centralLayout = new QGridLayout;
	centralLayout->addWidget(fractalExplorer, 1, 1);
	fractalExplorer->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
	centralWidget->setLayout(centralLayout);
	centralWidget->updateGeometry();
	this->setCentralWidget(centralWidget);

	/* Create fractal config widget. */
	fractalConfigWidget = new FractalConfigWidget(fractal);
	/* Connect the fractal config widget spin boxes to slots. */
	connect(fractalExplorer, SIGNAL(fractalChanged(Fractal &)),
		fractalConfigWidget, SLOT(updateBoxesValues(Fractal &)));
	connect(fractalConfigWidget->fractalFormulaComboBox, SIGNAL(currentIndexChanged(int)),
		fractalExplorer, SLOT(setFractalFormula(int)));
	connect(fractalConfigWidget->pParamSpinBox, SIGNAL(valueChanged(double)),
		fractalExplorer, SLOT(setPParam(double)));
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
	connect(fractalExplorer, SIGNAL(renderingParametersChanged(RenderingParameters &)),
		fractalRenderingWidget, SLOT(updateBoxesValues(RenderingParameters &)));
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
	connect(fractalRenderingWidget->spaceColorButton, SIGNAL(currentColorChanged(QColor)),
		fractalExplorer, SLOT(setSpaceColor(QColor)));
	connect(fractalRenderingWidget->gradientBox, SIGNAL(gradientStopsChanged(
		const QGradientStops&)), fractalExplorer, SLOT(setGradient(const QGradientStops&)));
	
	QWidget *otherParametersWidget = new QWidget;
	QFormLayout *otherParamLayout = new QFormLayout;
	preferredImageWidthSpinBox = new QSpinBox;
	preferredImageWidthSpinBox->setRange(2, 2048);
	preferredImageWidthSpinBox->setButtonSymbols(QAbstractSpinBox::NoButtons);
	preferredImageWidthSpinBox->setValue((int)preferredExplorerWidth);
	connect(preferredImageWidthSpinBox, SIGNAL(editingFinished()), this,
		SLOT(onPreferredImageWidthChanged()));
	preferredImageHeightSpinBox = new QSpinBox;
	preferredImageHeightSpinBox->setRange(2, 2048);
	preferredImageHeightSpinBox->setButtonSymbols(QAbstractSpinBox::NoButtons);
	preferredImageHeightSpinBox->setValue((int)preferredExplorerHeight);
	connect(preferredImageHeightSpinBox, SIGNAL(editingFinished()), this,
		SLOT(onPreferredImageHeightChanged()));
	otherParamLayout->addRow(tr("Preferred image width:"), preferredImageWidthSpinBox);
	otherParamLayout->addRow(tr("Preferred image height:"), preferredImageHeightSpinBox);
	otherParametersWidget->setLayout(otherParamLayout);
	
	/* Create dock widgets. */
	QDockWidget *fractalDock = new QDockWidget(tr("Fractal configuration"), this);
	QDockWidget *renderDock = new QDockWidget(tr("Rendering parameters"), this);
	QDockWidget *otherDock = new QDockWidget(tr("Other"), this);
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

	QMenu *fileMenu = menuBar()->addMenu(tr("&File"));
	QAction *exportImageAction = new QAction(tr("E&xport image"), this);
	exportImageAction->setToolTip(tr("Export fractal as image"));
	exportImageAction->setIcon(QApplication::style()->standardIcon(QStyle::SP_DriveFDIcon));
	QList<QKeySequence> exportImageShortcuts;
	exportImageShortcuts << QKeySequence::Save << QKeySequence::SaveAs;
	exportImageAction->setShortcuts(exportImageShortcuts);
	connect(exportImageAction, SIGNAL(triggered()), this, SLOT(exportImage()));
	fileMenu->addAction(exportImageAction);
	QAction *quitAction = new QAction(tr("&Quit"), this);
	quitAction->setShortcut(QKeySequence::Quit);
	connect(quitAction, SIGNAL(triggered()), this, SLOT(close()));
	fileMenu->addSeparator();
	fileMenu->addAction(quitAction);

	QMenu *editMenu = menuBar()->addMenu(tr("&Edit"));
	editMenu->addAction(fractalExplorer->restoreInitialStateAction);
	editMenu->addSeparator();
	editMenu->addAction(fractalExplorer->stopDrawingAction);
	editMenu->addAction(fractalExplorer->refreshAction);
	editMenu->addSeparator();
	editMenu->addAction(fractalExplorer->zoomInAction);
	editMenu->addAction(fractalExplorer->zoomOutAction);
	editMenu->addSeparator();
	editMenu->addAction(fractalExplorer->moveLeftAction);
	editMenu->addAction(fractalExplorer->moveRightAction);
	editMenu->addAction(fractalExplorer->moveUpAction);
	editMenu->addAction(fractalExplorer->moveDownAction);

	QToolBar *toolBar = addToolBar(tr("Tool bar"));
	toolBar->setToolButtonStyle(Qt::ToolButtonTextBesideIcon);

	QMenu *viewMenu = menuBar()->addMenu(tr("&View"));
	viewMenu->addAction(fractalDock->toggleViewAction());
	viewMenu->addAction(renderDock->toggleViewAction());
	viewMenu->addAction(toolBar->toggleViewAction());
	viewMenu->addSeparator();
	adaptExplorerToWindowAction = new QAction(tr("Adapt to &window"), this);
	adaptExplorerToWindowAction->setToolTip(tr("Adapt fractal explorer size to window"));
	adaptExplorerToWindowAction->setCheckable(true);
	adaptExplorerToWindowAction->setChecked(false);
	adaptExplorerToWindowAction->setShortcut(Qt::Key_F);
	connect(adaptExplorerToWindowAction, SIGNAL(toggled(bool)), this,
		SLOT(adaptExplorerToWindow(bool)));
	viewMenu->addAction(adaptExplorerToWindowAction);

	QMenu *helpMenu = menuBar()->addMenu(tr("&Help"));
	QAction *aboutQtAction = new QAction(tr("About &Qt"), this);
	connect(aboutQtAction, SIGNAL(triggered()), this, SLOT(aboutQt()));
	helpMenu->addAction(aboutQtAction);

	QAction *aboutQFractal2DAction = new QAction(tr("&About qFractal2D"), this);
	connect(aboutQFractal2DAction, SIGNAL(triggered()), this, SLOT(aboutQFractal2D()));
	helpMenu->addAction(aboutQFractal2DAction);

	toolBar->addAction(exportImageAction);
	toolBar->addSeparator();
	toolBar->addAction(fractalExplorer->restoreInitialStateAction);
	toolBar->addSeparator();
	toolBar->addAction(fractalExplorer->stopDrawingAction);
	toolBar->addAction(fractalExplorer->refreshAction);
	toolBar->addSeparator();
	toolBar->addAction(adaptExplorerToWindowAction);

	/* Free rendering parameters (copied by fractal explorer). */
	FreeRenderingParameters(render);

	/* Eventually launch fractal drawing. */
	fractalExplorer->refresh();
}

void MainWindow::aboutQt()
{
	QApplication::aboutQt();
}

void MainWindow::aboutQFractal2D()
{
	QString title(tr("About qFractal2D") + QApplication::applicationVersion());
	QString message(tr("<b><big>qFractal2D v") + QApplication::applicationVersion()
	+ tr("</b></big><br><br>qFractal2D is a program to explore fractal sets easily and \
generate fractal images efficiently.<br><br>\
Copyright (c) 2012 Marc Pegon <pe.marc@free.fr><br><br>\
This program is licensed under the Lesser GNU General Public License."));
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
	centralWidget->updateGeometry();
}

void MainWindow::exportImage()
{
	int finished = fractalExplorer->stopDrawing();
	QString fileName = QFileDialog::getSaveFileName(this, tr("Export image"),
				QDesktopServices::storageLocation(QDesktopServices::PicturesLocation),
				tr("Images (*.png *.jpg *.tiff *.ppm)"));

	if (!fileName.isNull()) {
		ExportFractalImageDialog dialog(fileName, fractalExplorer->getFractal(),
						fractalExplorer->getRender(),
						exportImageNbThreads, this);
		dialog.exec();
	}

	if (!finished) {
		fractalExplorer->refresh();
	}
}

void MainWindow::onPreferredImageWidthChanged()
{
	if (!adaptExplorerToWindowAction->isChecked()
		&& preferredImageWidthSpinBox->value() != fractalExplorer->width()) {
		fractalExplorer->resize(preferredImageWidthSpinBox->value(),
					preferredImageHeightSpinBox->value());
		centralWidget->updateGeometry();
		if (sizeHint().width() > size().width()) {
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
		centralWidget->updateGeometry();
		if (sizeHint().height() > size().height()) {
			adjustSize();
		}
	}
}

