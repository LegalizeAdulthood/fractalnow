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
#include <QPainter>
#include <QStyle>
#include <QtConcurrentRun>
#include <QTime>
#include <QTimer>
#include <QVector2D>

FractalExplorer::FractalExplorer(const Fractal &fractal, const RenderingParameters &render,
					uint_fast32_t width, uint_fast32_t height,
					uint_fast32_t minAntiAliasingSize,
					uint_fast32_t maxAntiAliasingSize,
					uint_fast32_t antiAliasingSizeIteration,
					uint_fast32_t nbThreads,
					QWidget *parent, Qt::WindowFlags f) : QLabel(parent, f)
{
	setCursor(QCursor(Qt::OpenHandCursor));
	setContextMenuPolicy(Qt::DefaultContextMenu);

	this->fractal = CopyFractal(&fractal);
	this->initialFractal = CopyFractal(&fractal);
	this->render = CopyRenderingParameters(&render);
	this->initialRender = CopyRenderingParameters(&render);
	this->initialWidth = width;
	this->initialHeight = height;
	this->minAntiAliasingSize = minAntiAliasingSize;
	this->maxAntiAliasingSize = maxAntiAliasingSize;
	this->antiAliasingSizeIteration = antiAliasingSizeIteration;

	movingFractalDeferred = false;
	movingFractalRealTime = false;
	fractalMoved = false;

	/* Create QImage and Image (from fractal lib) */
	fractalQImage = new QImage(width, height, QImage::Format_RGB32);
	CreateImage2(&fractalImage, fractalQImage->bits(), width, height, 1);
	adjustSpan();

	restoreInitialStateAction = new QAction(tr("Restore &initial state"), this);
	restoreInitialStateAction->setShortcut(QKeySequence::MoveToStartOfDocument);
	restoreInitialStateAction->setIcon(QApplication::style()->standardIcon(
						QStyle::SP_DirHomeIcon));
	connect(restoreInitialStateAction, SIGNAL(triggered()), this,
		SLOT(restoreInitialState()));

	stopDrawingAction = new QAction(tr("&Stop drawing"), this);
	stopDrawingAction->setShortcut(Qt::Key_S);
	stopDrawingAction->setIcon(QApplication::style()->standardIcon(QStyle::SP_BrowserStop));
	connect(stopDrawingAction, SIGNAL(triggered()), this, SLOT(stopDrawing()));

	refreshAction = new QAction(tr("&Refresh"), this);
	refreshAction->setShortcut(QKeySequence::Refresh);
	refreshAction->setIcon(QApplication::style()->standardIcon(QStyle::SP_BrowserReload));
	connect(refreshAction, SIGNAL(triggered()), this, SLOT(refresh()));

	zoomInAction = new QAction(tr("Zoom in"), this);
	QList<QKeySequence> zoomInShortcuts;
	zoomInShortcuts << Qt::Key_Plus << QKeySequence::ZoomIn;
	zoomInAction->setShortcuts(zoomInShortcuts);
	connect(zoomInAction, SIGNAL(triggered()), this, SLOT(zoomInFractal()));

	zoomOutAction = new QAction(tr("Zoom out"), this);
	QList<QKeySequence> zoomOutShortcuts;
	zoomOutShortcuts << Qt::Key_Minus << QKeySequence::ZoomOut;
	zoomOutAction->setShortcuts(zoomOutShortcuts);
	connect(zoomOutAction, SIGNAL(triggered()), this, SLOT(zoomOutFractal()));

	moveLeftAction = new QAction(tr("Move left"), this);
	moveLeftAction->setShortcut(Qt::Key_Left);
	connect(moveLeftAction, SIGNAL(triggered()), this, SLOT(moveLeftFractal()));

	moveRightAction = new QAction(tr("Move right"), this);
	moveRightAction->setShortcut(Qt::Key_Right);
	connect(moveRightAction, SIGNAL(triggered()), this, SLOT(moveRightFractal()));

	moveUpAction = new QAction(tr("Move up"), this);
	moveUpAction->setShortcut(Qt::Key_Up);
	connect(moveUpAction, SIGNAL(triggered()), this, SLOT(moveUpFractal()));

	moveDownAction = new QAction(tr("Move down"), this);
	moveDownAction->setShortcut(Qt::Key_Down);
	connect(moveDownAction, SIGNAL(triggered()), this, SLOT(moveDownFractal()));

	threads = CreateThreads(nbThreads);
	action = DoNothingAction();

	/* Create timer to repaint fractalImageLabel regularly. */
	timer = new QTimer(this);
	connect(timer, SIGNAL(timeout()), this, SLOT(update()));
	connect(timer, SIGNAL(timeout()), this, SLOT(onTimeout()));
	timer->start(50);
}

FractalExplorer::~FractalExplorer()
{
	cancelActionIfNotFinished();
	FreeAction(action);
	DestroyThreads(threads);
	FreeRenderingParameters(render);
	FreeRenderingParameters(initialRender);
	FreeImage(fractalImage);
	delete fractalQImage;
}

void FractalExplorer::adjustSpan()
{
	uint_fast32_t width = fractalImage.width;
	uint_fast32_t height = fractalImage.height;

	if (width != 0 && height != 0) {
		if (height / (double)width < 1) {
			fractal.spanX = width * fractal.spanY / height;
		} else {
			fractal.spanY = height * fractal.spanX / width;
		}
		reInitFractal();
	}
}

void FractalExplorer::restoreInitialState()
{
	cancelActionIfNotFinished();
	fractalQImage->fill(0);

	RenderingParameters oldRender = render;
	fractal = CopyFractal(&initialFractal);

	render = CopyRenderingParameters(&initialRender);
	FreeRenderingParameters(oldRender);
	adjustSpan();

	emit fractalChanged(fractal);
	emit renderingParametersChanged(render);
	refresh();
}

Fractal &FractalExplorer::getFractal()
{
	return fractal;
}

RenderingParameters &FractalExplorer::getRender()
{
	return render;
}

void FractalExplorer::resizeImage(uint_fast32_t width, uint_fast32_t height)
{
	if (height == 0) {
		if (fractal.spanX == 0) {
			height = width;
		} else {
			height = width * fractal.spanY / fractal.spanX;
		}
	}

	double dx = width - (double)fractalImage.width;
	double dy = height - (double)fractalImage.height;
	if (fractalImage.width != 0) {
		fractal.spanX +=  dx * fractal.spanX / fractalImage.width;
	}
	if (fractalImage.height != 0) {
		fractal.spanY +=  dy * fractal.spanY / fractalImage.height;
	}
	reInitFractal();

	cancelActionIfNotFinished();
	Image newImage, oldImage;
	QImage *newQImage, *oldQImage;
	newQImage = new QImage(width, height, QImage::Format_RGB32);

	newQImage->fill(0);
	QPainter painter(newQImage);
	painter.setRenderHint(QPainter::SmoothPixmapTransform);

	QRectF srcRect(std::max(0.,-dx/2), std::max(0.,-dy/2), fractalImage.width+std::min(0.,dx),
			fractalImage.height+std::min(0.,dy));
	QRectF dstRect(std::max(0.,dx/2), std::max(0.,dy/2), fractalImage.width-std::max(0.,-dx),
			fractalImage.height-std::max(0.,-dy));
	painter.drawImage(dstRect, *fractalQImage, srcRect);

	CreateImage2(&newImage, newQImage->bits(), width, height, 1);
	oldImage = fractalImage;
	oldQImage = fractalQImage;
	fractalImage = newImage;
	fractalQImage = newQImage;
	FreeImage(oldImage);
	delete oldQImage;

	updateGeometry();

	emit fractalChanged(fractal);
	refresh();
}

void FractalExplorer::resizeEvent(QResizeEvent * event)
{
	QSize size(event->size());
	resizeImage(size.width(), size.height());
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
	FreeAction(action);
	redrawFractal = false;
	lastActionType = A_FractalDrawing;
	action = LaunchDrawFractalFast(&fractalImage, &fractal, &render,
				DEFAULT_QUAD_INTERPOLATION_SIZE,
				DEFAULT_COLOR_DISSIMILARITY_THRESHOLD, threads);
}

/* Assumes that action is finished.*/
void FractalExplorer::launchFractalAntiAliasing()
{
	FreeAction(action);
	lastActionType = A_FractalAntiAliasing;
	action = LaunchAntiAliaseFractal(&fractalImage, &fractal, &render,
			currentAntiAliasingSize, DEFAULT_ADAPTIVE_AAM_THRESHOLD, threads);
}

int FractalExplorer::cancelActionIfNotFinished()
{
	int finished = ActionIsFinished(action);
	if (!finished) {
		CancelAction(action); 
		int unused = GetActionResult(action);
		(void)unused;
	}
	return finished;
}

void FractalExplorer::onTimeout()
{
	if (redrawFractal) {
		cancelActionIfNotFinished();
		launchFractalDrawing();
	} else {
		if (ActionIsFinished(action) && GetActionResult(action) == 0) {
			if (lastActionType == A_FractalDrawing) {
				currentAntiAliasingSize = minAntiAliasingSize;
				launchFractalAntiAliasing();
			} else if (currentAntiAliasingSize < maxAntiAliasingSize) {
				currentAntiAliasingSize += antiAliasingSizeIteration;
				if (currentAntiAliasingSize > maxAntiAliasingSize) {
					currentAntiAliasingSize = maxAntiAliasingSize;
				}
				launchFractalAntiAliasing();
			}
		}
	}
}

int FractalExplorer::stopDrawing()
{
	return cancelActionIfNotFinished();
}

void FractalExplorer::refresh()
{
	redrawFractal = true;
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
		QRectF srcRect = QRectF(std::max((double)0, ddx), std::max((double)0, ddy),
				fractalImage.width-fabs(ddx), fractalImage.height-fabs(ddy));
		QRectF dstRect = QRectF(std::max((double)0, -ddx), std::max((double)0, -ddy),
				fractalImage.width-fabs(ddx), fractalImage.height-fabs(ddy));
		painter.drawImage(dstRect, copiedImage, srcRect);
	}

	fractal.centerX += dx;
	fractal.centerY += dy;
	reInitFractal();
	if (emitFractalChanged) {
		emit fractalChanged(fractal);
	}
	refresh();
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

void FractalExplorer::zoomInFractal(double newSpanX, double zoomCenterX, double zoomCenterY,
					bool emitFractalChanged)
{
	cancelActionIfNotFinished();

	if (newSpanX == 0) {
		fractalQImage->fill(0);
		fractal.spanX = 0;
		fractal.spanY = 0;
	} else {
		double newSpanY = newSpanX * fractalImage.height / fractalImage.width;
		double newCenterX = zoomCenterX + newSpanX * (fractal.centerX - zoomCenterX)
					/ fractal.spanX;
		double newCenterY = zoomCenterY + newSpanY * (fractal.centerY - zoomCenterY)
					/ fractal.spanY;

		QImage copiedImage(fractalQImage->copy());
		fractalQImage->fill(0);
		QPainter painter(fractalQImage);
		painter.setRenderHint(QPainter::SmoothPixmapTransform);
		QRect dstRect(fractalQImage->rect());
		QRectF srcRect(0, 0, dstRect.width() * newSpanX / fractal.spanX,
					dstRect.height() * newSpanX / fractal.spanX); 
		srcRect.moveCenter(QPointF((fractalQImage->width())*(newCenterX-fractal.x1)/fractal.spanX,
					(fractalQImage->height())*(newCenterY-fractal.y1)/fractal.spanY));
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
	refresh();
}

void FractalExplorer::zoomOutFractal(double newSpanX, double zoomCenterX, double zoomCenterY,
					bool emitFractalChanged)
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
			newCenterX = zoomCenterX + newSpanX * (fractal.centerX - zoomCenterX)
					/ fractal.spanX;
			newCenterY = zoomCenterY + newSpanY * (fractal.centerY - zoomCenterY)
					/ fractal.spanY;
		}

		QImage copiedImage(fractalQImage->copy());
		fractalQImage->fill(0);
		QPainter painter(fractalQImage);
		painter.setRenderHint(QPainter::SmoothPixmapTransform);
		QRect srcRect(fractalQImage->rect());
		QRectF dstRect(0, 0, srcRect.width() * fractal.spanX / newSpanX, srcRect.height()
					* fractal.spanX / newSpanX); 
		dstRect.moveCenter(QPointF(srcRect.width()*(fractal.centerX-(newCenterX-newSpanX/2))
					/ newSpanX, srcRect.height()
					*(fractal.centerY-(newCenterY-newSpanY/2)) / newSpanY));

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
	refresh();
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
		emit fractalChanged(fractal);
		refresh();
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
				emit fractalChanged(fractal);
				refresh();
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
		QRectF srcRect = QRectF(std::max((double)0, -ddx), std::max((double)0, -ddy),
				fractalImage.width - fabs(ddx), fractalImage.height - fabs(ddy));
		QRectF dstRect = QRectF(std::max((double)0, ddx), std::max((double)0, ddy),
				fractalImage.width - fabs(ddx), fractalImage.height - fabs(ddy));
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

static inline double signl(double x)
{
	return (x > 0) ? 1 : -1;
}

void FractalExplorer::wheelEvent(QWheelEvent *event)
{
	if (movingFractalDeferred) {
		if (fractalMoved) {
			emit fractalChanged(fractal);
			refresh();
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
			newSpanX = fractal.spanX * powl(1 - signl(numSteps)*0.3, fabsl(numSteps));
		}
		double zoomCenterX, zoomCenterY;
		if (fractal.spanX == 0 || fractal.spanY == 0) {
			zoomCenterX = fractal.centerX;
			zoomCenterY = fractal.centerY;
		} else {
			zoomCenterX = (event->x() + 0.5) * fractal.spanX
					/ fractalImage.width + fractal.x1;
			zoomCenterY = (event->y() + 0.5) * fractal.spanY
					/ fractalImage.height + fractal.y1;
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

	refresh();
}

void FractalExplorer::setPParam(double value)
{
	cancelActionIfNotFinished();

	fractal.p = value;
	reInitFractal();

	refresh();
}

void FractalExplorer::setCParamRe(double value)
{
	cancelActionIfNotFinished();

	fractal.c = value + I*cimagF(fractal.c);
	reInitFractal();

	refresh();
}

void FractalExplorer::setCParamIm(double value)
{
	cancelActionIfNotFinished();

	fractal.c = crealF(fractal.c) + I*value;
	reInitFractal();

	refresh();
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

	refresh();
}

void FractalExplorer::setMaxIterations(int value)
{
	cancelActionIfNotFinished();
	
	fractal.maxIter = value;
	reInitFractal();

	refresh();
}

void FractalExplorer::setAddendFunction(int index)
{
	cancelActionIfNotFinished();

	render.addendFunction = (AddendFunction)index;
	reInitRenderingParameters();

	refresh();
}

void FractalExplorer::setStripeDensity(int value)
{
	cancelActionIfNotFinished();

	render.stripeDensity = value;
	reInitRenderingParameters();

	refresh();
}

void FractalExplorer::setColoringMethod(int index)
{
	cancelActionIfNotFinished();

	render.coloringMethod = (ColoringMethod)index;
	reInitRenderingParameters();

	refresh();
}

void FractalExplorer::setCountingFunction(int index)
{
	cancelActionIfNotFinished();

	render.countingFunction = (CountingFunction)index;
	reInitRenderingParameters();

	refresh();
}

void FractalExplorer::setInterpolationMethod(int index)
{
	cancelActionIfNotFinished();

	render.interpolationMethod = (InterpolationMethod)index;
	reInitRenderingParameters();

	refresh();
}

void FractalExplorer::setTransferFunction(int index)
{
	cancelActionIfNotFinished();

	render.transferFunction = (TransferFunction)index;
	reInitRenderingParameters();

	refresh();
}

void FractalExplorer::setColorScaling(double value)
{
	cancelActionIfNotFinished();

	render.multiplier = value;
	reInitRenderingParameters();

	refresh();
}

void FractalExplorer::setColorOffset(double value)
{
	cancelActionIfNotFinished();

	render.offset = value;
	reInitRenderingParameters();

	refresh();
}

void FractalExplorer::setSpaceColor(QColor color)
{
	cancelActionIfNotFinished();

	render.spaceColor = ColorFromRGB(1, (uint16_t)color.red(),
				(uint16_t)color.green(), (uint16_t)color.blue());
	reInitRenderingParameters();

	refresh();
}

void FractalExplorer::setGradient(const Gradient &gradient)
{
	cancelActionIfNotFinished();

	Gradient newGradient = Gradient8(&gradient);
	Gradient oldGradient = render.gradient;
	render.gradient = newGradient;
	FreeGradient(oldGradient);
	reInitRenderingParameters();

	refresh();
}

inline static bool pos_less_than(const QGradientStop &s1, const QGradientStop &s2)
{
    return s1.first < s2.first;
}

void FractalExplorer::setGradient(const QGradientStops &gradientStops)
{
	if (gradientStops.size() < 2) {
		/* Emit signal to be sure that fractal explorer's gradient
		 * matches rendering widget's gradient box.
		 */
		emit renderingParametersChanged(render);
		return;
	}
	cancelActionIfNotFinished();

	/* First, sort gradient stops.*/
	QGradientStops sortedGradientStops(gradientStops);
	qSort(sortedGradientStops.begin(), sortedGradientStops.end(), pos_less_than);

	/* Now create well formed gradient stops, i.e. first gradient
	 * stop position must be 0, last must be 1, and positions
	 * must be strictly increasing ("duplicates" are skipped).
	 */
	bool wellFormed = 1;
	QGradientStops wellFormedGradientStops;
	/* First position must be 0.*/
	if (sortedGradientStops[0].first != 0) {
		wellFormed = 0;
	}
	wellFormedGradientStops << QPair<qreal,QColor>(0, sortedGradientStops[0].second);
	for (int i = 1; i < sortedGradientStops.size(); ++i) {
		/* Positions should be strictly increasing.*/
		if (sortedGradientStops[i].first <= wellFormedGradientStops[i-1].first) {
			wellFormed = 0;
			continue;
		}
		wellFormedGradientStops << sortedGradientStops[i];
	}
	/* Last position must be 1.*/
	if (wellFormedGradientStops.last().first != 1) {
		wellFormedGradientStops.last() = QPair<qreal,QColor>(1,
						wellFormedGradientStops.last().second);
		wellFormed = 0;
	}

	/* Now gradient fractal2D gradient from well formed gradient.*/
	FLOAT positionStop[wellFormedGradientStops.size()];
	Color colorStop[wellFormedGradientStops.size()];
	QGradientStop currentStop;
	for (int i = 0; i < wellFormedGradientStops.size(); ++i) {
		currentStop = wellFormedGradientStops[i];
		positionStop[i] = currentStop.first;
		colorStop[i] = ColorFromUint32(currentStop.second.rgb());
	}

	/* Replace old gradient.*/
	Gradient oldGradient = render.gradient;
	GenerateGradient(&render.gradient, wellFormedGradientStops.size(), positionStop,
				colorStop, DEFAULT_GRADIENT_SIZE);
	FreeGradient(oldGradient);
	reInitRenderingParameters();

	refresh();
	if (!wellFormed) {
		emit renderingParametersChanged(render);
	}
}

void FractalExplorer::contextMenuEvent(QContextMenuEvent *event)
{
	QMenu menu(this);
	menu.addAction(restoreInitialStateAction);
	menu.addSeparator();
	menu.addAction(stopDrawingAction);
	menu.addAction(refreshAction);
	menu.addSeparator();
	menu.addAction(zoomInAction);
	menu.addAction(zoomOutAction);
	menu.addSeparator();
	menu.addAction(moveLeftAction);
	menu.addAction(moveRightAction);
	menu.addAction(moveUpAction);
	menu.addAction(moveDownAction);
	QAction *action = menu.exec(event->globalPos());
	(void)action;
}

