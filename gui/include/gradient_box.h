/*
 *  gradient_box.h -- part of FractalNow
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
  * \file gradient_box.h
  * \brief Header file introducing MainWindow class.
  *
  * \author Marc Pegon
  */

#ifndef __GRADIENT_BOX_H__
#define __GRADIENT_BOX_H__

#include "gradient_label.h"

#include "fractalnow.h"

#include <QWidget>

class GradientBox : public QWidget
{
	Q_OBJECT

	public:
	GradientBox(const QGradientStops &gradientStops, QWidget *parent = 0);
	GradientBox(const Gradient &gradient, QWidget *parent = 0);

	public slots:
	void setGradient(const Gradient &gradient);
	void setGradientStops(const QGradientStops &gradientStops);
	void openGradientDialog();

	private:
	void initGradientBox(const QGradientStops &gradientStops);
	GradientLabel *gradientLabel;
	QGradientStops gradientStops;

	signals:
	void gradientStopsChanged(const QGradientStops &gradientStops);
};

#endif
