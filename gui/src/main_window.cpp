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
#include <QDockWidget>
#include <QTextCodec>
#include <QtConcurrentRun>
#include <QRect>
#include <QPainter>

MainWindow::MainWindow(int argc, char *argv[])
{
	/* Set C numeric locale and codecs for strings. */
	QTextCodec::setCodecForCStrings(QTextCodec::codecForName("UTF-8"));
	QTextCodec::setCodecForTr(QTextCodec::codecForName("UTF-8"));
	setlocale(LC_NUMERIC, "C");

	/* Read arguments on command-line. */
	args = new CommandLineArguments(argc, argv);
	if (!args->fractalFileName) {
		/* Build default fractal. */
		InitFractal2(&fractal, FRAC_MANDELBROT, 2, 0, -0.7, 0, 3, 3, 1E3, 250);
	} else {
		/* Read fractal file to initialize fractal. */
		ReadFractalFile(&fractal, args->fractalFileName);
	}
		
	if (!args->renderingFileName) {
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
					1E3, 0, gradient);
	} else {
		/* Read rendering file to initialize rendering parameters. */
		ReadRenderingFile(&render, args->renderingFileName);
		if (render.bytesPerComponent != 1) {
			error("Rendering files with more than 1 byte per component are not accepted.\n");
		}
	}

	/* Compute omitted width or height from spanX and spanY to keep aspect ratio. */
	if (args->width == 0) {
		args->width = roundF(fractal.spanX * args->height / fractal.spanY);
	} else if (args->height == 0) {
		args->height = roundF(fractal.spanY * args->width / fractal.spanX);
	}

	/* Create QImage and Image (from fractal lib) */
	qimage = new QImage(args->width, args->height, QImage::Format_RGB32);
	CreateImage2(&image, qimage->bits(), args->width, args->height, 1);
	/* Create image label which will be the central widget. */
	imageLabel = new ImageLabel;
	imageLabel->setImage(qimage);
	this->setCentralWidget(imageLabel);

	/* Create fractal config widget. */
	fractalConfigWidget = new FractalConfigWidget(fractal);
	/* Connect the fractal config widget spin boxes to slots. */
	connect(fractalConfigWidget->fractalFormulaComboBox, SIGNAL(currentIndexChanged(int)),
		this, SLOT(fractalFormulaChanged(int)));
	connect(fractalConfigWidget->pParamSpinBox, SIGNAL(valueChanged(double)),
		this, SLOT(pParamChanged(double)));
	connect(fractalConfigWidget->cParamReSpinBox, SIGNAL(valueChanged(double)),
		this, SLOT(cParamReChanged(double)));
	connect(fractalConfigWidget->cParamImSpinBox, SIGNAL(valueChanged(double)),
		this, SLOT(cParamImChanged(double)));
	connect(fractalConfigWidget->centerXSpinBox, SIGNAL(valueChanged(double)),
		this, SLOT(centerXChanged(double)));
	connect(fractalConfigWidget->centerYSpinBox, SIGNAL(valueChanged(double)),
		this, SLOT(centerYChanged(double)));
	connect(fractalConfigWidget->spanXSpinBox, SIGNAL(valueChanged(double)),
		this, SLOT(spanXChanged(double)));
	connect(fractalConfigWidget->bailoutRadiusSpinBox, SIGNAL(valueChanged(double)),
		this, SLOT(bailoutRadiusChanged(double)));
	connect(fractalConfigWidget->maxIterationsSpinBox, SIGNAL(valueChanged(int)),
		this, SLOT(maxIterationsChanged(int)));

	/* Create rendering config widget.*/
	fractalRenderingWidget = new FractalRenderingWidget(render);
	/* Connect the fractal rendering widget combo and spin boxes to slots. */
	connect(fractalRenderingWidget->addendFunctionComboBox, SIGNAL(currentIndexChanged(int)),
		this, SLOT(addendFunctionChanged(int)));
	connect(fractalRenderingWidget->stripeDensitySpinBox, SIGNAL(valueChanged(int)),
		this, SLOT(stripeDensityChanged(int)));
	connect(fractalRenderingWidget->countingFunctionComboBox, SIGNAL(currentIndexChanged(int)),
		this, SLOT(countingFunctionChanged(int)));
	connect(fractalRenderingWidget->coloringMethodComboBox, SIGNAL(currentIndexChanged(int)),
		this, SLOT(coloringMethodChanged(int)));
	connect(fractalRenderingWidget->interpolationMethodComboBox, SIGNAL(currentIndexChanged(int)),
		this, SLOT(interpolationMethodChanged(int)));
	connect(fractalRenderingWidget->transferFunctionComboBox, SIGNAL(currentIndexChanged(int)),
		this, SLOT(transferFunctionChanged(int)));
	connect(fractalRenderingWidget->colorScalingSpinBox, SIGNAL(valueChanged(double)),
		this, SLOT(colorScalingChanged(double)));
	connect(fractalRenderingWidget->colorOffsetSpinBox, SIGNAL(valueChanged(double)),
		this, SLOT(colorOffsetChanged(double)));
	
	/* Create Tab widget */
	//QTabWidget *tabWidget = new QTabWidget;
	//tabWidget->addTab(fractalConfigWidget, QString("Fractal configuration"));
	//tabWidget->addTab(fractalRenderingWidget, QString("Fractal rendering"));

	/* Create dock widgets. */
	QDockWidget *fractalDock = new QDockWidget(QString("Fractal configuration"), this);
	QDockWidget *renderDock = new QDockWidget(QString("Rendering parameters"), this);
	/* Set dock widget properties. */
	fractalDock->setWidget(fractalConfigWidget);
	fractalDock->setAllowedAreas(Qt::LeftDockWidgetArea | Qt::RightDockWidgetArea);
	fractalDock->setFeatures(QDockWidget::DockWidgetMovable | QDockWidget::DockWidgetFloatable);
	renderDock->setWidget(fractalRenderingWidget);
	renderDock->setAllowedAreas(Qt::LeftDockWidgetArea | Qt::RightDockWidgetArea);
	renderDock->setFeatures(QDockWidget::DockWidgetMovable | QDockWidget::DockWidgetFloatable);
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

	/* Connect fractal drawing & antialiasing watchers to slots. */
	connect(&fractalDrawingWatcher, SIGNAL(finished()), this, SLOT(fractalDrawingFinished()));
	connect(&fractalAntialiasingWatcher, SIGNAL(finished()), this, SLOT(fractalAntiAliasingFinished()));

	/* Create timer to repaint image label regularly. */
	timer = new QTimer(this);
	connect(timer, SIGNAL(timeout()), imageLabel, SLOT(update()));
	timer->start(50);

	action = DoNothingAction();
	/* Eventually launch fractal drawing. */
	launchFractalDrawing();
}

MainWindow::~MainWindow()
{
	delete args;
	FreeRenderingParameters(render);
	FreeImage(image);
	FreeAction(*action);
	free(action);
}

void MainWindow::closeEvent(QCloseEvent * event)
{
	if (!future.isFinished()) {
		CancelAction(action); /* Send cancelation request to drawing/antialiasing action.*/
		int res = future.result(); /* Wait for it to finish. */
		(void)res;
	}

	event->accept();
}

void MainWindow::reInitFractal()
{
	InitFractal2(&fractal, fractal.fractalFormula, fractal.p, fractal.c,
			fractal.centerX, fractal.centerY,
			fractal.spanX, fractal.spanY, 
			fractal.escapeRadius, fractal.maxIter);
}

void MainWindow::reInitRenderingParameters()
{
	InitRenderingParameters(&render, 1, ColorFromUint32(0x0), render.countingFunction,
				render.coloringMethod, render.addendFunction, render.stripeDensity,
				render.interpolationMethod, render.transferFunction,
				render.multiplier, render.offset, render.gradient);
}

void MainWindow::launchFractalDrawing()
{
	redrawAfterCancelation = 0;
	/* Free previous action. Safe even for first launching
	 * because action has been initialized to doNothingAction().
	 */
	FreeAction(*action);
	free(action);
	action = LaunchDrawFractalFast(&image, &fractal, &render, DEFAULT_QUAD_INTERPOLATION_SIZE,
				DEFAULT_COLOR_DISSIMILARITY_THRESHOLD);

	future = QtConcurrent::run(WaitForActionTermination, action);
	fractalDrawingWatcher.setFuture(future);
}

void MainWindow::launchFractalAntiAliasing()
{
	redrawAfterCancelation = 0;
	FreeAction(*action);
	free(action);
	action = LaunchAntiAliaseFractal(&image, &fractal, &render, currentAntiAliasingSize, DEFAULT_ADAPTIVE_AAM_THRESHOLD);

	future = QtConcurrent::run(WaitForActionTermination, action);
	fractalAntialiasingWatcher.setFuture(future);
}

inline int MainWindow::cancelActionIfNotFinished()
{
	int finished = future.isFinished();
	if (!finished) {
		redrawAfterCancelation = 1;
		CancelAction(action); 
		future.waitForFinished();
	}
	return finished;
}

void MainWindow::fractalDrawingFinished()
{
	if (future.result() == 0) {
		currentAntiAliasingSize = args->minAntiAliasingSize;
		launchFractalAntiAliasing();
	} else {
		if (redrawAfterCancelation) {
			launchFractalDrawing();
		}
	}
}

void MainWindow::fractalAntiAliasingFinished()
{
	if (future.result() == 0) {
		if (currentAntiAliasingSize < args->maxAntiAliasingSize) {
			currentAntiAliasingSize++;
			launchFractalAntiAliasing();
		}
	} else {
		if (redrawAfterCancelation) {
			launchFractalDrawing();
		}
	}
}

void MainWindow::fractalFormulaChanged(int index)
{
	int alreadyFinished = cancelActionIfNotFinished();

	fractal.fractalFormula = (FractalFormula)index;
	reInitFractal();

	/* If action was already finished, we need to explicitely launch fractal drawing.
	 * In the other case, the slot xxxFinished will be called naturally (since action
	 * was canceled and just finished), and will relaunch drawing.
	 */
	if (alreadyFinished) launchFractalDrawing();
}

void MainWindow::pParamChanged(double value)
{
	int alreadyFinished = cancelActionIfNotFinished();

	fractal.p = value;
	reInitFractal();

	if (alreadyFinished) launchFractalDrawing();
}

void MainWindow::cParamReChanged(double value)
{
	int alreadyFinished = cancelActionIfNotFinished();

	fractal.c = value + I*cimagF(fractal.c);
	reInitFractal();

	if (alreadyFinished) launchFractalDrawing();
}

void MainWindow::cParamImChanged(double value)
{
	int alreadyFinished = cancelActionIfNotFinished();

	fractal.c = crealF(fractal.c) + I*value;
	reInitFractal();

	if (alreadyFinished) launchFractalDrawing();
}

void MainWindow::centerXChanged(double value)
{
	int alreadyFinished = cancelActionIfNotFinished();

	double dx = value - fractal.centerX;
	if (fractal.spanX == 0 || fabs(dx) >= fractal.spanX) {
		qimage->fill(0);
	} else {
		QImage copiedImage(qimage->copy());
		qimage->fill(0);
		QPainter painter(qimage);
		painter.setRenderHint(QPainter::SmoothPixmapTransform);
		dx = qimage->width() * dx / fractal.spanX;
		QRectF srcRect = QRectF(std::max((double)0, dx), 0, image.width - fabs(dx), image.height);
		QRectF dstRect = QRectF(std::max((double)0, -dx), 0, image.width - fabs(dx), image.height);
		painter.drawImage(dstRect, copiedImage, srcRect);
	}

	fractal.centerX = value;
	reInitFractal();

	if (alreadyFinished) launchFractalDrawing();
}

void MainWindow::centerYChanged(double value)
{
	int alreadyFinished = cancelActionIfNotFinished();

	double dy = value - fractal.centerY;
	if (fractal.spanY == 0 || fabs(dy) >= fractal.spanY) {
		qimage->fill(0);
	} else {
		QImage copiedImage(qimage->copy());
		qimage->fill(0);
		QPainter painter(qimage);
		painter.setRenderHint(QPainter::SmoothPixmapTransform);
		dy = qimage->height() * dy / fractal.spanY;
		QRectF srcRect = QRectF(0, std::max((double)0, dy), image.width, image.height - fabs(dy));
		QRectF dstRect = QRectF(0, std::max((double)0, -dy), image.width, image.height - fabs(dy));
		painter.drawImage(dstRect, copiedImage, srcRect);
	}

	fractal.centerY = value;
	reInitFractal();

	if (alreadyFinished) launchFractalDrawing();
}

void MainWindow::spanXChanged(double value)
{
	int alreadyFinished = cancelActionIfNotFinished();
	
	if (value > fractal.spanX) {
		/* Zoom out */
		QImage copiedImage(qimage->copy());
		qimage->fill(0);
		QPainter painter(qimage);
		painter.setRenderHint(QPainter::SmoothPixmapTransform);
		QRect srcRect(qimage->rect());
		QRectF dstRect(0, 0, srcRect.width() * fractal.spanX / value, srcRect.height() * fractal.spanX / value); 
		dstRect.moveCenter(srcRect.center()+QPoint(1,1));
		painter.drawImage(dstRect, copiedImage, srcRect);
	} else {
		/* Zoom in */
		QImage copiedImage(qimage->copy());
		qimage->fill(0);
		QPainter painter(qimage);
		painter.setRenderHint(QPainter::SmoothPixmapTransform);
		QRect dstRect(qimage->rect());
		QRectF srcRect(0, 0, dstRect.width() * value / fractal.spanX, dstRect.height() * value / fractal.spanX); 
		srcRect.moveCenter(dstRect.center()+QPoint(1,1));
		painter.drawImage(dstRect, copiedImage, srcRect);
	}
	fractal.spanX = value;
	if (image.width == 0) {
		fractal.spanY = value;
	} else {
		fractal.spanY = value * image.height / image.width;
	}
	reInitFractal();

	if (alreadyFinished) launchFractalDrawing();
}

void MainWindow::bailoutRadiusChanged(double value)
{
	int alreadyFinished = cancelActionIfNotFinished();
	
	fractal.escapeRadius = value;
	reInitFractal();

	if (alreadyFinished) launchFractalDrawing();
}

void MainWindow::maxIterationsChanged(int value)
{
	int alreadyFinished = cancelActionIfNotFinished();
	
	fractal.maxIter = value;
	reInitFractal();

	if (alreadyFinished) launchFractalDrawing();
}

void MainWindow::addendFunctionChanged(int index)
{
	int alreadyFinished = cancelActionIfNotFinished();

	render.addendFunction = (AddendFunction)index;
	reInitRenderingParameters();

	if (alreadyFinished) launchFractalDrawing();
}

void MainWindow::stripeDensityChanged(int value)
{
	int alreadyFinished = cancelActionIfNotFinished();

	render.stripeDensity = value;
	reInitRenderingParameters();

	if (alreadyFinished) launchFractalDrawing();
}

void MainWindow::coloringMethodChanged(int index)
{
	int alreadyFinished = cancelActionIfNotFinished();

	render.coloringMethod = (ColoringMethod)index;
	reInitRenderingParameters();

	if (alreadyFinished) launchFractalDrawing();
}

void MainWindow::countingFunctionChanged(int index)
{
	int alreadyFinished = cancelActionIfNotFinished();

	render.countingFunction = (CountingFunction)index;
	reInitRenderingParameters();

	if (alreadyFinished) launchFractalDrawing();
}

void MainWindow::interpolationMethodChanged(int index)
{
	int alreadyFinished = cancelActionIfNotFinished();

	render.interpolationMethod = (InterpolationMethod)index;
	reInitRenderingParameters();

	if (alreadyFinished) launchFractalDrawing();
}

void MainWindow::transferFunctionChanged(int index)
{
	int alreadyFinished = cancelActionIfNotFinished();

	render.transferFunction = (TransferFunction)index;
	reInitRenderingParameters();

	if (alreadyFinished) launchFractalDrawing();
}

void MainWindow::colorScalingChanged(double value)
{
	int alreadyFinished = cancelActionIfNotFinished();

	render.multiplier = value;
	reInitRenderingParameters();

	if (alreadyFinished) launchFractalDrawing();
}

void MainWindow::colorOffsetChanged(double value)
{
	int alreadyFinished = cancelActionIfNotFinished();

	render.offset = value;
	reInitRenderingParameters();

	if (alreadyFinished) launchFractalDrawing();
}

