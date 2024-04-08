/*
 *  color_button.h -- part of fractal2D
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
  * \file color_button.h
  * \brief Header file introducing ColorButton class.
  *
  * \author Marc Pegon
  */

#ifndef _COLOR_BUTTON__H__
#define _COLOR_BUTTON__H__

#include <QPushButton>
#include <QColorDialog>

class ColorButton : public QPushButton
{
	Q_OBJECT

	public:
	ColorButton(QWidget *parent = 0);
	QColor currentColor();

	private:
	QColor color;
	QColorDialog *colorDialog;
	void updateIcon();

	public slots:
	void setCurrentColor(QColor color);

	private slots:
	void openColorDialog();

	signals:
	void currentColorChanged(const QColor &color);
};

#endif

