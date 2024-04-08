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
#include "error.h"
#include "floating_point.h"
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
#include <QGridLayout>
#include <QMenuBar>
#include <QTextCodec>
#include <QThread>

MainWindow::MainWindow(int argc, char *argv[])
{
	/* Set C numeric locale and codecs for strings. */
	QTextCodec::setCodecForCStrings(QTextCodec::codecForName("UTF-8"));
	QTextCodec::setCodecForTr(QTextCodec::codecForName("UTF-8"));
	setlocale(LC_NUMERIC, "C");

	/* Set number of threads.
	 * We decide to launch more threads that the actual number
	 * of cores, because some parts of fractalImage may need much
	 * more calculations than others, so we may have one thread
	 * taking much more time that the others, which is not good
	 * (action terminates when all thread terminate).
	 * With more threads, we "average" the time taken by each
	 * thread.
	 */
	if (QThread::idealThreadCount() > 0) {
		nbThreads = 8*QThread::idealThreadCount();
	}

	Fractal fractal;
	RenderingParameters render;

	/* Read arguments on command-line. */
	CommandLineArguments args(argc, argv);
	if (!args.fractalFileName) {
		/* Build default fractal. */
		InitFractal2(&fractal, FRAC_MANDELBROT, 2, 0, -0.7, 0, 3, 3, 1E3, 250);
	} else {
		/* Read fractal file to initialize fractal. */
		ReadFractalFile(&fractal, args.fractalFileName);
	}
		
	if (!args.renderingFileName) {
		/* Build default rendering parameters. */
		/* Generate gradient. */
		uint32_t hex_color[5] = { 0xFF, 0xFFFFFF, 0xFFFF00, 0xFF0000, 0xFF };
		Color color[5];
		for (uint_fast32_t i = 0; i < 5; ++i) {
			color[i] = ColorFromUint32(hex_color[i]);
		}
		Gradient gradient;
		GenerateGradient(&gradient, color, 5, 10000);

		std::string identityName = "identity";
		std::string smoothName = "smooth";
		/* Init rendering parameters. */
		InitRenderingParameters(&render, 1, ColorFromUint32(0x0), CF_SMOOTH, CM_SIMPLE,
					AF_TRIANGLEINEQUALITY, 1, IM_LINEAR, TF_IDENTITY,
					2.5E-2, 0, gradient);
	} else {
		/* Read rendering file to initialize rendering parameters. */
		ReadRenderingFile(&render, args.renderingFileName);
		if (render.bytesPerComponent != 1) {
			error("Rendering files with more than 1 byte per component are not accepted.\n");
		}
	}

	/* Compute omitted width or height from spanX and spanY to keep aspect ratio. */
	if (args.width == 0) {
		args.width = roundF(fractal.spanX * args.height / fractal.spanY);
	} else if (args.height == 0) {
		args.height = roundF(fractal.spanY * args.width / fractal.spanX);
	}

	/* Create fractalImage label which will be the central widget. */
	fractalExplorer = new FractalExplorer(fractal, render, args.width, args.height, args.minAntiAliasingSize, args.maxAntiAliasingSize, args.antiAliasingSizeIteration);
	fractalExplorer->setFocus();

	QWidget *centralWidget = new QWidget;
	QGridLayout *centralLayout = new QGridLayout;
	centralLayout->addWidget(fractalExplorer, 1, 1);
	centralLayout->setColumnStretch(0, 1);
	centralLayout->setColumnStretch(2, 1);
	centralLayout->setRowStretch(0, 1);
	centralLayout->setRowStretch(2, 1);
	centralWidget->setLayout(centralLayout);
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
	
	/* Create dock widgets. */
	QDockWidget *fractalDock = new QDockWidget(tr("Fractal configuration"), this);
	QDockWidget *renderDock = new QDockWidget(tr("Rendering parameters"), this);
	/* Set dock widget properties. */
	fractalDock->setWidget(fractalConfigWidget);
	fractalDock->setAllowedAreas(Qt::LeftDockWidgetArea | Qt::RightDockWidgetArea);
	renderDock->setWidget(fractalRenderingWidget);
	renderDock->setAllowedAreas(Qt::LeftDockWidgetArea | Qt::RightDockWidgetArea);
	/* Add dock widgets. */
	this->addDockWidget(Qt::RightDockWidgetArea, fractalDock);
	this->addDockWidget(Qt::RightDockWidgetArea, renderDock);
	setTabPosition(Qt::LeftDockWidgetArea | Qt::RightDockWidgetArea, QTabWidget::North);
	tabifyDockWidget(fractalDock, renderDock);
	/* Set current tabified dock to fractaldock. */
	QList<QTabBar *> tabList = findChildren<QTabBar *>();
	if (!tabList.isEmpty()) {
		QTabBar *tabBar = tabList.at(0);
		tabBar->setCurrentIndex(0);
	}

	//QMenu *fileMenu = menuBar()->addMenu(tr("&File"));
	QMenu *editMenu = menuBar()->addMenu(tr("&Edit"));
	QMenu *viewMenu = menuBar()->addMenu(tr("&View"));
	editMenu->addAction(fractalExplorer->zoomInAction);
	editMenu->addAction(fractalExplorer->zoomOutAction);
	editMenu->addAction(fractalExplorer->moveLeftAction);
	editMenu->addAction(fractalExplorer->moveRightAction);
	editMenu->addAction(fractalExplorer->moveUpAction);
	editMenu->addAction(fractalExplorer->moveDownAction);
	viewMenu->addAction(fractalDock->toggleViewAction());
	viewMenu->addAction(renderDock->toggleViewAction());
}

void MainWindow::closeEvent(QCloseEvent * event)
{
	fractalExplorer->cancelActionIfNotFinished();

	event->accept();
}

