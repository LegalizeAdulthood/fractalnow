/*
 *  fractal_explorer.cpp -- part of fractal2D
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
 
#include "fractal_explorer.h"
#include <QApplication>
#include <QMenu>
#include <QtConcurrentRun>
#include <QPainter>
#include <QTimer>
#include <QVector2D>

FractalExplorer::FractalExplorer(const Fractal &fractal, const RenderingParameters &render,
					uint_fast32_t width, uint_fast32_t height,
					uint_fast32_t minAntiAliasingSize,
					uint_fast32_t maxAntiAliasingSize,
					uint_fast32_t antiAliasingSizeIteration) : QLabel()
{
	setCursor(QCursor(Qt::OpenHandCursor));
	//setContextMenuPolicy(Qt::CustomContextMenu);
	setContextMenuPolicy(Qt::ActionsContextMenu);

	this->fractal = fractal;
	this->render = render;
	this->minAntiAliasingSize = minAntiAliasingSize;
	this->maxAntiAliasingSize = maxAntiAliasingSize;
	this->antiAliasingSizeIteration = antiAliasingSizeIteration;

	movingFractalDeferred = false;
	movingFractalRealTime = false;
	fractalMoved = false;

	/* Create QImage and Image (from fractal lib) */
	fractalQImage = new QImage(width, height, QImage::Format_RGB32);
	CreateImage2(&fractalImage, fractalQImage->bits(), width, height, 1);

	zoomInAction = new QAction(tr("Zoom in"), this);
	QList<QKeySequence> zoomInShortcuts;
	zoomInShortcuts << Qt::Key_Plus << QKeySequence::ZoomIn;
	zoomInAction->setShortcuts(zoomInShortcuts);
	addAction(zoomInAction);
	zoomOutAction = new QAction(tr("Zoom out"), this);
	QList<QKeySequence> zoomOutShortcuts;
	zoomOutShortcuts << Qt::Key_Minus << QKeySequence::ZoomOut;
	zoomOutAction->setShortcuts(zoomOutShortcuts);
	addAction(zoomOutAction);
	moveLeftAction = new QAction(tr("Move left"), this);
	moveLeftAction->setShortcut(Qt::Key_Left);
	addAction(moveLeftAction);
	moveRightAction = new QAction(tr("Move right"), this);
	moveRightAction->setShortcut(Qt::Key_Right);
	addAction(moveRightAction);
	moveUpAction = new QAction(tr("Move up"), this);
	moveUpAction->setShortcut(Qt::Key_Up);
	addAction(moveUpAction);
	moveDownAction = new QAction(tr("Move down"), this);
	moveDownAction->setShortcut(Qt::Key_Down);
	addAction(moveDownAction);
	connect(zoomInAction, SIGNAL(triggered()), this, SLOT(zoomInFractal()));
	connect(zoomOutAction, SIGNAL(triggered()), this, SLOT(zoomOutFractal()));
	connect(moveLeftAction, SIGNAL(triggered()), this, SLOT(moveLeftFractal()));
	connect(moveRightAction, SIGNAL(triggered()), this, SLOT(moveRightFractal()));
	connect(moveUpAction, SIGNAL(triggered()), this, SLOT(moveUpFractal()));
	connect(moveDownAction, SIGNAL(triggered()), this, SLOT(moveDownFractal()));

	/* Connect fractal drawing & antialiasing watchers to slots. */
	connect(&fractalDrawingWatcher, SIGNAL(resultReadyAt(int)), this, SLOT(onFractalDrawingFinished()), Qt::DirectConnection);
	connect(&fractalAntialiasingWatcher, SIGNAL(resultReadyAt(int)), this, SLOT(onFractalAntiAliasingFinished()), Qt::DirectConnection);

	connect(this, SIGNAL(wakeUpSignal()), this, SLOT(wakeUpIfAsleep()), Qt::QueuedConnection);

	/* Create timer to repaint fractalImageLabel regularly. */
	timer = new QTimer(this);
	connect(timer, SIGNAL(timeout()), this, SLOT(update()));
	timer->start(50);

	action = DoNothingAction();
	/* Eventually launch fractal drawing. */
	launchFractalDrawing();
}

FractalExplorer::~FractalExplorer()
{
	FreeRenderingParameters(render);
	FreeImage(fractalImage);
	FreeAction(*action);
	free(action);
}

void FractalExplorer::paintEvent(QPaintEvent *event)
{
	(void)event;
	setFocusPolicy(Qt::StrongFocus);

	if (fractalQImage != NULL) {
		QPainter painter(this);
		painter.setRenderHint(QPainter::SmoothPixmapTransform);
		painter.drawImage(fractalQImage->rect(), *fractalQImage, fractalQImage->rect());
	}
}

QSize FractalExplorer::sizeHint() const
{
	if (fractalQImage == NULL) {
		return QSize(0, 0);
	} else {
		return fractalQImage->size();
	}
}

QSize FractalExplorer::minimumSizeHint() const
{
	if (fractalQImage == NULL) {
		return QSize(0, 0);
	} else {
		return fractalQImage->size();
	}
}

void FractalExplorer::reInitFractal()
{
	InitFractal2(&fractal, fractal.fractalFormula, fractal.p, fractal.c,
			fractal.centerX, fractal.centerY,
			fractal.spanX, fractal.spanY, 
			fractal.escapeRadius, fractal.maxIter);
}

void FractalExplorer::reInitRenderingParameters()
{
	InitRenderingParameters(&render, render.bytesPerComponent, render.spaceColor,
				render.countingFunction, render.coloringMethod,
				render.addendFunction, render.stripeDensity,
				render.interpolationMethod, render.transferFunction,
				render.multiplier, render.offset, render.gradient);
}

/* Assumes that action is finished.*/
void FractalExplorer::launchFractalDrawing()
{
	/* Free previous action. Safe even for first launching
	 * because action has been initialized to doNothingAction().
	 */
	FreeAction(*action);
	free(action);
	action = LaunchDrawFractalFast(&fractalImage, &fractal, &render, DEFAULT_QUAD_INTERPOLATION_SIZE,
				DEFAULT_COLOR_DISSIMILARITY_THRESHOLD);

	future = QtConcurrent::run(WaitForFinished, action);
	fractalDrawingWatcher.setFuture(future);
}

/* Assumes that action is finished.*/
void FractalExplorer::launchFractalAntiAliasing()
{
	FreeAction(*action);
	free(action);
	action = LaunchAntiAliaseFractal(&fractalImage, &fractal, &render, currentAntiAliasingSize, DEFAULT_ADAPTIVE_AAM_THRESHOLD);

	future = QtConcurrent::run(WaitForFinished, action);
	fractalAntialiasingWatcher.setFuture(future);
}

void FractalExplorer::cancelActionIfNotFinished()
{
	int finished = future.isFinished();
	if (!finished) {
		CancelAction(action); 
		future.waitForFinished();
	}
}

void FractalExplorer::wakeUpIfAsleep()
{
	if (future.isFinished()) {
		launchFractalDrawing();
	} else {
		cancelActionIfNotFinished();
		emit wakeUpIfAsleep();
	}
}

void FractalExplorer::onFractalDrawingFinished()
{
	/* We need to be sure here that this slot is called
	 * right after future is finished !
	 * That is why we use a queued signal for wake up :
	 * we don't want the wake up slot to be executed before
	 * this.
	 */
	if (future.result() == 0) {
		currentAntiAliasingSize = minAntiAliasingSize;
		launchFractalAntiAliasing();
	}
}

void FractalExplorer::onFractalAntiAliasingFinished()
{
	/* Same comment as onFractalDrawingFinished. */
	if (future.result() == 0) {
		if (currentAntiAliasingSize < maxAntiAliasingSize) {
			currentAntiAliasingSize += antiAliasingSizeIteration;
			if (currentAntiAliasingSize > maxAntiAliasingSize) {
				currentAntiAliasingSize = maxAntiAliasingSize;
			}
			launchFractalAntiAliasing();
		}
	}
}

void FractalExplorer::moveFractal(double dx, double dy, bool emitFractalChanged)
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
	}

	fractal.centerX += dx;
	fractal.centerY += dy;
	reInitFractal();
	if (emitFractalChanged) {
		emit fractalChanged(fractal);
	}
	emit wakeUpSignal();
}

void FractalExplorer::moveLeftFractal()
{
	double dx;
	if (fractal.spanX == 0) {
		dx = -MIN_SINGLE_STEP;
	} else {
		/* We use modf to have a multiple of image width,
		 * so that the temporary qimage matches the
		 * fractal image that will be computed exactly.
		 */
		modf(-0.2 * fractalImage.width, &dx);
		dx *= fractal.spanX / fractalImage.width;
	}
	moveFractal(dx, 0, true);
}

void FractalExplorer::moveRightFractal()
{
	double dx;
	if (fractal.spanX == 0) {
		dx = MIN_SINGLE_STEP;
	} else {
		modf(0.2 * fractalImage.width, &dx);
		dx *= fractal.spanX / fractalImage.width;
	}
	moveFractal(dx, 0, true);
}

void FractalExplorer::moveUpFractal()
{
	double dy;
	if (fractal.spanY == 0) {
		dy = -MIN_SINGLE_STEP;
	} else {
		modf(-0.2 * fractalImage.height, &dy);
		dy *= fractal.spanY / fractalImage.height;
	}
	moveFractal(0, dy, true);
}

void FractalExplorer::moveDownFractal()
{
	double dy;
	if (fractal.spanY == 0) {
		dy = MIN_SINGLE_STEP;
	} else {
		modf(0.2 * fractalImage.height, &dy);
		dy *= fractal.spanY / fractalImage.height;
	}
	moveFractal(0, dy, true);
}

void FractalExplorer::zoomInFractal(double newSpanX, double zoomCenterX, double zoomCenterY, bool emitFractalChanged)
{
	cancelActionIfNotFinished();

	if (newSpanX == 0) {
		fractalQImage->fill(0);
		fractal.spanX = 0;
		fractal.spanY = 0;
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
	}
	reInitFractal();
	if (emitFractalChanged) {
		emit fractalChanged(fractal);
	}
	emit wakeUpSignal();
}

void FractalExplorer::zoomOutFractal(double newSpanX, double zoomCenterX, double zoomCenterY, bool emitFractalChanged)
{
	cancelActionIfNotFinished();

	if (newSpanX == 0) {
		fractalQImage->fill(0);
		fractal.spanX = 0;
		fractal.spanY = 0;
	} else {
		double newSpanY = newSpanX * fractalImage.height / fractalImage.width;
		double newCenterX, newCenterY;
		if (fractal.spanX == 0 || fractal.spanY == 0) {
			newCenterX = zoomCenterX;
			newCenterY = zoomCenterY;
		} else {
			newCenterX = zoomCenterX + newSpanX * (fractal.centerX - zoomCenterX) / fractal.spanX;
			newCenterY = zoomCenterY + newSpanY * (fractal.centerY - zoomCenterY) / fractal.spanY;
		}

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

	}
	reInitFractal();
	if (emitFractalChanged) {
		emit fractalChanged(fractal);
	}
	emit wakeUpSignal();
}

void FractalExplorer::zoomInFractal()
{
	double newSpanX;
	if (fractal.spanX == 0) {
		newSpanX = MIN_SINGLE_STEP;
	} else {
		modf((1 - 0.3) * fractalImage.width, &newSpanX);
		newSpanX *= fractal.spanX / fractalImage.width;
	}
	zoomInFractal(newSpanX, fractal.centerX, fractal.centerY, true);
}

void FractalExplorer::zoomOutFractal()
{
	double newSpanX;
	if (fractal.spanX == 0) {
		newSpanX = MIN_SINGLE_STEP;
	} else {
		modf((1 + 0.3) * fractalImage.width, &newSpanX);
		newSpanX *= fractal.spanX / fractalImage.width;
	}
	zoomOutFractal(newSpanX, fractal.centerX, fractal.centerY, true);
}

void FractalExplorer::mousePressEvent(QMouseEvent *event)
{
	if (fractalMoved) {
		emit wakeUpSignal();
		emit fractalChanged(fractal);
	}
	movingFractalDeferred = false;
	movingFractalRealTime = false;
	fractalMoved = false;
	QApplication::restoreOverrideCursor();

	switch (event->button()) {
	case Qt::LeftButton:
		movingFractalDeferred = true;
		fractalCenterXOnPress = fractal.centerX;
		fractalCenterYOnPress = fractal.centerY;
		mousePosOnPress = event->posF();
		QApplication::setOverrideCursor(Qt::ClosedHandCursor);
		break;
	case Qt::MidButton:
		movingFractalRealTime = true;
		QApplication::setOverrideCursor(Qt::ClosedHandCursor);
		break;
	default:
		QLabel::mousePressEvent(event);
	}
	prevMousePos = event->posF();
}

void FractalExplorer::mouseReleaseEvent(QMouseEvent *event)
{
	switch (event->button()) {
	case Qt::LeftButton:
		if (movingFractalDeferred) {
			if (fractalMoved) {
				emit wakeUpSignal();
				emit fractalChanged(fractal);
			}
			movingFractalDeferred = false;
			fractalMoved = false;
			QApplication::restoreOverrideCursor();
		}
		break;
	case Qt::MidButton:
		if (movingFractalRealTime) {
			movingFractalRealTime = false;
			QApplication::restoreOverrideCursor();
		}
		break;
	default:
		QLabel::mouseReleaseEvent(event);
	}
}

void FractalExplorer::mouseMoveEvent(QMouseEvent *event)
{
	if (movingFractalDeferred) {
		if (!fractalMoved) {
			cancelActionIfNotFinished();
			imageCopyOnPress = fractalQImage->copy();
		}
		fractalMoved = true;
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
		
		reInitFractal();
		emit fractalChanged(fractal);
	} else if (movingFractalRealTime) {
		QVector2D vect(event->posF()-prevMousePos);
		double dx = vect.x() * fractal.spanX / fractalImage.width;
		double dy = vect.y() * fractal.spanY / fractalImage.height;
		moveFractal(-dx, -dy, true);
		prevMousePos = event->posF();
	} else {
		QLabel::mouseMoveEvent(event);
	}
}

void FractalExplorer::wheelEvent(QWheelEvent *event)
{
	if (movingFractalDeferred) {
		if (fractalMoved) {
			emit wakeUpSignal();
			emit fractalChanged(fractal);
		}
		movingFractalDeferred = false;
		fractalMoved = false;
		QApplication::restoreOverrideCursor();
	}

	double numDegrees = event->delta() / 8.;
	double numSteps = numDegrees / 15;

	if (event->orientation() == Qt::Vertical) {
		double newSpanX;
		if (fractal.spanX == 0) {
			newSpanX = MIN_SINGLE_STEP;
		} else {
			newSpanX = fractal.spanX * (1 - numSteps*0.3);
		}
		double zoomCenterX, zoomCenterY;
		if (fractal.spanX == 0 || fractal.spanY == 0) {
			zoomCenterX = fractal.centerX;
			zoomCenterY = fractal.centerY;
		} else {
			zoomCenterX = (event->x() + 0.5) * fractal.spanX / fractalImage.width + fractal.x1;
			zoomCenterY = (event->y() + 0.5) * fractal.spanY / fractalImage.height + fractal.y1;
		}
		if (numSteps > 0) {
			zoomInFractal(newSpanX, zoomCenterX, zoomCenterY, true);
		} else {
			zoomOutFractal(newSpanX, zoomCenterX, zoomCenterY, true);
		}
	}
	event->accept();
}

void FractalExplorer::setFractalFormula(int index)
{
	cancelActionIfNotFinished();

	fractal.fractalFormula = (FractalFormula)index;
	reInitFractal();

	emit wakeUpSignal();
}

void FractalExplorer::setPParam(double value)
{
	cancelActionIfNotFinished();

	fractal.p = value;
	reInitFractal();

	emit wakeUpSignal();
}

void FractalExplorer::setCParamRe(double value)
{
	cancelActionIfNotFinished();

	fractal.c = value + I*cimagF(fractal.c);
	reInitFractal();

	emit wakeUpSignal();
}

void FractalExplorer::setCParamIm(double value)
{
	cancelActionIfNotFinished();

	fractal.c = crealF(fractal.c) + I*value;
	reInitFractal();

	emit wakeUpSignal();
}

void FractalExplorer::setCenterX(double value)
{
	double dx = value - fractal.centerX;
	moveFractal(dx, 0);
}

void FractalExplorer::setCenterY(double value)
{
	double dy = value - fractal.centerY;
	moveFractal(0, dy);
}

void FractalExplorer::setSpanX(double value)
{
	if (value > fractal.spanX) {
		/* Zoom out */
		zoomOutFractal(value, fractal.centerX, fractal.centerY);
	} else {
		/* Zoom in */
		zoomInFractal(value, fractal.centerX, fractal.centerY);
	}
}

void FractalExplorer::setBailoutRadius(double value)
{
	cancelActionIfNotFinished();
	
	fractal.escapeRadius = value;
	reInitFractal();

	emit wakeUpSignal();
}

void FractalExplorer::setMaxIterations(int value)
{
	cancelActionIfNotFinished();
	
	fractal.maxIter = value;
	reInitFractal();

	emit wakeUpSignal();
}

void FractalExplorer::setAddendFunction(int index)
{
	cancelActionIfNotFinished();

	render.addendFunction = (AddendFunction)index;
	reInitRenderingParameters();

	emit wakeUpSignal();
}

void FractalExplorer::setStripeDensity(int value)
{
	cancelActionIfNotFinished();

	render.stripeDensity = value;
	reInitRenderingParameters();

	emit wakeUpSignal();
}

void FractalExplorer::setColoringMethod(int index)
{
	cancelActionIfNotFinished();

	render.coloringMethod = (ColoringMethod)index;
	reInitRenderingParameters();

	emit wakeUpSignal();
}

void FractalExplorer::setCountingFunction(int index)
{
	cancelActionIfNotFinished();

	render.countingFunction = (CountingFunction)index;
	reInitRenderingParameters();

	emit wakeUpSignal();
}

void FractalExplorer::setInterpolationMethod(int index)
{
	cancelActionIfNotFinished();

	render.interpolationMethod = (InterpolationMethod)index;
	reInitRenderingParameters();

	emit wakeUpSignal();
}

void FractalExplorer::setTransferFunction(int index)
{
	cancelActionIfNotFinished();

	render.transferFunction = (TransferFunction)index;
	reInitRenderingParameters();

	emit wakeUpSignal();
}

void FractalExplorer::setColorScaling(double value)
{
	cancelActionIfNotFinished();

	render.multiplier = value;
	reInitRenderingParameters();

	emit wakeUpSignal();
}

void FractalExplorer::setColorOffset(double value)
{
	cancelActionIfNotFinished();

	render.offset = value;
	reInitRenderingParameters();

	emit wakeUpSignal();
}

void FractalExplorer::setSpaceColor(QColor color)
{
	cancelActionIfNotFinished();

	render.spaceColor = ColorFromRGB(1, (uint16_t)color.red(), (uint16_t)color.green(), (uint16_t)color.blue());
	reInitRenderingParameters();

	emit wakeUpSignal();
}

