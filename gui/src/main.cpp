/*
 *  main.cpp -- part of FractalNow
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
 
#include "main.h"
#include "main_window.h"

#include <QApplication>
 
int main(int argc, char *argv[])
{
	QApplication::setGraphicsSystem(QString("raster"));
	QApplication::setApplicationName("QFractalNow");
	QApplication::setOrganizationName("fractalnow");
	QApplication::setApplicationVersion(QString(FractalNow_VersionNumber()));
	QApplication app(argc, argv);

	MainWindow mainWindow(argc, argv);
	mainWindow.show();

	return app.exec();
}

