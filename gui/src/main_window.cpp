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
#include <QtConcurrentRun>
#include <QRect>
#include <QPainter>
#include <QThread>
#include <QVector2D>

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
					2.5E-2, 0, gradient);
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
	fractalQImage = new QImage(args->width, args->height, QImage::Format_RGB32);
	CreateImage2(&fractalImage, fractalQImage->bits(), args->width, args->height, 1);
	/* Create fractalImage label which will be the central widget. */
	fractalImageLabel = new FractalImageLabel;
	fractalImageLabel->setImage(fractalQImage);
	fractalImageLabel->setFocus();
	/* Connect FractalImageLabel signals to slots.
	 * Note that all modifications on fractal/rendering parameters
	 * are done here in MainWindow.
	 * Don't know how to do otherwise : for image label to be able
	 * to modify fractal, it would need to know about action and
	 * fractalConfigWidget spinboxes..
	 */
	connect(fractalImageLabel, SIGNAL(mousePressed(QMouseEvent *)), this, SLOT(fractalImageLabelMousePressed(QMouseEvent *)));
	connect(fractalImageLabel, SIGNAL(mouseReleased(QMouseEvent *)), this, SLOT(fractalImageLabelMouseReleased(QMouseEvent *)));
	connect(fractalImageLabel, SIGNAL(mouseMoved(QMouseEvent *)), this, SLOT(fractalImageLabelMouseMoved(QMouseEvent *)));
	connect(fractalImageLabel, SIGNAL(mouseWheelRotated(QWheelEvent *)), this, SLOT(fractalImageLabelMouseWheelRotated(QWheelEvent *)));
	connect(fractalImageLabel, SIGNAL(customContextMenuRequested(const QPoint&)),
		this, SLOT(showImageLabelContextMenu(const QPoint&)));
	QWidget *centralWidget = new QWidget;
	QGridLayout *centralLayout = new QGridLayout;
	centralLayout->addWidget(fractalImageLabel, 1, 1);
	centralLayout->setColumnStretch(0, 1);
	centralLayout->setColumnStretch(2, 1);
	centralLayout->setRowStretch(0, 1);
	centralLayout->setRowStretch(2, 1);
	centralWidget->setLayout(centralLayout);
	this->setCentralWidget(centralWidget);

	movingFractalDeferred = 0;
	fractalMoved = 0;

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
	connect(fractalRenderingWidget->spaceColorButton, SIGNAL(currentColorChanged(QColor)),
		this, SLOT(spaceColorChanged(QColor)));
	
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

	zoomInAction = new QAction(tr("Zoom in"), this);
	QList<QKeySequence> zoomInShortcuts;
	zoomInShortcuts << Qt::Key_Plus << QKeySequence::ZoomIn;
	zoomInAction->setShortcuts(zoomInShortcuts);
	zoomOutAction = new QAction(tr("Zoom out"), this);
	QList<QKeySequence> zoomOutShortcuts;
	zoomOutShortcuts << Qt::Key_Minus << QKeySequence::ZoomOut;
	zoomOutAction->setShortcuts(zoomOutShortcuts);
	moveLeftAction = new QAction(tr("Move left"), this);
	moveLeftAction->setShortcut(Qt::Key_Left);
	moveRightAction = new QAction(tr("Move right"), this);
	moveRightAction->setShortcut(Qt::Key_Right);
	moveUpAction = new QAction(tr("Move up"), this);
	moveUpAction->setShortcut(Qt::Key_Up);
	moveDownAction = new QAction(tr("Move down"), this);
	moveDownAction->setShortcut(Qt::Key_Down);
	connect(zoomInAction, SIGNAL(triggered()), this, SLOT(zoomInFractal()));
	connect(zoomOutAction, SIGNAL(triggered()), this, SLOT(zoomOutFractal()));
	connect(moveLeftAction, SIGNAL(triggered()), this, SLOT(moveLeftFractal()));
	connect(moveRightAction, SIGNAL(triggered()), this, SLOT(moveRightFractal()));
	connect(moveUpAction, SIGNAL(triggered()), this, SLOT(moveUpFractal()));
	connect(moveDownAction, SIGNAL(triggered()), this, SLOT(moveDownFractal()));
	//QMenu *fileMenu = menuBar()->addMenu(tr("&File"));
	QMenu *editMenu = menuBar()->addMenu(tr("&Edit"));
	QMenu *viewMenu = menuBar()->addMenu(tr("&View"));
	editMenu->addAction(zoomInAction);
	editMenu->addAction(zoomOutAction);
	editMenu->addAction(moveLeftAction);
	editMenu->addAction(moveRightAction);
	editMenu->addAction(moveUpAction);
	editMenu->addAction(moveDownAction);
	viewMenu->addAction(fractalDock->toggleViewAction());
	viewMenu->addAction(renderDock->toggleViewAction());

	/* Connect fractal drawing & antialiasing watchers to slots. */
	connect(&fractalDrawingWatcher, SIGNAL(resultReadyAt(int)), this, SLOT(onFractalDrawingFinished()), Qt::DirectConnection);
	connect(&fractalAntialiasingWatcher, SIGNAL(resultReadyAt(int)), this, SLOT(onFractalAntiAliasingFinished()), Qt::DirectConnection);

	connect(this, SIGNAL(wakeUpSignal()), this, SLOT(wakeUpIfAsleep()), Qt::QueuedConnection);

	/* Create timer to repaint fractalImageLabel regularly. */
	timer = new QTimer(this);
	connect(timer, SIGNAL(timeout()), fractalImageLabel, SLOT(update()));
	timer->start(50);

	action = DoNothingAction();
	/* Eventually launch fractal drawing. */
	launchFractalDrawing();
}

MainWindow::~MainWindow()
{
	delete args;
	FreeRenderingParameters(render);
	FreeImage(fractalImage);
	FreeAction(*action);
	free(action);
}

void MainWindow::closeEvent(QCloseEvent * event)
{
	if (!future.isFinished()) {
		CancelAction(action); /* Send cancelation request to drawing/antialiasing action.*/
		future.waitForFinished(); /* Wait for it to finish. */
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
	InitRenderingParameters(&render, render.bytesPerComponent, render.spaceColor,
				render.countingFunction, render.coloringMethod,
				render.addendFunction, render.stripeDensity,
				render.interpolationMethod, render.transferFunction,
				render.multiplier, render.offset, render.gradient);
}

/* Assumes that action is finished.*/
void MainWindow::launchFractalDrawing()
{
	/* Free previous action. Safe even for first launching
	 * because action has been initialized to doNothingAction().
	 */
	FreeAction(*action);
	free(action);
	action = LaunchDrawFractalFast(&fractalImage, &fractal, &render, DEFAULT_QUAD_INTERPOLATION_SIZE,
				DEFAULT_COLOR_DISSIMILARITY_THRESHOLD);

	future = QtConcurrent::run(WaitForActionTermination, action);
	fractalDrawingWatcher.setFuture(future);
}

/* Assumes that action is finished.*/
void MainWindow::launchFractalAntiAliasing()
{
	FreeAction(*action);
	free(action);
	action = LaunchAntiAliaseFractal(&fractalImage, &fractal, &render, currentAntiAliasingSize, DEFAULT_ADAPTIVE_AAM_THRESHOLD);

	future = QtConcurrent::run(WaitForActionTermination, action);
	fractalAntialiasingWatcher.setFuture(future);
}

void MainWindow::cancelActionIfNotFinished()
{
	int finished = future.isFinished();
	if (!finished) {
		CancelAction(action); 
		future.waitForFinished();
	}
}

void MainWindow::wakeUpIfAsleep()
{
	if (future.isFinished()) {
		launchFractalDrawing();
	} else {
		cancelActionIfNotFinished();
		emit wakeUpIfAsleep();
	}
}

void MainWindow::onFractalDrawingFinished()
{
	/* We need to be sure here that this slot is called
	 * right after future is finished !
	 * That is why we use a queued signal for wake up :
	 * we don't want the wake up slot to be executed before
	 * this.
	 */
	if (future.result() == 0) {
		currentAntiAliasingSize = args->minAntiAliasingSize;
		launchFractalAntiAliasing();
	}
}

void MainWindow::onFractalAntiAliasingFinished()
{
	/* Same comment as onFractalDrawingFinished. */
	if (future.result() == 0) {
		if (currentAntiAliasingSize < args->maxAntiAliasingSize) {
			currentAntiAliasingSize++;
			launchFractalAntiAliasing();
		}
	}
}

void MainWindow::moveFractal(double dx, double dy)
{
	cancelActionIfNotFinished();

	if (fractal.spanX == 0 || fractal.spanY == 0
		|| fabs(dx) >= fractal.spanX || fabs(dy) > fractal.spanY) {
		fractalQImage->fill(0);
	} else {
		QImage copiedImage(fractalQImage->copy());
		fractalQImage->fill(0);
		QPainter painter(fractalQImage);
		painter.setRenderHint(QPainter::SmoothPixmapTransform);
		double ddx = fractalQImage->width() * dx / fractal.spanX;
		double ddy = fractalQImage->height() * dy / fractal.spanY;
		QRectF srcRect = QRectF(std::max((double)0, ddx), std::max((double)0, ddy), fractalImage.width - fabs(ddx), fractalImage.height - fabs(ddy));
		QRectF dstRect = QRectF(std::max((double)0, -ddx), std::max((double)0, -ddy), fractalImage.width - fabs(ddx), fractalImage.height - fabs(ddy));
		painter.drawImage(dstRect, copiedImage, srcRect);

		fractal.centerX += dx;
		fractal.centerY += dy;
		fractalConfigWidget->blockBoxesSignals(true);
		fractalConfigWidget->centerXSpinBox->setValue(fractal.centerX);
		fractalConfigWidget->centerYSpinBox->setValue(fractal.centerY);
		fractalConfigWidget->updateSpaceBoxesSingleSteps();
		fractalConfigWidget->blockBoxesSignals(false);
	}

	reInitFractal();
	emit wakeUpSignal();
}

void MainWindow::moveLeftFractal()
{
	double dx = -fractalConfigWidget->centerYSpinBox->singleStep();
	moveFractal(dx, 0);
}

void MainWindow::moveRightFractal()
{
	double dx = fractalConfigWidget->centerYSpinBox->singleStep();
	moveFractal(dx, 0);
}

void MainWindow::moveUpFractal()
{
	double dy = -fractalConfigWidget->centerYSpinBox->singleStep();
	moveFractal(0, dy);
}

void MainWindow::moveDownFractal()
{
	double dy = fractalConfigWidget->centerYSpinBox->singleStep();
	moveFractal(0, dy);
}

void MainWindow::zoomInFractal(double newSpanX, double zoomCenterX, double zoomCenterY)
{
	cancelActionIfNotFinished();

	if (fractal.spanX == 0 || fractal.spanY == 0) {
		fractalQImage->fill(0);
	} else {
		double newSpanY = newSpanX * fractalImage.height / fractalImage.width;
		double newCenterX = zoomCenterX + newSpanX * (fractal.centerX - zoomCenterX) / fractal.spanX;
		double newCenterY = zoomCenterY + newSpanY * (fractal.centerY - zoomCenterY) / fractal.spanY;

		QImage copiedImage(fractalQImage->copy());
		fractalQImage->fill(0);
		QPainter painter(fractalQImage);
		painter.setRenderHint(QPainter::SmoothPixmapTransform);
		QRect dstRect(fractalQImage->rect());
		QRectF srcRect(0, 0, dstRect.width() * newSpanX / fractal.spanX, dstRect.height() * newSpanX / fractal.spanX); 
		srcRect.moveCenter(QPointF((fractalQImage->width())*(newCenterX-fractal.x1)/fractal.spanX,(fractalQImage->height())*(newCenterY-fractal.y1)/fractal.spanY));
		painter.drawImage(dstRect, copiedImage, srcRect);

		fractal.centerX= newCenterX;
		fractal.centerY= newCenterY;
		fractal.spanX = newSpanX;
		fractal.spanY = newSpanY;

		fractalConfigWidget->blockBoxesSignals(true);
		fractalConfigWidget->centerXSpinBox->setValue(newCenterX);
		fractalConfigWidget->centerYSpinBox->setValue(newCenterY);
		fractalConfigWidget->spanXSpinBox->setValue(newSpanX);
		fractalConfigWidget->updateSpaceBoxesSingleSteps();
		fractalConfigWidget->blockBoxesSignals(false);
	}

	reInitFractal();
	emit wakeUpSignal();
}

void MainWindow::zoomOutFractal(double newSpanX, double zoomCenterX, double zoomCenterY)
{
	cancelActionIfNotFinished();

	if (fractal.spanX == 0 || fractal.spanY == 0) {
		fractalQImage->fill(0);
	} else {
		double newSpanY = newSpanX * fractalImage.height / fractalImage.width;
		double newCenterX = zoomCenterX + newSpanX * (fractal.centerX - zoomCenterX) / fractal.spanX;
		double newCenterY = zoomCenterY + newSpanY * (fractal.centerY - zoomCenterY) / fractal.spanY;

		QImage copiedImage(fractalQImage->copy());
		fractalQImage->fill(0);
		QPainter painter(fractalQImage);
		painter.setRenderHint(QPainter::SmoothPixmapTransform);
		QRect srcRect(fractalQImage->rect());
		QRectF dstRect(0, 0, srcRect.width() * fractal.spanX / newSpanX, srcRect.height() * fractal.spanX / newSpanX); 
		dstRect.moveCenter(QPointF(srcRect.width()*(fractal.centerX-(newCenterX-newSpanX/2)) / newSpanX, srcRect.height()*(fractal.centerY-(newCenterY-newSpanY/2)) / newSpanY));

		painter.drawImage(dstRect, copiedImage, srcRect);

		fractal.centerX= newCenterX;
		fractal.centerY= newCenterY;
		fractal.spanX = newSpanX;
		fractal.spanY = newSpanY;

		fractalConfigWidget->blockBoxesSignals(true);
		fractalConfigWidget->centerXSpinBox->setValue(newCenterX);
		fractalConfigWidget->centerYSpinBox->setValue(newCenterY);
		fractalConfigWidget->spanXSpinBox->setValue(newSpanX);
		fractalConfigWidget->updateSpaceBoxesSingleSteps();
		fractalConfigWidget->blockBoxesSignals(false);
	}

	reInitFractal();
	emit wakeUpSignal();
}

void MainWindow::zoomInFractal()
{
	QDoubleSpinBox *spanXSpinBox = fractalConfigWidget->spanXSpinBox;
	double newSpanX = fractal.spanX - spanXSpinBox->singleStep();
	zoomInFractal(newSpanX, fractal.centerX, fractal.centerY);
}

void MainWindow::zoomOutFractal()
{
	QDoubleSpinBox *spanXSpinBox = fractalConfigWidget->spanXSpinBox;
	double newSpanX = fractal.spanX + spanXSpinBox->singleStep();
	zoomOutFractal(newSpanX, fractal.centerX, fractal.centerY);
}

void MainWindow::fractalFormulaChanged(int index)
{
	cancelActionIfNotFinished();

	fractal.fractalFormula = (FractalFormula)index;
	reInitFractal();

	emit wakeUpSignal();
}

void MainWindow::pParamChanged(double value)
{
	cancelActionIfNotFinished();

	fractal.p = value;
	reInitFractal();

	emit wakeUpSignal();
}

void MainWindow::cParamReChanged(double value)
{
	cancelActionIfNotFinished();

	fractal.c = value + I*cimagF(fractal.c);
	reInitFractal();

	emit wakeUpSignal();
}

void MainWindow::cParamImChanged(double value)
{
	cancelActionIfNotFinished();

	fractal.c = crealF(fractal.c) + I*value;
	reInitFractal();

	emit wakeUpSignal();
}

void MainWindow::centerXChanged(double value)
{
	double dx = value - fractal.centerX;
	moveFractal(dx, 0);
}

void MainWindow::centerYChanged(double value)
{
	double dy = value - fractal.centerY;
	moveFractal(0, dy);
}

void MainWindow::spanXChanged(double value)
{
	if (value > fractal.spanX) {
		/* Zoom out */
		zoomOutFractal(value, fractal.centerX, fractal.centerY);
	} else {
		/* Zoom in */
		zoomInFractal(value, fractal.centerX, fractal.centerY);
	}
}

void MainWindow::bailoutRadiusChanged(double value)
{
	cancelActionIfNotFinished();
	
	fractal.escapeRadius = value;
	reInitFractal();

	emit wakeUpSignal();
}

void MainWindow::maxIterationsChanged(int value)
{
	cancelActionIfNotFinished();
	
	fractal.maxIter = value;
	reInitFractal();

	emit wakeUpSignal();
}

void MainWindow::addendFunctionChanged(int index)
{
	cancelActionIfNotFinished();

	render.addendFunction = (AddendFunction)index;
	reInitRenderingParameters();

	emit wakeUpSignal();
}

void MainWindow::stripeDensityChanged(int value)
{
	cancelActionIfNotFinished();

	render.stripeDensity = value;
	reInitRenderingParameters();

	emit wakeUpSignal();
}

void MainWindow::coloringMethodChanged(int index)
{
	cancelActionIfNotFinished();

	render.coloringMethod = (ColoringMethod)index;
	reInitRenderingParameters();

	emit wakeUpSignal();
}

void MainWindow::countingFunctionChanged(int index)
{
	cancelActionIfNotFinished();

	render.countingFunction = (CountingFunction)index;
	reInitRenderingParameters();

	emit wakeUpSignal();
}

void MainWindow::interpolationMethodChanged(int index)
{
	cancelActionIfNotFinished();

	render.interpolationMethod = (InterpolationMethod)index;
	reInitRenderingParameters();

	emit wakeUpSignal();
}

void MainWindow::transferFunctionChanged(int index)
{
	cancelActionIfNotFinished();

	render.transferFunction = (TransferFunction)index;
	reInitRenderingParameters();

	emit wakeUpSignal();
}

void MainWindow::colorScalingChanged(double value)
{
	cancelActionIfNotFinished();

	render.multiplier = value;
	reInitRenderingParameters();

	emit wakeUpSignal();
}

void MainWindow::colorOffsetChanged(double value)
{
	cancelActionIfNotFinished();

	render.offset = value;
	reInitRenderingParameters();

	emit wakeUpSignal();
}

void MainWindow::spaceColorChanged(QColor color)
{
	cancelActionIfNotFinished();

	render.spaceColor = ColorFromRGB(1, (uint16_t)color.red(), (uint16_t)color.green(), (uint16_t)color.blue());
	reInitRenderingParameters();

	emit wakeUpSignal();
}

void MainWindow::fractalImageLabelMousePressed(QMouseEvent *event)
{
	if (fractalMoved) {
		emit wakeUpSignal();
	}
	movingFractalDeferred = 0;
	movingFractalRealTime = 0;
	fractalMoved = 0;
	QApplication::restoreOverrideCursor();

	switch (event->button()) {
	case Qt::LeftButton:
		movingFractalDeferred = 1;
		fractalCenterXOnPress = fractal.centerX;
		fractalCenterYOnPress = fractal.centerY;
		mousePosOnPress = event->posF();
		QApplication::setOverrideCursor(Qt::ClosedHandCursor);
		break;
	case Qt::MidButton:
		movingFractalRealTime = 1;
		QApplication::setOverrideCursor(Qt::ClosedHandCursor);
		break;
	default:
		QMainWindow::mousePressEvent(event);
	}
	prevMousePos = event->posF();
}

void MainWindow::fractalImageLabelMouseReleased(QMouseEvent *event)
{
	switch (event->button()) {
	case Qt::LeftButton:
		if (movingFractalDeferred) {
			if (fractalMoved) {
				emit wakeUpSignal();
			}
			movingFractalDeferred = 0;
			fractalMoved = 0;
			QApplication::restoreOverrideCursor();
		}
		break;
	case Qt::MidButton:
		if (movingFractalRealTime) {
			movingFractalRealTime = 0;
			QApplication::restoreOverrideCursor();
		}
		break;
	default:
		QMainWindow::mouseReleaseEvent(event);
	}
}

void MainWindow::fractalImageLabelMouseMoved(QMouseEvent *event)
{
	if (movingFractalDeferred) {
		if (!fractalMoved) {
			cancelActionIfNotFinished();
			imageCopyOnPress = fractalQImage->copy();
		}
		fractalMoved = 1;
		QVector2D vect(event->posF()-mousePosOnPress);
		double dx = vect.x() * fractal.spanX / fractalImage.width;
		double dy = vect.y() * fractal.spanY / fractalImage.height;
		fractalQImage->fill(0);
		QPainter painter(fractalQImage);
		painter.setRenderHint(QPainter::SmoothPixmapTransform);
		double ddx = vect.x();
		double ddy = vect.y();
		QRectF srcRect = QRectF(std::max((double)0, -ddx), std::max((double)0, -ddy), fractalImage.width - fabs(ddx), fractalImage.height - fabs(ddy));
		QRectF dstRect = QRectF(std::max((double)0, ddx), std::max((double)0, ddy), fractalImage.width - fabs(ddx), fractalImage.height - fabs(ddy));
		painter.drawImage(dstRect, imageCopyOnPress, srcRect);

		fractal.centerX = fractalCenterXOnPress-dx;
		fractal.centerY = fractalCenterYOnPress-dy;
		fractalConfigWidget->blockBoxesSignals(true);
		fractalConfigWidget->centerXSpinBox->setValue(fractal.centerX);
		fractalConfigWidget->centerYSpinBox->setValue(fractal.centerY);
		fractalConfigWidget->updateSpaceBoxesSingleSteps();
		fractalConfigWidget->blockBoxesSignals(false);

		reInitFractal();
	} else if (movingFractalRealTime) {
		QVector2D vect(event->posF()-prevMousePos);
		double dx = vect.x() * fractal.spanX / fractalImage.width;
		double dy = vect.y() * fractal.spanY / fractalImage.height;
		moveFractal(-dx, -dy);
		prevMousePos = event->posF();
	} else {
		QMainWindow::mouseMoveEvent(event);
	}
}

void MainWindow::fractalImageLabelMouseWheelRotated(QWheelEvent *event)
{
	if (movingFractalDeferred) {
		if (fractalMoved) {
			emit wakeUpSignal();
		}
		movingFractalDeferred = 0;
		fractalMoved = 0;
		QApplication::restoreOverrideCursor();
	}

	double numDegrees = event->delta() / 8.;
	double numSteps = numDegrees / 15;

	if (event->orientation() == Qt::Vertical) {
		QDoubleSpinBox *spanXSpinBox = fractalConfigWidget->spanXSpinBox;
		double newSpanX = fractal.spanX-numSteps*spanXSpinBox->singleStep();
		double zoomCenterX = (event->x() + 0.5) * fractal.spanX / fractalImage.width + fractal.x1;
		double zoomCenterY = (event->y() + 0.5) * fractal.spanY / fractalImage.height + fractal.y1;
		if (numSteps > 0) {
			zoomInFractal(newSpanX, zoomCenterX, zoomCenterY);
		} else {
			zoomOutFractal(newSpanX, zoomCenterX, zoomCenterY);
		}
	}
	event->accept();
}

void MainWindow::showImageLabelContextMenu(const QPoint& pos)
{
	QPoint globalPos = fractalImageLabel->mapToGlobal(pos);
	QMenu menu(fractalImageLabel);
	menu.addAction(zoomInAction);
	menu.addAction(zoomOutAction);
	menu.addAction(moveLeftAction);
	menu.addAction(moveRightAction);
	menu.addAction(moveUpAction);
	menu.addAction(moveDownAction);
	menu.exec(globalPos);
}

