/*
 *  main_window.h -- part of FractalNow
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

#include "fractal.h"
#include "fractal_rendering_parameters.h"
#include "image.h"

#include "command_line.h"
#include "export_fractal_image_dialog.h"
#include "fractal_config_widget.h"
#include "fractal_explorer.h"
#include "fractal_rendering_widget.h"

#include <QCheckBox>
#include <QCloseEvent>
#include <QDesktopServices>
#include <QDoubleSpinBox>
#include <QDropEvent>
#include <QMainWindow>
#include <QSpinBox>

#define DEFAULT_EXPLORER_WIDTH (uint)(640)
#define DEFAULT_EXPLORER_HEIGHT (uint)(512)

class MainWindow : public QMainWindow
{
	Q_OBJECT

	public:
	MainWindow(int argc, char *argv[]);
	~MainWindow();

	private:
	enum FileType { UNKNOWN_FILE=0, CONFIG_FILE,
		FRACTAL_FILE, RENDER_FILE, GRADIENT_FILE };

	ExportFractalImageDialog *exportFractalImageDialog;
	QDockWidget *fractalDock, *renderDock, *otherDock;
	QToolBar *toolBar;
	FractalExplorer *fractalExplorer;
	FractalConfigWidget *fractalConfigWidget;
	FractalRenderingWidget *fractalRenderingWidget;
	QSpinBox *preferredImageWidthSpinBox;
	QSpinBox *preferredImageHeightSpinBox;
	QCheckBox *useCacheCheckBox;
	QSpinBox *cacheSizeSpinBox;
	QCheckBox *solidGuessingCheckBox;
	QAction *adaptExplorerToWindowAction;
	uint_fast32_t fractalExplorerNbThreads;
	uint_fast32_t exportImageNbThreads;
	bool adaptExplorerSize;
	uint lastPreferredExplorerWidth, lastPreferredExplorerHeight;
	uint lastWindowWidth, lastWindowHeight;
	QString imageDir;
	QString configDir;
	QString gradientDir;
	bool useCache;
	int cacheSize;
	bool solidGuessing;
	QAction *switchFullScreenAction;

	enum FileType getFileType(const char *fileName);
	void saveSettings();
	void loadSettings();
	void loadFile(const char *fileName);
	void loadConfigFile(const char *fileName);
	void loadFractalFile(const char *fileName);
	void loadRenderingFile(const char *fileName);
	void loadGradientFile(const char *fileName);
	void openFile(QString fileName);
	void dragEnterEvent(QDragEnterEvent *event);
	void dropEvent(QDropEvent *event);

	private slots:
	void delayedInit();
	void aboutQt();
	void aboutQFractalNow();
	void adaptExplorerToWindow(bool checked);
	void exportImage();
	void openConfigFile();
	void saveConfigFile();
	void openGradientFile();
	void saveGradientFile();
	void onPreferredImageWidthChanged();
	void onPreferredImageHeightChanged();
	void onCacheSizeChanged();
	void switchFullScreenMode(bool checked);
	void escapeFullScreen();
};

#endif
