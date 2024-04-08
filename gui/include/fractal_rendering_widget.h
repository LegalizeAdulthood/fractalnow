/*
 *  fractal_rendering_widget.h -- part of FractalNow
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
  * \brief Header file introducing FractalRenderingWidget class.
  *
  * \author Marc Pegon
  */

#ifndef __FRACTAL_RENDERING_WIDGET_H__
#define __FRACTAL_RENDERING_WIDGET_H__

#include "fractal_rendering_parameters.h"
#include "color_button.h"
#include "gradient_box.h"
#include <QComboBox>
#include <QDoubleSpinBox>
#include <QWidget>

class FractalRenderingWidget : public QWidget
{
	Q_OBJECT

	public:
	FractalRenderingWidget(const RenderingParameters &render);

	QComboBox *countingFunctionComboBox;
	QComboBox *coloringMethodComboBox;
	QComboBox *addendFunctionComboBox;
	QSpinBox *stripeDensitySpinBox;
	QComboBox *interpolationMethodComboBox;
	QComboBox *transferFunctionComboBox;
	QDoubleSpinBox *colorScalingSpinBox;
	QDoubleSpinBox *colorOffsetSpinBox;
	ColorButton *spaceColorButton;
	GradientBox *gradientBox;

	public slots:
	void updateBoxesValues(const RenderingParameters &render);
	void updateBoxesEnabledValue();
	void updateColorScalingSingleStep();
	void editGradient();

	private:
	void blockBoxesSignals(bool block);
};

#endif
