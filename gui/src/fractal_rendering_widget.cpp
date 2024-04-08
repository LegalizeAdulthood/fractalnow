/*
 *  fractal_rendering_widget.cpp -- part of fractal2D
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

#include "fractal_rendering_widget.h"
#include "fractal_addend_function.h"
#include "fractal_coloring.h"
#include "fractal_counting_function.h"
#include "fractal_transfer_function.h"
#include <stdint.h>
#include <limits>
#include <QFormLayout>
#include <QLabel>

FractalRenderingWidget::FractalRenderingWidget(RenderingParameters &render) : QWidget()
{
	countingFunctionComboBox = new QComboBox;
	for (uint_fast32_t i = 0; i < nbCountingFunctions; ++i) {
		countingFunctionComboBox->addItem(CountingFunctionDescStr[i]);
	}
	countingFunctionComboBox->setCurrentIndex((int)render.countingFunction);

	coloringMethodComboBox = new QComboBox;
	for (uint_fast32_t i = 0; i < nbColoringMethods; ++i) {
		coloringMethodComboBox->addItem(ColoringMethodDescStr[i]);
	}
	coloringMethodComboBox->setCurrentIndex((int)render.coloringMethod);

	addendFunctionComboBox = new QComboBox;
	for (uint_fast32_t i = 0; i < nbAddendFunctions; ++i) {
		addendFunctionComboBox->addItem(AddendFunctionDescStr[i]);
	}
	addendFunctionComboBox->setCurrentIndex((int)render.addendFunction);

	stripeDensitySpinBox = new QSpinBox;
	stripeDensitySpinBox->setRange(1, std::numeric_limits<int>::max());
	stripeDensitySpinBox->setValue((int)render.stripeDensity);
	stripeDensitySpinBox->setAccelerated(true);

	interpolationMethodComboBox = new QComboBox;
	for (uint_fast32_t i = 0; i < nbInterpolationMethods; ++i) {
		interpolationMethodComboBox->addItem(InterpolationMethodDescStr[i]);
	}
	interpolationMethodComboBox->setCurrentIndex((int)render.interpolationMethod);

	transferFunctionComboBox = new QComboBox;
	for (uint_fast32_t i = 0; i < nbTransferFunctions; ++i) {
		transferFunctionComboBox->addItem(TransferFunctionDescStr[i]);
	}
	transferFunctionComboBox->setCurrentIndex((int)render.transferFunction);

	spaceColorButton = new ColorButton;
	spaceColorButton->setCurrentColor(QColor(render.spaceColor.r, render.spaceColor.g,
					render.spaceColor.b));

	colorScalingSpinBox = new QDoubleSpinBox;
	colorScalingSpinBox->setDecimals(8);
	colorScalingSpinBox->setRange(0, std::numeric_limits<double>::max());
	colorScalingSpinBox->setValue(render.multiplier);
	colorScalingSpinBox->setAccelerated(true);

	colorOffsetSpinBox = new QDoubleSpinBox;
	colorOffsetSpinBox->setDecimals(6);
	colorOffsetSpinBox->setRange(0, 1);
	colorOffsetSpinBox->setSingleStep(0.01);
	colorOffsetSpinBox->setValue(render.offset);
	colorOffsetSpinBox->setAccelerated(true);

	QFormLayout *formLayout = new QFormLayout;
	formLayout->addRow(tr("Counting function:"), countingFunctionComboBox);
	formLayout->addRow(tr("Coloring method:"), coloringMethodComboBox);
	formLayout->addRow(tr("Addend function:"), addendFunctionComboBox);
	formLayout->addRow(tr("Stripe density:"), stripeDensitySpinBox);
	formLayout->addRow(tr("Interpolation method:"), interpolationMethodComboBox);
	formLayout->addRow(tr("Transfer function:"), transferFunctionComboBox);
	formLayout->addRow(tr("Color scaling:"), colorScalingSpinBox);
	formLayout->addRow(tr("Color offset:"), colorOffsetSpinBox);
	formLayout->addRow(tr("Space color:"), spaceColorButton);
	this->setLayout(formLayout);

	updateBoxesEnabledValue();
	updateColorScalingSingleStep();

	connect(coloringMethodComboBox, SIGNAL(currentIndexChanged(int)),
		this, SLOT(updateBoxesEnabledValue()));
	connect(addendFunctionComboBox, SIGNAL(currentIndexChanged(int)),
		this, SLOT(updateBoxesEnabledValue()));
	connect(colorScalingSpinBox, SIGNAL(valueChanged(double)), this, SLOT(updateColorScalingSingleStep()));
}

void FractalRenderingWidget::updateBoxesEnabledValue()
{
	ColoringMethod coloringMethod = (ColoringMethod)coloringMethodComboBox->currentIndex();
	AddendFunction addendFunction = (AddendFunction)addendFunctionComboBox->currentIndex();

	switch (coloringMethod) {
	case CM_SIMPLE:
		addendFunctionComboBox->setEnabled(false);
		stripeDensitySpinBox->setEnabled(false);
		interpolationMethodComboBox->setEnabled(false);
		break;
	case CM_AVERAGE:
		addendFunctionComboBox->setEnabled(true);
		stripeDensitySpinBox->setEnabled(addendFunction == AF_STRIPE);
		interpolationMethodComboBox->setEnabled(true);
		break;
	default:
		addendFunctionComboBox->setEnabled(true);
		stripeDensitySpinBox->setEnabled(true);
		interpolationMethodComboBox->setEnabled(true);
		break;
	}
}

void FractalRenderingWidget::updateColorScalingSingleStep()
{
	double colorScaling = colorScalingSpinBox->value();
	colorScalingSpinBox->setSingleStep(colorScaling / 10);
}

