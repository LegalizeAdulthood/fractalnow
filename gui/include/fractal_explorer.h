/*
 *  fractal_explorer.h -- part of fractal2D
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

#include <QAction>
#include <QFutureWatcher>
#include <QImage>
#include <QLabel>
#include <QPaintEvent>
#include "fractal.h"
#include "fractal_rendering_parameters.h"
#include "image.h"
#include "thread.h"
#include "main.h"

class FractalExplorer : public QLabel
{
	Q_OBJECT
	 
	public:
	FractalExplorer(const Fractal &fractal, const RenderingParameters &render,
				uint_fast32_t width, uint_fast32_t height,
				uint_fast32_t minAntiAliasingSize,
				uint_fast32_t maxAntiAliasingSize,
				uint_fast32_t antiAliasingSizeIteration,
				uint_fast32_t nbThreads,
				QWidget *parent = 0, Qt::WindowFlags f = 0);

	Fractal &getFractal();
	RenderingParameters &getRender();
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
	void zoomInFractal();
	void zoomOutFractal();
	void moveUpFractal();
	void moveDownFractal();
	void moveLeftFractal();
	void moveRightFractal();

	void setFractalFormula(int index);
	void setPParam(double value);
	void setCParamRe(double value);
	void setCParamIm(double value);
	void setCenterX(double value);
	void setCenterY(double value);
	void setSpanX(double value);
	void setBailoutRadius(double value);
	void setMaxIterations(int value);

	void setAddendFunction(int index);
	void setStripeDensity(int value);
	void setColoringMethod(int index);
	void setCountingFunction(int index);
	void setInterpolationMethod(int index);
	void setTransferFunction(int index);
	void setColorScaling(double value);
	void setColorOffset(double value);
	void setSpaceColor(QColor color);
	void setGradient(const QGradientStops &gradientStops);
	void setGradient(const Gradient &gradient);

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
	void moveFractal(double dx, double dy, bool emitFractalChanged = false);
	void zoomInFractal(double newSpanX, double zoomCenterX, double zoomCenterY,
				bool emitFractalChanged = false);
	void zoomOutFractal(double newSpanX, double zoomCenterX, double zoomCenterY,
				bool emitFractalChanged = false);

	enum ActionType {
		A_FractalDrawing = 0,
		A_FractalAntiAliasing
	};
	enum ActionType lastActionType;
	bool redrawFractal;
	double currentAntiAliasingSize;
	bool movingFractalDeferred;
	bool movingFractalRealTime;
	bool fractalMoved;
	double fractalCenterXOnPress, fractalCenterYOnPress;
	QPointF prevMousePos;
	QPointF mousePosOnPress;
	QImage imageCopyOnPress;

	uint_fast32_t initialWidth, initialHeight;
	Fractal fractal, initialFractal;
	RenderingParameters render, initialRender;
	uint_fast32_t minAntiAliasingSize;
	uint_fast32_t maxAntiAliasingSize;
	uint_fast32_t antiAliasingSizeIteration;
	Threads *threads;
	QImage *fractalQImage;
	Image fractalImage;
	Action *action;
	QTimer *timer;

	private slots:
	void onTimeout();

	signals:
	void wakeUpSignal();
	void fractalChanged(Fractal &);
	void renderingParametersChanged(RenderingParameters &);
};

#endif
