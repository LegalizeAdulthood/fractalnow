/*
 *  fractal_explorer.cpp -- part of FractalNow
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
#include <QMessageBox>
#include <QPainter>
#include <QStyle>
#include <QtConcurrentRun>
#include <QTime>
#include <QTimer>
#include <QVector2D>

FractalExplorer::FractalExplorer(const FractalConfig &fractalConfig,
					uint_fast32_t width, uint_fast32_t height,
					uint_fast32_t minAntiAliasingSize,
					uint_fast32_t maxAntiAliasingSize,
					uint_fast32_t antiAliasingSizeIteration,
					uint_fast32_t quadInterpolationSize,
					double colorDissimilarityThreshold,
					double adaptiveAAMThreshold,
					uint_fast32_t nbThreads,
					QWidget *parent, Qt::WindowFlags f) :
	QLabel(parent, f), fractal(this->fractalConfig.fractal),
	render(this->fractalConfig.render),
	quadInterpolationSize(quadInterpolationSize),
	colorDissimilarityThreshold(colorDissimilarityThreshold),
	adaptiveAAMThreshold(adaptiveAAMThreshold)
{
	setCursor(QCursor(Qt::OpenHandCursor));
	setContextMenuPolicy(Qt::DefaultContextMenu);

	mpfr_init(fractalCenterXOnPress);
	mpfr_init(fractalCenterYOnPress);

	this->fractalConfig = CopyFractalConfig(&fractalConfig);
	if (fractalConfig.render.bytesPerComponent == 2) {
		QMessageBox::critical(this, tr("Gradient color depth is 16 bits."),
			tr("16-bits gradient will be converted to 8-bits gradient."));
		Gradient newGradient = Gradient8(&this->render.gradient);
		ResetGradient(&this->render, newGradient);
		FreeGradient(newGradient);
	}
	this->initialFractalConfig = CopyFractalConfig(&this->fractalConfig);
	this->initialWidth = width;
	this->initialHeight = height;
	this->minAntiAliasingSize = minAntiAliasingSize;
	this->maxAntiAliasingSize = maxAntiAliasingSize;
	this->antiAliasingSizeIteration = antiAliasingSizeIteration;

	drawingPaused = false;
	redrawFractal = false;
	movingFractalDeferred = false;
	movingFractalRealTime = false;
	fractalMoved = false;

	/* Create QImage and Image (from fractal lib) */
	fractalQImage = new QImage(width, height, QImage::Format_RGB32);
	fractalQImage->fill(0);
	CreateImage2(&fractalImage, fractalQImage->bits(), width, height, 1);
	adjustSpan();

	restoreInitialStateAction = new QAction(tr("Restore &initial state"), this);
	restoreInitialStateAction->setIconText(tr("Initial state"));
	restoreInitialStateAction->setShortcut(QKeySequence::MoveToStartOfDocument);
	restoreInitialStateAction->setIcon(QApplication::style()->standardIcon(
						QStyle::SP_DirHomeIcon));
	connect(restoreInitialStateAction, SIGNAL(triggered()), this,
		SLOT(restoreInitialState()));

	stopDrawingAction = new QAction(tr("&Stop drawing"), this);
	stopDrawingAction->setIconText(tr("Stop"));
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

	int failure = CreateFractalCache(&cache, 0);
	if (failure) {
		emit fractalCacheSizeChanged(cache.size);
		QMessageBox::critical(this, tr("Failed to resize cache"),
			tr("Could not allocate memory for cache."));
	}
	pCache = NULL;

	solidGuessing = true;
	floatPrecision = FP_DOUBLE;

	threads = CreateThreads(nbThreads);
	task = DoNothingTask();
	LaunchTask(task, threads);

	/* Create timer to repaint fractalImageLabel regularly. */
	timer = new QTimer(this);
	connect(timer, SIGNAL(timeout()), this, SLOT(onTimeout()));
	timer->start(10);
}

FractalExplorer::~FractalExplorer()
{
	cancelActionIfNotFinished();
	FreeTask(task);
	DestroyThreads(threads);
	FreeFractalConfig(fractalConfig);
	FreeFractalConfig(initialFractalConfig);
	FreeImage(fractalImage);
	FreeFractalCache(&cache);
	mpfr_clear(fractalCenterXOnPress);
	mpfr_clear(fractalCenterYOnPress);
	delete fractalQImage;
}

bool FractalExplorer::getFractalCacheEnabled() const
{
	return (pCache != NULL);
}

void FractalExplorer::useFractalCache(bool enabled)
{
	bool changed = (enabled == (pCache == NULL));
	if (changed) {
		cancelActionIfNotFinished();

		if (enabled) {
			pCache = &cache;
		} else {
			pCache = NULL;
		}
	
		refresh();
	}
}

int FractalExplorer::getFractalCacheSize() const
{
	return (int)cache.size;
}

void FractalExplorer::resizeFractalCache(int size)
{
	if (size < 0) {
		size = 0;
	}
	int failure = ResizeCacheThreadSafe(&cache, size);
	if (failure) {
		emit fractalCacheSizeChanged(cache.size);
		QMessageBox::critical(this, tr("Failed to resize cache"),
			tr("Could not allocate memory for cache."));
	}
}

bool FractalExplorer::getSolidGuessingEnabled() const
{
	return solidGuessing;
}

void FractalExplorer::setSolidGuessingEnabled(bool enabled)
{
	bool changed = (enabled != solidGuessing);
	if (changed) {
		cancelActionIfNotFinished();

		solidGuessing = enabled;

		refresh();
	}
}

void FractalExplorer::setFractalConfig(const FractalConfig &fractalConfig)
{
	cancelActionIfNotFinished();

	FractalConfig oldFractalConfig = this->fractalConfig;
	this->fractalConfig = CopyFractalConfig(&fractalConfig);
	if (this->fractalConfig.render.bytesPerComponent == 2) {
		QMessageBox::critical(this, tr("Gradient color depth is 16 bits."),
			tr("16-bits gradient will be converted to 8-bits gradient."));
		Gradient newGradient = Gradient8(&this->fractalConfig.render.gradient);
		ResetGradient(&this->fractalConfig.render, newGradient);
		FreeGradient(newGradient);
	}
	FreeFractalConfig(oldFractalConfig);
	adjustSpan();

	emit fractalChanged(fractal);
	emit renderingParametersChanged(render);
	refresh();
}

void FractalExplorer::setFractal(const Fractal &fractal)
{
	cancelActionIfNotFinished();

	ResetFractal(&fractalConfig, fractal);
	adjustSpan();

	emit fractalChanged(this->fractal);
	refresh();
}

void FractalExplorer::setRenderingParameters(const RenderingParameters &render)
{
	cancelActionIfNotFinished();

	RenderingParameters newRender = CopyRenderingParameters(&render);
	if (newRender.bytesPerComponent == 2) {
		QMessageBox::critical(this, tr("Gradient color depth is 16 bits."),
			tr("16-bits gradient will be converted to 8-bits gradient."));
		Gradient newGradient = Gradient8(&newRender.gradient);
		ResetGradient(&newRender, newGradient);
		FreeGradient(newGradient);
	}
	ResetRenderingParameters(&fractalConfig, newRender);
	FreeRenderingParameters(newRender);
	adjustSpan();

	emit renderingParametersChanged(this->render);
	refresh();
}

void FractalExplorer::setGradient(const Gradient &gradient)
{
	cancelActionIfNotFinished();

	if (gradient.bytesPerComponent == 2) {
		QMessageBox::critical(this, tr("Gradient color depth is 16 bits."),
			tr("16-bits gradient will be converted to 8-bits gradient."));
	}
	ResetGradient(&fractalConfig.render, gradient);
	adjustSpan();

	emit renderingParametersChanged(render);
	refresh();
}

void FractalExplorer::adjustSpan()
{
	uint_fast32_t width = fractalImage.width;
	uint_fast32_t height = fractalImage.height;

	if (width != 0 && height != 0) {
		if (height / (double)width < 1) {
			mpfr_set(fractal.spanX, fractal.spanY, MPFR_RNDN);
			mpfr_mul_ui(fractal.spanX, fractal.spanX, width, MPFR_RNDN);
			mpfr_div_ui(fractal.spanX, fractal.spanX, height, MPFR_RNDN);
		} else {
			mpfr_set(fractal.spanY, fractal.spanX, MPFR_RNDN);
			mpfr_mul_ui(fractal.spanY, fractal.spanY, height, MPFR_RNDN);
			mpfr_div_ui(fractal.spanY, fractal.spanY, width, MPFR_RNDN);
		}
		reInitFractal();
	}
}

void FractalExplorer::restoreInitialState()
{
	setFractalConfig(initialFractalConfig);
}

const FractalConfig &FractalExplorer::getFractalConfig() const
{
	return fractalConfig;
}

const Fractal &FractalExplorer::getFractal() const
{
	return fractal;
}

const RenderingParameters &FractalExplorer::getRender() const
{
	return render;
}

void FractalExplorer::resizeImage(uint_fast32_t width, uint_fast32_t height)
{
	if (height == 0) {
		if (mpfr_cmp_ui(fractal.spanX, 0) == 0) {
			height = width;
		} else {
			mpfr_t tmp;
			mpfr_div(tmp, fractal.spanY, fractal.spanX, MPFR_RNDN);
			mpfr_mul_ui(tmp, tmp, width, MPFR_RNDN);
			height = mpfr_get_ld(tmp, MPFR_RNDN);
		}
	}

	double dx = width - (double)fractalImage.width;
	double dy = height - (double)fractalImage.height;
	if (fractalImage.width != 0) {
		mpfr_mul_d(fractal.spanX, fractal.spanX, 1. + dx/fractalImage.width, MPFR_RNDN);
	}
	if (fractalImage.height != 0) {
		mpfr_mul_d(fractal.spanY, fractal.spanY, 1. + dy/fractalImage.height, MPFR_RNDN);
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
	UNUSED(event);
	setFocusPolicy(Qt::StrongFocus);

	if (fractalQImage != NULL) {
		QPainter painter(this);
		painter.setRenderHint(QPainter::SmoothPixmapTransform);
		if (!drawingPaused) {
			PauseTask(task);
		}
		painter.drawImage(fractalQImage->rect(), *fractalQImage, fractalQImage->rect());
		if (!drawingPaused) {
			ResumeTask(task);
		}
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
	Fractal newFractal;
	InitFractal(&newFractal, fractal.fractalFormula, fractal.p, fractal.c,
			fractal.centerX, fractal.centerY,
			fractal.spanX, fractal.spanY, 
			fractal.escapeRadius, fractal.maxIter);
	Fractal oldFractal = fractal;
	fractal = newFractal;
	FreeFractal(oldFractal);
}

void FractalExplorer::reInitRenderingParameters()
{
	InitRenderingParameters(&render, render.bytesPerComponent, render.spaceColor,
				render.iterationCount, render.coloringMethod,
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
	FreeTask(task);
	redrawFractal = false;
	lastActionType = A_FractalDrawing;
	task = CreateDrawFractalTask(&fractalImage, &fractal, &render,
				solidGuessing ? quadInterpolationSize : 1,
				colorDissimilarityThreshold, floatPrecision,
				pCache, threads->N);
	LaunchTask(task, threads);
	if (drawingPaused) {
		PauseTask(task);
	}
}

/* Assumes that action is finished.*/
void FractalExplorer::launchFractalAntiAliasing()
{
	FreeTask(task);
	lastActionType = A_FractalAntiAliasing;
	task = CreateAntiAliaseFractalTask(&fractalImage, &fractal, &render,
			currentAntiAliasingSize, adaptiveAAMThreshold,
			floatPrecision, NULL, threads->N);
	LaunchTask(task, threads);
	if (drawingPaused) {
		PauseTask(task);
	}
}

int FractalExplorer::cancelActionIfNotFinished()
{
	int finished = TaskIsFinished(task);
	if (!finished) {
		CancelTask(task); 
		if (drawingPaused) {
			ResumeTask(task);
		}
		int unused = GetTaskResult(task);
		UNUSED(unused);
	}
	return finished;
}

void FractalExplorer::onTimeout()
{
	bool updateNeeded = true;
	if (redrawFractal) {
		cancelActionIfNotFinished();
		launchFractalDrawing();
	} else {
		if (TaskIsFinished(task) && GetTaskResult(task) == 0) {
			if (lastActionType == A_FractalDrawing) {
				currentAntiAliasingSize = minAntiAliasingSize;
				launchFractalAntiAliasing();
			} else if (currentAntiAliasingSize < maxAntiAliasingSize) {
				currentAntiAliasingSize += antiAliasingSizeIteration;
				if (currentAntiAliasingSize > maxAntiAliasingSize) {
					currentAntiAliasingSize = maxAntiAliasingSize;
				}
				launchFractalAntiAliasing();
			} else {
				updateNeeded = false;
			}
		}
	}
	if (updateNeeded) {
		update();
	}
}

int FractalExplorer::stopDrawing()
{
	return cancelActionIfNotFinished();
}

void FractalExplorer::pauseDrawing()
{
	drawingPaused = true;
	PauseTask(task);
}

void FractalExplorer::resumeDrawing()
{
	drawingPaused = false;
	ResumeTask(task);
}

void FractalExplorer::refresh()
{
	redrawFractal = true;
	update();
}

void FractalExplorer::moveFractal(const mpfr_t dx, const mpfr_t dy, bool emitFractalChanged)
{
	cancelActionIfNotFinished();
	mpfr_t absdx, absdy;
	mpfr_init(absdx);
	mpfr_init(absdy);
	mpfr_abs(absdx, dx, MPFR_RNDN);
	mpfr_abs(absdy, dy, MPFR_RNDN);

	if (mpfr_cmp_ui(fractal.spanX, 0) == 0 || mpfr_cmp_ui(fractal.spanY, 0) == 0
		|| mpfr_cmp(absdx, fractal.spanX) >= 0 || mpfr_cmp(absdy, fractal.spanY) >= 0) {
		fractalQImage->fill(0);
	} else {
		QImage copiedImage(fractalQImage->copy());
		fractalQImage->fill(0);
		QPainter painter(fractalQImage);
		painter.setRenderHint(QPainter::SmoothPixmapTransform);
		mpfr_t imgdx, imgdy;
		mpfr_init(imgdx);
		mpfr_init(imgdy);

		mpfr_div(imgdx, dx, fractal.spanX, MPFR_RNDN);
		mpfr_mul_ui(imgdx, imgdx, fractalQImage->width(), MPFR_RNDN);

		mpfr_div(imgdy, dy, fractal.spanY, MPFR_RNDN);
		mpfr_mul_ui(imgdy, imgdy, fractalQImage->height(), MPFR_RNDN);
		
		long double ddx = mpfr_get_ld(imgdx, MPFR_RNDN);
		long double ddy = mpfr_get_ld(imgdy, MPFR_RNDN);

		QRectF srcRect = QRectF(std::max((long double)0, ddx), std::max((long double)0, ddy),
				fractalImage.width-fabsl(ddx), fractalImage.height-fabsl(ddy));
		QRectF dstRect = QRectF(std::max((long double)0, -ddx), std::max((long double)0, -ddy),
				fractalImage.width-fabsl(ddx), fractalImage.height-fabsl(ddy));
		painter.drawImage(dstRect, copiedImage, srcRect);

		mpfr_clear(imgdx);
		mpfr_clear(imgdy);
	}
	mpfr_clear(absdx);
	mpfr_clear(absdy);

	mpfr_add(fractal.centerX, fractal.centerX, dx, MPFR_RNDN);
	mpfr_add(fractal.centerY, fractal.centerY, dy, MPFR_RNDN);

	//fractal.centerX += dx;
	//fractal.centerY += dy;
	reInitFractal();
	if (emitFractalChanged) {
		emit fractalChanged(fractal);
	}
	refresh();
}

void FractalExplorer::moveLeftFractal()
{
	mpfr_t dx, tmp;
	mpfr_init(dx);
	mpfr_init(tmp);
	if (mpfr_cmp_ui(fractal.spanX, 0) == 0) {
		mpfr_set_ld(dx, -MIN_SINGLE_STEP, MPFR_RNDN);
	} else {
		/* We use modf to have a multiple of image width,
		 * so that the temporary qimage matches the
		 * fractal image that will be computed exactly.
		 */
		mpfr_set_ld(tmp, -0.2 * fractalImage.width, MPFR_RNDN);
		mpfr_modf(dx, tmp, tmp, MPFR_RNDN);
		mpfr_mul(dx, dx, fractal.spanX, MPFR_RNDN);
		mpfr_div_ui(dx, dx, fractalImage.width, MPFR_RNDN);
	}
	mpfr_set_ui(tmp, 0, MPFR_RNDN);
	moveFractal(dx, tmp, true);

	mpfr_clear(dx);
	mpfr_clear(tmp);
}

void FractalExplorer::moveRightFractal()
{
	mpfr_t dx, tmp;
	mpfr_init(dx);
	mpfr_init(tmp);
	if (mpfr_cmp_ui(fractal.spanX, 0) == 0) {
		mpfr_set_ld(dx, MIN_SINGLE_STEP, MPFR_RNDN);
	} else {
		mpfr_set_ld(tmp, 0.2 * fractalImage.width, MPFR_RNDN);
		mpfr_modf(dx, tmp, tmp, MPFR_RNDN);
		mpfr_mul(dx, dx, fractal.spanX, MPFR_RNDN);
		mpfr_div_ui(dx, dx, fractalImage.width, MPFR_RNDN);
	}
	mpfr_set_ui(tmp, 0, MPFR_RNDN);
	moveFractal(dx, tmp, true);

	mpfr_clear(dx);
	mpfr_clear(tmp);
}

void FractalExplorer::moveUpFractal()
{
	mpfr_t dy, tmp;
	mpfr_init(dy);
	mpfr_init(tmp);
	if (mpfr_cmp_ui(fractal.spanY,0) == 0) {
		mpfr_set_ld(dy, -MIN_SINGLE_STEP, MPFR_RNDN);
	} else {
		mpfr_set_ld(tmp, -0.2 * fractalImage.height, MPFR_RNDN);
		mpfr_modf(dy, tmp, tmp, MPFR_RNDN);
		mpfr_mul(dy, dy, fractal.spanY, MPFR_RNDN);
		mpfr_div_ui(dy, dy, fractalImage.height, MPFR_RNDN);
	}
	mpfr_set_ui(tmp, 0, MPFR_RNDN);
	moveFractal(tmp, dy, true);

	mpfr_clear(dy);
	mpfr_clear(tmp);
}

void FractalExplorer::moveDownFractal()
{
	mpfr_t dy, tmp;
	mpfr_init(dy);
	mpfr_init(tmp);
	if (mpfr_cmp_ui(fractal.spanY,0) == 0) {
		mpfr_set_ld(dy, MIN_SINGLE_STEP, MPFR_RNDN);
	} else {
		mpfr_set_ld(tmp, 0.2 * fractalImage.height, MPFR_RNDN);
		mpfr_modf(dy, tmp, tmp, MPFR_RNDN);
		mpfr_mul(dy, dy, fractal.spanY, MPFR_RNDN);
		mpfr_div_ui(dy, dy, fractalImage.height, MPFR_RNDN);
	}
	mpfr_set_ui(tmp, 0, MPFR_RNDN);
	moveFractal(tmp, dy, true);

	mpfr_clear(dy);
	mpfr_clear(tmp);
}

void FractalExplorer::zoomInFractal(const mpfr_t newSpanX, const mpfr_t zoomCenterX, const mpfr_t zoomCenterY,
					bool emitFractalChanged)
{
	cancelActionIfNotFinished();

	if (mpfr_cmp_ui(newSpanX, 0) == 0) {
		fractalQImage->fill(0);
		mpfr_set_ui(fractal.spanX, 0, MPFR_RNDN);
		mpfr_set_ui(fractal.spanY, 0, MPFR_RNDN);
	} else {
		mpfr_t newSpanY, newCenterX, newCenterY, ratioSpanX;
		mpfr_init(newSpanY);
		mpfr_init(newCenterX);
		mpfr_init(newCenterY);
		mpfr_init(ratioSpanX);

		mpfr_div(ratioSpanX, newSpanX, fractal.spanX, MPFR_RNDN);

		mpfr_mul_ui(newSpanY, newSpanX, fractalImage.height, MPFR_RNDN);
		mpfr_div_ui(newSpanY, newSpanY, fractalImage.width, MPFR_RNDN);

		mpfr_sub(newCenterX, fractal.centerX, zoomCenterX, MPFR_RNDN);
		mpfr_mul(newCenterX, newCenterX, ratioSpanX, MPFR_RNDN);
		mpfr_add(newCenterX, newCenterX, zoomCenterX, MPFR_RNDN);

		mpfr_sub(newCenterY, fractal.centerY, zoomCenterY, MPFR_RNDN);
		mpfr_mul(newCenterY, newCenterY, newSpanY, MPFR_RNDN);
		mpfr_div(newCenterY, newCenterY, fractal.spanY, MPFR_RNDN);
		mpfr_add(newCenterY, newCenterY, zoomCenterY, MPFR_RNDN);

		QImage copiedImage(fractalQImage->copy());
		fractalQImage->fill(0);
		QPainter painter(fractalQImage);
		painter.setRenderHint(QPainter::SmoothPixmapTransform);
		QRect dstRect(fractalQImage->rect());

		QRectF srcRect(0, 0, dstRect.width() * mpfr_get_ld(ratioSpanX,MPFR_RNDN),
					dstRect.height() * mpfr_get_ld(ratioSpanX,MPFR_RNDN)); 

		mpfr_t srcRectCenterX, srcRectCenterY;
		mpfr_init(srcRectCenterX);
		mpfr_init(srcRectCenterY);
		mpfr_sub(srcRectCenterX, newCenterX, fractal.x1, MPFR_RNDN);
		mpfr_div(srcRectCenterX, srcRectCenterX, fractal.spanX, MPFR_RNDN);
		mpfr_mul_ui(srcRectCenterX, srcRectCenterX, fractalQImage->width(), MPFR_RNDN);
		mpfr_sub(srcRectCenterY, newCenterY, fractal.y1, MPFR_RNDN);
		mpfr_div(srcRectCenterY, srcRectCenterY, fractal.spanY, MPFR_RNDN);
		mpfr_mul_ui(srcRectCenterY, srcRectCenterY, fractalQImage->height(), MPFR_RNDN);

		srcRect.moveCenter(QPointF(mpfr_get_ld(srcRectCenterX, MPFR_RNDN), mpfr_get_ld(srcRectCenterY, MPFR_RNDN)));
		painter.drawImage(dstRect, copiedImage, srcRect);

		mpfr_set(fractal.centerX, newCenterX, MPFR_RNDN);
		mpfr_set(fractal.centerY, newCenterY, MPFR_RNDN);
		mpfr_set(fractal.spanX, newSpanX, MPFR_RNDN);
		mpfr_set(fractal.spanY, newSpanY, MPFR_RNDN);

		mpfr_clear(newSpanY);
		mpfr_clear(newCenterX);
		mpfr_clear(newCenterY);
		mpfr_clear(ratioSpanX);
		mpfr_clear(srcRectCenterX);
		mpfr_clear(srcRectCenterY);
	}
	reInitFractal();
	if (emitFractalChanged) {
		emit fractalChanged(fractal);
	}
	refresh();
}

void FractalExplorer::zoomOutFractal(const mpfr_t newSpanX, const mpfr_t zoomCenterX, const mpfr_t zoomCenterY,
					bool emitFractalChanged)
{
	cancelActionIfNotFinished();

	if (mpfr_cmp_ui(newSpanX,0) == 0) {
		fractalQImage->fill(0);
		mpfr_set_ui(fractal.spanX, 0, MPFR_RNDN);
		mpfr_set_ui(fractal.spanY, 0, MPFR_RNDN);
	} else {
		mpfr_t newSpanY, newCenterX, newCenterY, ratioSpanX;
		mpfr_init(newSpanY);
		mpfr_init(newCenterX);
		mpfr_init(newCenterY);
		mpfr_init(ratioSpanX);

		mpfr_div(ratioSpanX, newSpanX, fractal.spanX, MPFR_RNDN);

		mpfr_mul_ui(newSpanY, newSpanX, fractalImage.height, MPFR_RNDN);
		mpfr_div_ui(newSpanY, newSpanY, fractalImage.width, MPFR_RNDN);
		if (mpfr_cmp_ui(fractal.spanX,0) == 0 || mpfr_cmp_ui(fractal.spanY,0) == 0) {
			mpfr_set(newCenterX, zoomCenterX, MPFR_RNDN);
			mpfr_set(newCenterY, zoomCenterY, MPFR_RNDN);
		} else {
			mpfr_sub(newCenterX, fractal.centerX, zoomCenterX, MPFR_RNDN);
			mpfr_mul(newCenterX, newCenterX, ratioSpanX, MPFR_RNDN);
			mpfr_add(newCenterX, newCenterX, zoomCenterX, MPFR_RNDN);

			mpfr_sub(newCenterY, fractal.centerY, zoomCenterY, MPFR_RNDN);
			mpfr_mul(newCenterY, newCenterY, newSpanY, MPFR_RNDN);
			mpfr_div(newCenterY, newCenterY, fractal.spanY, MPFR_RNDN);
			mpfr_add(newCenterY, newCenterY, zoomCenterY, MPFR_RNDN);
		}

		QImage copiedImage(fractalQImage->copy());
		fractalQImage->fill(0);
		QPainter painter(fractalQImage);
		painter.setRenderHint(QPainter::SmoothPixmapTransform);
		QRect srcRect(fractalQImage->rect());
		QRectF dstRect(0, 0, srcRect.width() / mpfr_get_ld(ratioSpanX, MPFR_RNDN),
				srcRect.height() / mpfr_get_ld(ratioSpanX, MPFR_RNDN)); 

		mpfr_t dstRectCenterX, dstRectCenterY;
		mpfr_init(dstRectCenterX);
		mpfr_init(dstRectCenterY);

		mpfr_div_ui(dstRectCenterX, newSpanX, 2, MPFR_RNDN);
		mpfr_sub(dstRectCenterX, newCenterX, dstRectCenterX, MPFR_RNDN);
		mpfr_sub(dstRectCenterX, fractal.centerX, dstRectCenterX, MPFR_RNDN);
		mpfr_div(dstRectCenterX, dstRectCenterX, newSpanX, MPFR_RNDN);
		mpfr_mul_ui(dstRectCenterX, dstRectCenterX, srcRect.width(), MPFR_RNDN);

		mpfr_div_ui(dstRectCenterY, newSpanY, 2, MPFR_RNDN);
		mpfr_sub(dstRectCenterY, newCenterY, dstRectCenterY, MPFR_RNDN);
		mpfr_sub(dstRectCenterY, fractal.centerY, dstRectCenterY, MPFR_RNDN);
		mpfr_div(dstRectCenterY, dstRectCenterY, newSpanY, MPFR_RNDN);
		mpfr_mul_ui(dstRectCenterY, dstRectCenterY, srcRect.height(), MPFR_RNDN);
		
		dstRect.moveCenter(QPointF(mpfr_get_ld(dstRectCenterX, MPFR_RNDN), mpfr_get_ld(dstRectCenterY, MPFR_RNDN)));

		painter.drawImage(dstRect, copiedImage, srcRect);

		mpfr_set(fractal.centerX, newCenterX, MPFR_RNDN);
		mpfr_set(fractal.centerY, newCenterY, MPFR_RNDN);
		mpfr_set(fractal.spanX, newSpanX, MPFR_RNDN);
		mpfr_set(fractal.spanY, newSpanY, MPFR_RNDN);

		mpfr_clear(newSpanY);
		mpfr_clear(newCenterX);
		mpfr_clear(newCenterY);
		mpfr_clear(ratioSpanX);
		mpfr_clear(dstRectCenterX);
		mpfr_clear(dstRectCenterY);
	}
	reInitFractal();
	if (emitFractalChanged) {
		emit fractalChanged(fractal);
	}
	refresh();
}

void FractalExplorer::zoomInFractal()
{
	mpfr_t newSpanX, tmp;
	mpfr_init(newSpanX);
	mpfr_init(tmp);
	if (mpfr_cmp_ui(fractal.spanX,0) == 0) {
		mpfr_set_ld(newSpanX, MIN_SINGLE_STEP, MPFR_RNDN);
	} else {
		mpfr_set_ld(tmp, (1 - 0.3) * fractalImage.width, MPFR_RNDN);
		mpfr_modf(newSpanX, tmp, tmp, MPFR_RNDN);
		mpfr_mul(newSpanX, newSpanX, fractal.spanX, MPFR_RNDN);
		mpfr_div_ui(newSpanX, newSpanX, fractalImage.width, MPFR_RNDN);
	}
	zoomInFractal(newSpanX, fractal.centerX, fractal.centerY, true);
	mpfr_clear(newSpanX);
	mpfr_clear(tmp);
}

void FractalExplorer::zoomOutFractal()
{
	mpfr_t newSpanX, tmp;
	mpfr_init(newSpanX);
	mpfr_init(tmp);
	if (mpfr_cmp_ui(fractal.spanX,0) == 0) {
		mpfr_set_ld(newSpanX, MIN_SINGLE_STEP, MPFR_RNDN);
	} else {
		mpfr_set_ld(tmp, (1 + 0.3) * fractalImage.width, MPFR_RNDN);
		mpfr_modf(newSpanX, tmp, tmp, MPFR_RNDN);
		mpfr_mul(newSpanX, newSpanX, fractal.spanX, MPFR_RNDN);
		mpfr_div_ui(newSpanX, newSpanX, fractalImage.width, MPFR_RNDN);
	}
	zoomOutFractal(newSpanX, fractal.centerX, fractal.centerY, true);
	mpfr_clear(newSpanX);
	mpfr_clear(tmp);
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
		mpfr_set(fractalCenterXOnPress, fractal.centerX, MPFR_RNDN);
		mpfr_set(fractalCenterYOnPress, fractal.centerY, MPFR_RNDN);
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
		mpfr_t dx, dy;
		mpfr_init(dx);
		mpfr_init(dy);

		mpfr_mul_d(dx, fractal.spanX, vect.x(), MPFR_RNDN);
		mpfr_div_ui(dx, dx, fractalImage.width, MPFR_RNDN);

		mpfr_mul_d(dy, fractal.spanY, vect.y(), MPFR_RNDN);
		mpfr_div_ui(dy, dy, fractalImage.height, MPFR_RNDN);

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

		mpfr_sub(fractal.centerX, fractalCenterXOnPress, dx, MPFR_RNDN);
		mpfr_sub(fractal.centerY, fractalCenterYOnPress, dy, MPFR_RNDN);

		mpfr_clear(dx);
		mpfr_clear(dy);
		
		reInitFractal();
		emit fractalChanged(fractal);
	} else if (movingFractalRealTime) {
		QVector2D vect(event->posF()-prevMousePos);
		mpfr_t dx, dy;
		mpfr_init(dx);
		mpfr_init(dy);

		mpfr_mul_d(dx, fractal.spanX, -vect.x(), MPFR_RNDN);
		mpfr_div_ui(dx, dx, fractalImage.width, MPFR_RNDN);

		mpfr_mul_d(dy, fractal.spanY, -vect.y(), MPFR_RNDN);
		mpfr_div_ui(dy, dy, fractalImage.height, MPFR_RNDN);

		moveFractal(dx, dy, true);
		prevMousePos = event->posF();

		mpfr_clear(dx);
		mpfr_clear(dy);
	} else {
		QLabel::mouseMoveEvent(event);
	}
	update();
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
		mpfr_t newSpanX;
		mpfr_init(newSpanX);
		if (mpfr_cmp_ui(fractal.spanX, 0) == 0) {
			mpfr_set_ld(newSpanX, MIN_SINGLE_STEP, MPFR_RNDN);
		} else {
			mpfr_mul_d(newSpanX, fractal.spanX, powl(1 - signl(numSteps)*0.3, fabsl(numSteps)), MPFR_RNDN);
		}
		mpfr_t zoomCenterX, zoomCenterY;
		mpfr_init(zoomCenterX);
		mpfr_init(zoomCenterY);
		if (mpfr_cmp_ui(fractal.spanX,0) == 0 || mpfr_cmp_ui(fractal.spanY,0) == 0) {
			mpfr_set(zoomCenterX, fractal.centerX, MPFR_RNDN);
			mpfr_set(zoomCenterY, fractal.centerY, MPFR_RNDN);
		} else {
			mpfr_mul_d(zoomCenterX, fractal.spanX, (event->x() + 0.5) / fractalImage.width, MPFR_RNDN);
			mpfr_add(zoomCenterX, zoomCenterX, fractal.x1, MPFR_RNDN);

			mpfr_mul_d(zoomCenterY, fractal.spanY, (event->y() + 0.5) / fractalImage.height, MPFR_RNDN);
			mpfr_add(zoomCenterY, zoomCenterY, fractal.y1, MPFR_RNDN);
		}
		if (numSteps > 0) {
			zoomInFractal(newSpanX, zoomCenterX, zoomCenterY, true);
		} else {
			zoomOutFractal(newSpanX, zoomCenterX, zoomCenterY, true);
		}
		mpfr_clear(newSpanX);
		mpfr_clear(zoomCenterX);
		mpfr_clear(zoomCenterY);
	}
	event->accept();
	update();
}

void FractalExplorer::setFractalFormula(int index)
{
	cancelActionIfNotFinished();

	fractal.fractalFormula = (FractalFormula)index;
	reInitFractal();
	emit fractalChanged(fractal);

	refresh();
}

void FractalExplorer::setPParam(const mpc_t *value)
{
	cancelActionIfNotFinished();

	mpc_set(fractal.p, *value, MPC_RNDNN);
	reInitFractal();

	refresh();
}

void FractalExplorer::setPParamRe(const mpfr_t *value)
{
	cancelActionIfNotFinished();

	mpfr_t im;
	mpfr_init(im);
	mpfr_set(im, mpc_imagref(fractal.p), MPFR_RNDN);
	mpc_set_fr_fr(fractal.p, *value, im, MPC_RNDNN);
	mpfr_clear(im);
	reInitFractal();

	refresh();
}

void FractalExplorer::setPParamIm(const mpfr_t *value)
{
	cancelActionIfNotFinished();

	mpfr_t re;
	mpfr_init(re);
	mpfr_set(re, mpc_realref(fractal.p), MPFR_RNDN);
	mpc_set_fr_fr(fractal.p, re, *value, MPC_RNDNN);
	mpfr_clear(re);
	reInitFractal();

	refresh();
}

void FractalExplorer::setCParamRe(const mpfr_t *value)
{
	cancelActionIfNotFinished();

	mpfr_t im;
	mpfr_init(im);
	mpfr_set(im, mpc_imagref(fractal.c), MPFR_RNDN);
	mpc_set_fr_fr(fractal.c, *value, im, MPC_RNDNN);
	mpfr_clear(im);
	reInitFractal();

	refresh();
}

void FractalExplorer::setCParamIm(const mpfr_t *value)
{
	cancelActionIfNotFinished();

	mpfr_t re;
	mpfr_init(re);
	mpfr_set(re, mpc_realref(fractal.c), MPFR_RNDN);
	mpc_set_fr_fr(fractal.c, re, *value, MPC_RNDNN);
	mpfr_clear(re);
	reInitFractal();

	refresh();
}

void FractalExplorer::setCenterX(const mpfr_t *value)
{
	mpfr_t dx, dy;
	mpfr_init(dx);
	mpfr_init(dy);

	mpfr_sub(dx, *value, fractal.centerX, MPFR_RNDN);
	mpfr_set_ui(dy, 0, MPFR_RNDN);

	moveFractal(dx, dy);

	mpfr_clear(dx);
	mpfr_clear(dy);
}

void FractalExplorer::setCenterY(const mpfr_t *value)
{
	mpfr_t dx, dy;
	mpfr_init(dx);
	mpfr_init(dy);

	mpfr_set_ui(dx, 0, MPFR_RNDN);
	mpfr_sub(dy, *value, fractal.centerY, MPFR_RNDN);

	moveFractal(dx, dy);

	mpfr_clear(dx);
	mpfr_clear(dy);
}

void FractalExplorer::setSpanX(const mpfr_t *value)
{
	if (mpfr_cmp(*value, fractal.spanX) > 0) {
		/* Zoom out */
		zoomOutFractal(*value, fractal.centerX, fractal.centerY);
	} else {
		/* Zoom in */
		zoomInFractal(*value, fractal.centerX, fractal.centerY);
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

void FractalExplorer::setIterationCount(int index)
{
	cancelActionIfNotFinished();

	render.iterationCount = (IterationCount)index;
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

void FractalExplorer::setSpaceColor(const QColor &color)
{
	cancelActionIfNotFinished();

	render.spaceColor = ColorFromRGB(1, (uint16_t)color.red(),
				(uint16_t)color.green(), (uint16_t)color.blue());
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

	/* Now gradient FractalNow gradient from well formed gradient.*/
	double positionStop[wellFormedGradientStops.size()];
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

void FractalExplorer::setFloatPrecision(int index)
{
	cancelActionIfNotFinished();

	floatPrecision = (FloatPrecision)index;

	refresh();
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
	UNUSED(action);
}

