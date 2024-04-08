/*
 *  fractal_explorer.h -- part of FractalNow
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
 
 /**
  * \file fractal_explorer.h
  * \brief Header file introducing FractalExplorer class.
  *
  * \author Marc Pegon
  */

#ifndef _FRACTAL_EXPLORER__H__
#define _FRACTAL_EXPLORER__H__

#include "main.h"

#include "fractalnow.h"

#include <mpfr.h>
#include <mpc.h>

#include <QAction>
#include <QFutureWatcher>
#include <QImage>
#include <QLabel>
#include <QPaintEvent>

class FractalExplorer : public QLabel
{
	Q_OBJECT
	 
	public:
	FractalExplorer(const FractalConfig &fractalConfig,
			uint_fast32_t width, uint_fast32_t height,
			uint_fast32_t minAntiAliasingSize,
			uint_fast32_t maxAntiAliasingSize,
			uint_fast32_t antiAliasingSizeIteration,
			uint_fast32_t quadInterpolationSize,
			double colorDissimilarityThreshold,
			double adaptiveAAMThreshold,
			uint_fast32_t nbThreads,
			QWidget *parent = 0, Qt::WindowFlags f = 0);
	const FractalConfig &getFractalConfig() const;
	const Fractal &getFractal() const;
	const RenderingParameters &getRender() const;
	bool getFractalCacheEnabled() const;
	int getFractalCacheSize() const;
	bool getSolidGuessingEnabled() const;
	void launchFractalDrawing();
	void launchFractalAntiAliasing();
	void resizeImage(uint_fast32_t width, uint_fast32_t height);
	QSize sizeHint() const;
	~FractalExplorer();

	QAction *restoreInitialStateAction;
	QAction *stopDrawingAction;
	QAction *refreshAction;
	QAction *zoomInAction;
	QAction *zoomOutAction;
	QAction *moveLeftAction;
	QAction *moveRightAction;
	QAction *moveUpAction;
	QAction *moveDownAction;

	public slots:
	void restoreInitialState();
	void refresh();
	int stopDrawing(); // return 1 if drawing was not active anyway
	void pauseDrawing();
	void resumeDrawing();
	void zoomInFractal();
	void zoomOutFractal();
	void moveUpFractal();
	void moveDownFractal();
	void moveLeftFractal();
	void moveRightFractal();

	void useFractalCache(bool enabled);
	void resizeFractalCache(int size);
	void setSolidGuessingEnabled(bool enabled);

	void setFractalFormula(int index);
	void setPParam(const mpc_t *value);
	void setPParamRe(const mpfr_t *value);
	void setPParamIm(const mpfr_t *value);
	void setCParamRe(const mpfr_t *value);
	void setCParamIm(const mpfr_t *value);
	void setCenterX(const mpfr_t *value);
	void setCenterY(const mpfr_t *value);
	void setSpanX(const mpfr_t *value);
	void setBailoutRadius(double value);
	void setMaxIterations(int value);

	void setAddendFunction(int index);
	void setStripeDensity(int value);
	void setColoringMethod(int index);
	void setIterationCount(int index);
	void setInterpolationMethod(int index);
	void setTransferFunction(int index);
	void setColorScaling(double value);
	void setColorOffset(double value);
	void setSpaceColor(const QColor &color);
	void setGradient(const QGradientStops &gradientStops);
	void setFractalConfig(const FractalConfig &fractalConfig);
	void setFractal(const Fractal &fractal);
	void setRenderingParameters(const RenderingParameters &render);
	void setGradient(const Gradient &gradient);
	void setFloatPrecision(int index);

	private:
	void paintEvent(QPaintEvent *event);
	void resizeEvent(QResizeEvent * event);
	void mousePressEvent(QMouseEvent *event);
	void mouseReleaseEvent(QMouseEvent *event);
	void mouseMoveEvent(QMouseEvent *event);
	void wheelEvent(QWheelEvent *event);
	void contextMenuEvent(QContextMenuEvent *event);

	int cancelActionIfNotFinished();
	void adjustSpan();
	void reInitFractal();
	void reInitRenderingParameters();
	void moveFractal(const mpfr_t dx, const mpfr_t dy, bool emitFractalChanged = false);
	void zoomInFractal(const mpfr_t newSpanX, const mpfr_t zoomCenterX,
			const mpfr_t zoomCenterY, bool emitFractalChanged = false);
	void zoomOutFractal(const mpfr_t newSpanX, const mpfr_t zoomCenterX,
			const mpfr_t zoomCenterY, bool emitFractalChanged = false);

	enum ActionType {
		A_FractalDrawing = 0,
		A_FractalAntiAliasing
	};
	enum ActionType lastActionType;
	bool drawingPaused;
	bool redrawFractal;
	double currentAntiAliasingSize;
	bool movingFractalDeferred;
	bool movingFractalRealTime;
	bool fractalMoved;
	mpfr_t fractalCenterXOnPress, fractalCenterYOnPress;
	QPointF prevMousePos;
	QPointF mousePosOnPress;
	QImage imageCopyOnPress;

	uint_fast32_t initialWidth, initialHeight;
	FractalConfig fractalConfig, initialFractalConfig;
	Fractal &fractal;
	RenderingParameters &render;
	uint_fast32_t quadInterpolationSize;
	double colorDissimilarityThreshold;
	double adaptiveAAMThreshold;
	FloatPrecision floatPrecision;
	bool solidGuessing;
	FractalCache cache;
	FractalCache *pCache;
	uint_fast32_t minAntiAliasingSize;
	uint_fast32_t maxAntiAliasingSize;
	uint_fast32_t antiAliasingSizeIteration;
	Threads *threads;
	QImage *fractalQImage;
	Image fractalImage;
	Task *task;
	QTimer *timer;

	private slots:
	void onTimeout();

	signals:
	void wakeUpSignal();
	void fractalChanged(const Fractal &);
	void renderingParametersChanged(const RenderingParameters &);
	void fractalCacheSizeChanged(int size);
};

#endif
