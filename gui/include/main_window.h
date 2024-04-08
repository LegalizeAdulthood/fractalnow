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
  * \file main_window.h
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
#include "fractal_explorer.h"
#include <QCloseEvent>
#include <QDesktopServices>
#include <QDoubleSpinBox>
#include <QMainWindow>
#include <QSpinBox>

class MainWindow : public QMainWindow
{
	Q_OBJECT

	public:
	MainWindow(int argc, char *argv[]);

	private:
	FractalExplorer *fractalExplorer;
	FractalConfigWidget *fractalConfigWidget;
	FractalRenderingWidget *fractalRenderingWidget;
	QSpinBox *preferredImageWidthSpinBox;
	QSpinBox *preferredImageHeightSpinBox;
	QAction *adaptExplorerToWindowAction;
	QWidget *centralWidget;

	private slots:
	void aboutQt();
	void aboutQFractal2D();
	void adaptExplorerToWindow(bool checked);
	void exportImage();
	void onPreferredImageWidthChanged();
	void onPreferredImageHeightChanged();
};

#endif
