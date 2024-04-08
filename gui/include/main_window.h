/*
 *  main_window.h -- part of fractal2D
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
  * \file help.h
  * \brief Header file introducing MainWindow class.
  *
  * \author Marc Pegon
  */

#ifndef __MAIN_WINDOW_H__
#define __MAIN_WINDOW_H__

#include "command_line.h"
#include "fractal.h"
#include "fractal_config_widget.h"
#include "fractal_rendering_parameters.h"
#include "fractal_rendering_widget.h"
#include "image.h"
#include "fractal_image_label.h"
#include <QCloseEvent>
#include <QDoubleSpinBox>
#include <QFuture>
#include <QFutureWatcher>
#include <QKeyEvent>
#include <QMainWindow>
#include <QImage>
#include <QTimer>

#define DEFAULT_DECIMAL_NUMBER (int)(20)

class MainWindow : public QMainWindow
{
	Q_OBJECT

	public:
	MainWindow(int argc, char *argv[]);
	~MainWindow();

	private:
	void closeEvent(QCloseEvent * event);
	void reInitFractal();
	void reInitRenderingParameters();
	void launchFractalDrawing();
	void launchFractalAntiAliasing();
	void cancelActionIfNotFinished();
	void moveFractal(double dx, double dy);
	void zoomInFractal(double newSpanX, double zoomCenterX, double zoomCenterY);
	void zoomOutFractal(double newSpanX, double zoomCenterX, double zoomCenterY);

	CommandLineArguments *args;
	Fractal fractal;
	RenderingParameters render;
	Image fractalImage;
	QImage *fractalQImage;
	FractalImageLabel *fractalImageLabel;
	FractalConfigWidget *fractalConfigWidget;
	FractalRenderingWidget *fractalRenderingWidget;

	double currentAntiAliasingSize;
	int movingFractalDeferred;
	int movingFractalRealTime;
	int fractalMoved;
	double fractalCenterXOnPress, fractalCenterYOnPress;
	QPointF prevMousePos;
	QPointF mousePosOnPress;
	QImage imageCopyOnPress;

	Action *action;
	QTimer *timer;
	QFutureWatcher<int> fractalDrawingWatcher;
	QFutureWatcher<int> fractalAntialiasingWatcher;
	QFuture<int> future;

	QAction *zoomInAction;
	QAction *zoomOutAction;
	QAction *moveLeftAction;
	QAction *moveRightAction;
	QAction *moveUpAction;
	QAction *moveDownAction;

	private slots:
	void showImageLabelContextMenu(const QPoint&);

	void wakeUpIfAsleep();
	void onFractalDrawingFinished();
	void onFractalAntiAliasingFinished();
	void zoomInFractal();
	void zoomOutFractal();
	void moveUpFractal();
	void moveDownFractal();
	void moveLeftFractal();
	void moveRightFractal();

	void fractalFormulaChanged(int index);
	void pParamChanged(double value);
	void cParamReChanged(double value);
	void cParamImChanged(double value);
	void centerXChanged(double value);
	void centerYChanged(double value);
	void spanXChanged(double value);
	void bailoutRadiusChanged(double value);
	void maxIterationsChanged(int value);

	void addendFunctionChanged(int index);
	void stripeDensityChanged(int value);
	void coloringMethodChanged(int index);
	void countingFunctionChanged(int index);
	void interpolationMethodChanged(int index);
	void transferFunctionChanged(int index);
	void colorScalingChanged(double value);
	void colorOffsetChanged(double value);
	void spaceColorChanged(QColor color);

	void fractalImageLabelMousePressed(QMouseEvent *event);
	void fractalImageLabelMouseReleased(QMouseEvent *event);
	void fractalImageLabelMouseMoved(QMouseEvent *event);
	void fractalImageLabelMouseWheelRotated(QWheelEvent *event);

	signals:
	void wakeUpSignal();
};

#endif
