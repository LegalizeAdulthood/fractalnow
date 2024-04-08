/*
 *  fractal_config_widget.h -- part of fractal2D
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
  * \file fractal_config_widget.h
  * \brief Header file introducing FractalConfigWidget class.
  *
  * \author Marc Pegon
  */

#ifndef __FRACTAL_CONFIG_WIDGET_H__
#define __FRACTAL_CONFIG_WIDGET_H__

#include "fractal.h"
#include <QComboBox>
#include <QDoubleSpinBox>
#include <QWidget>

class FractalConfigWidget : public QWidget
{
	Q_OBJECT

	public:
	FractalConfigWidget(Fractal &fractal);
	void blockBoxesSignals(bool block);
	void updateBoxesValues(Fractal &fractal);

	QComboBox *fractalFormulaComboBox;
	QDoubleSpinBox *pParamSpinBox;
	QDoubleSpinBox *cParamReSpinBox;
	QDoubleSpinBox *cParamImSpinBox;
	QDoubleSpinBox *centerXSpinBox;
	QDoubleSpinBox *centerYSpinBox;
	QDoubleSpinBox *spanXSpinBox;
	QDoubleSpinBox *bailoutRadiusSpinBox;
	QSpinBox *maxIterationsSpinBox;

	public slots:
	void updateSpaceBoxesSingleSteps();
	void updateCParamReSingleStep();
	void updateCParamImSingleStep();
	void updateBoxesEnabledValue();
};

#endif
