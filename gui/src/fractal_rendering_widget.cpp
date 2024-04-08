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
#include <QGridLayout>
#include <QLabel>

FractalRenderingWidget::FractalRenderingWidget(RenderingParameters &render) : QWidget()
{
	QLabel *countingFunctionLabel = new QLabel(tr("Counting function:"));
	countingFunctionComboBox = new QComboBox;
	for (uint_fast32_t i = 0; i < nbCountingFunctions; ++i) {
		countingFunctionComboBox->addItem(CountingFunctionDescStr[i]);
	}
	countingFunctionComboBox->setCurrentIndex((int)render.countingFunction);

	QLabel *coloringMethodLabel = new QLabel(tr("Coloring method:"));
	coloringMethodComboBox = new QComboBox;
	for (uint_fast32_t i = 0; i < nbColoringMethods; ++i) {
		coloringMethodComboBox->addItem(ColoringMethodDescStr[i]);
	}
	coloringMethodComboBox->setCurrentIndex((int)render.coloringMethod);

	QLabel *addendFunctionLabel = new QLabel(tr("Addend function:"));
	addendFunctionComboBox = new QComboBox;
	for (uint_fast32_t i = 0; i < nbAddendFunctions; ++i) {
		addendFunctionComboBox->addItem(AddendFunctionDescStr[i]);
	}
	addendFunctionComboBox->setCurrentIndex((int)render.addendFunction);

	QLabel *stripeDensityLabel = new QLabel(tr("Stripe density:"));
	stripeDensitySpinBox = new QSpinBox;
	stripeDensitySpinBox->setRange(1, std::numeric_limits<int>::max());
	stripeDensitySpinBox->setValue((int)render.stripeDensity);
	stripeDensitySpinBox->setAccelerated(true);

	QLabel *interpolationMethodLabel = new QLabel(tr("Interpolation method:"));
	interpolationMethodComboBox = new QComboBox;
	for (uint_fast32_t i = 0; i < nbInterpolationMethods; ++i) {
		interpolationMethodComboBox->addItem(InterpolationMethodDescStr[i]);
	}
	interpolationMethodComboBox->setCurrentIndex((int)render.interpolationMethod);

	QLabel *transferFunctionLabel = new QLabel(tr("Transfer function:"));
	transferFunctionComboBox = new QComboBox;
	for (uint_fast32_t i = 0; i < nbTransferFunctions; ++i) {
		transferFunctionComboBox->addItem(TransferFunctionDescStr[i]);
	}
	transferFunctionComboBox->setCurrentIndex((int)render.transferFunction);

	QLabel *spaceColorLabel = new QLabel(tr("Space color:"));
	spaceColorButton = new ColorButton;
	spaceColorButton->setCurrentColor(QColor(render.spaceColor.r, render.spaceColor.g,
					render.spaceColor.b));

	QLabel *colorScalingLabel = new QLabel(tr("Color scaling:"));
	colorScalingSpinBox = new QDoubleSpinBox;
	colorScalingSpinBox->setDecimals(8);
	colorScalingSpinBox->setRange(0, std::numeric_limits<double>::max());
	colorScalingSpinBox->setValue(render.multiplier);
	colorScalingSpinBox->setAccelerated(true);

	QLabel *colorOffsetLabel = new QLabel(tr("Color offset:"));
	colorOffsetSpinBox = new QDoubleSpinBox;
	colorOffsetSpinBox->setDecimals(6);
	colorOffsetSpinBox->setRange(0, 1);
	colorOffsetSpinBox->setSingleStep(0.01);
	colorOffsetSpinBox->setValue(render.offset);
	colorOffsetSpinBox->setAccelerated(true);

	QGridLayout *gridLayout = new QGridLayout;
	gridLayout->addWidget(countingFunctionLabel, 0, 0);
	gridLayout->addWidget(countingFunctionComboBox, 0, 1);

	gridLayout->addWidget(coloringMethodLabel, 1, 0);
	gridLayout->addWidget(coloringMethodComboBox, 1, 1);

	gridLayout->addWidget(addendFunctionLabel, 2, 0);
	gridLayout->addWidget(addendFunctionComboBox, 2, 1);

	gridLayout->addWidget(stripeDensityLabel, 3, 0);
	gridLayout->addWidget(stripeDensitySpinBox, 3, 1);

	gridLayout->addWidget(interpolationMethodLabel, 4, 0);
	gridLayout->addWidget(interpolationMethodComboBox, 4, 1);

	gridLayout->addWidget(transferFunctionLabel, 5, 0);
	gridLayout->addWidget(transferFunctionComboBox, 5, 1);

	gridLayout->addWidget(colorScalingLabel, 6, 0);
	gridLayout->addWidget(colorScalingSpinBox, 6, 1);

	gridLayout->addWidget(colorOffsetLabel, 7, 0);
	gridLayout->addWidget(colorOffsetSpinBox, 7, 1);

	gridLayout->addWidget(spaceColorLabel, 8, 0);
	gridLayout->addWidget(spaceColorButton, 8, 1);

	gridLayout->setColumnStretch(1, 1);
	gridLayout->setRowStretch(9, 1);
	this->setLayout(gridLayout);

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

