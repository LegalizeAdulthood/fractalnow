/*
 *  gradient_dialog.h -- part of FractalNow
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
  * \file gradient_dialog.h
  * \brief Header file introducing ColorButton class.
  *
  * \author Marc Pegon
  */

#ifndef _GRADIENT_DIALOG__H__
#define _GRADIENT_DIALOG__H__

#include "gradient_editor.h"

#include <QDialog>
#include <QGradient>

class GradientDialog : public QDialog
{
	Q_OBJECT

	public:
	GradientDialog(const QGradientStops &gradientStops, QWidget *parent = 0, 
			Qt::WindowFlags f = 0);
	const QGradientStops &currentGradientStops() const;

	private:
	GradientEditor *gradientEditor;
	QGradientStops gradientStops;
	
	private slots:
	void initGradientEditor();
	void onGradientStopsChanged(const QGradientStops &gradientStops);
};

#endif

