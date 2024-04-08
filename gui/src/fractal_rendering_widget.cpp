/*
 *  fractal_rendering_widget.cpp -- part of FractalNow
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

#include <stdint.h>
#include <limits>

#include <QFormLayout>
#include <QLabel>

FractalRenderingWidget::FractalRenderingWidget(const RenderingParameters &render) : QWidget()
{
	coloringMethodComboBox = new QComboBox;
	for (uint_fast32_t i = 0; i < nbColoringMethods; ++i) {
		coloringMethodComboBox->addItem(coloringMethodDescStr[i]);
	}
	coloringMethodComboBox->setCurrentIndex((int)render.coloringMethod);

	iterationCountComboBox = new QComboBox;
	for (uint_fast32_t i = 0; i < nbIterationCounts; ++i) {
		iterationCountComboBox->addItem(iterationCountDescStr[i]);
	}
	iterationCountComboBox->setCurrentIndex((int)render.iterationCount);

	addendFunctionComboBox = new QComboBox;
	for (uint_fast32_t i = 0; i < nbAddendFunctions; ++i) {
		addendFunctionComboBox->addItem(addendFunctionDescStr[i]);
	}
	addendFunctionComboBox->setCurrentIndex((int)render.addendFunction);

	stripeDensitySpinBox = new QSpinBox;
	stripeDensitySpinBox->setRange(1, std::numeric_limits<int>::max());
	stripeDensitySpinBox->setValue((int)render.stripeDensity);
	stripeDensitySpinBox->setAccelerated(true);

	interpolationMethodComboBox = new QComboBox;
	for (uint_fast32_t i = 0; i < nbInterpolationMethods; ++i) {
		interpolationMethodComboBox->addItem(interpolationMethodDescStr[i]);
	}
	interpolationMethodComboBox->setCurrentIndex((int)render.interpolationMethod);

	transferFunctionComboBox = new QComboBox;
	for (uint_fast32_t i = 0; i < nbTransferFunctions; ++i) {
		transferFunctionComboBox->addItem(transferFunctionDescStr[i]);
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

	gradientBox = new GradientBox(render.gradient);

	QFormLayout *formLayout = new QFormLayout;
	formLayout->addRow(tr("Coloring method:"), coloringMethodComboBox);
	formLayout->addRow(tr("Iteration count:"), iterationCountComboBox);
	formLayout->addRow(tr("Addend function:"), addendFunctionComboBox);
	formLayout->addRow(tr("Stripe density:"), stripeDensitySpinBox);
	formLayout->addRow(tr("Interpolation method:"), interpolationMethodComboBox);
	formLayout->addRow(tr("Transfer function:"), transferFunctionComboBox);
	formLayout->addRow(tr("Color scaling:"), colorScalingSpinBox);
	formLayout->addRow(tr("Color offset:"), colorOffsetSpinBox);
	formLayout->addRow(tr("Space color:"), spaceColorButton);

	QVBoxLayout *vBoxLayout = new QVBoxLayout;
	vBoxLayout->addLayout(formLayout);
	vBoxLayout->addWidget(new QLabel(tr("Gradient:")));
	vBoxLayout->addWidget(gradientBox);
	vBoxLayout->addStretch(1);
	this->setLayout(vBoxLayout);

	updateBoxesEnabledValue();
	updateColorScalingSingleStep();

	connect(coloringMethodComboBox, SIGNAL(currentIndexChanged(int)),
		this, SLOT(updateBoxesEnabledValue()));
	connect(addendFunctionComboBox, SIGNAL(currentIndexChanged(int)),
		this, SLOT(updateBoxesEnabledValue()));
	connect(colorScalingSpinBox, SIGNAL(valueChanged(double)), this, SLOT(updateColorScalingSingleStep()));
}

void FractalRenderingWidget::blockBoxesSignals(bool block)
{
	iterationCountComboBox->blockSignals(block);
	coloringMethodComboBox->blockSignals(block);
	addendFunctionComboBox->blockSignals(block);
	stripeDensitySpinBox->blockSignals(block);
	interpolationMethodComboBox->blockSignals(block);
	transferFunctionComboBox->blockSignals(block);
	colorScalingSpinBox->blockSignals(block);
	colorOffsetSpinBox->blockSignals(block);
	spaceColorButton->blockSignals(block);
	gradientBox->blockSignals(block);
}

void FractalRenderingWidget::updateBoxesValues(const RenderingParameters &render)
{
	blockBoxesSignals(true);
	iterationCountComboBox->setCurrentIndex((int)render.iterationCount);
	coloringMethodComboBox->setCurrentIndex((int)render.coloringMethod);
	addendFunctionComboBox->setCurrentIndex((int)render.addendFunction);
	stripeDensitySpinBox->setValue(render.stripeDensity);
	interpolationMethodComboBox->setCurrentIndex((int)render.interpolationMethod);
	transferFunctionComboBox->setCurrentIndex((int)render.transferFunction);
	colorScalingSpinBox->setValue(render.multiplier);
	colorOffsetSpinBox->setValue(render.offset);
	spaceColorButton->setCurrentColor(QColor(render.spaceColor.r,
					render.spaceColor.g, render.spaceColor.b));
	gradientBox->setGradient(render.gradient);
	updateColorScalingSingleStep();
	updateBoxesEnabledValue();
	blockBoxesSignals(false);
}

void FractalRenderingWidget::updateBoxesEnabledValue()
{
	ColoringMethod coloringMethod = (ColoringMethod)coloringMethodComboBox->currentIndex();
	AddendFunction addendFunction = (AddendFunction)addendFunctionComboBox->currentIndex();

	switch (coloringMethod) {
	case CM_ITERATIONCOUNT:
		iterationCountComboBox->setEnabled(true);
		addendFunctionComboBox->setEnabled(false);
		stripeDensitySpinBox->setEnabled(false);
		interpolationMethodComboBox->setEnabled(false);
		break;
	case CM_AVERAGECOLORING:
		iterationCountComboBox->setEnabled(false);
		addendFunctionComboBox->setEnabled(true);
		stripeDensitySpinBox->setEnabled(addendFunction == AF_STRIPE);
		interpolationMethodComboBox->setEnabled(true);
		break;
	default:
		iterationCountComboBox->setEnabled(true);
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

void FractalRenderingWidget::editGradient()
{
	gradientBox->openGradientDialog();

}
