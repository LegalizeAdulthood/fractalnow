/*
 *  fractal_config_widget.cpp -- part of fractal2D
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
 *  Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1DEFAULT_DECIMAL_NUMBER1, USA.
 */

#include "fractal_config_widget.h"
#include "floating_point.h"
#include "fractal_formula.h"
#include <limits>
#include <QLabel>
#include <QFormLayout>
#include "main.h"

FractalConfigWidget::FractalConfigWidget(Fractal &fractal) : QWidget()
{
	fractalFormulaComboBox = new QComboBox;
	for (uint_fast32_t i = 0; i < nbFractalFormulas; ++i) {
		fractalFormulaComboBox->addItem(fractalFormulaDescStr[i]);
	}

	pParamSpinBox = new QDoubleSpinBox;
	pParamSpinBox->setDecimals(DEFAULT_DECIMAL_NUMBER);
	pParamSpinBox->setRange(1, std::numeric_limits<double>::max());
	pParamSpinBox->setAccelerated(true);

	cParamReSpinBox = new QDoubleSpinBox;
	cParamReSpinBox->setDecimals(DEFAULT_DECIMAL_NUMBER);
	cParamReSpinBox->setRange(-std::numeric_limits<double>::max(), std::numeric_limits<double>::max());
	cParamReSpinBox->setAccelerated(true);

	cParamImSpinBox = new QDoubleSpinBox;
	cParamImSpinBox->setDecimals(DEFAULT_DECIMAL_NUMBER);
	cParamImSpinBox->setRange(-std::numeric_limits<double>::max(), std::numeric_limits<double>::max());
	cParamImSpinBox->setAccelerated(true);

	centerXSpinBox = new QDoubleSpinBox;
	centerXSpinBox->setDecimals(DEFAULT_DECIMAL_NUMBER);
	centerXSpinBox->setRange(-std::numeric_limits<double>::max(), std::numeric_limits<double>::max());
	centerXSpinBox->setAccelerated(true);

	centerYSpinBox = new QDoubleSpinBox;
	centerYSpinBox->setDecimals(DEFAULT_DECIMAL_NUMBER);
	centerYSpinBox->setRange(-std::numeric_limits<double>::max(), std::numeric_limits<double>::max());
	centerYSpinBox->setAccelerated(true);

	spanXSpinBox = new QDoubleSpinBox;
	spanXSpinBox->setDecimals(DEFAULT_DECIMAL_NUMBER);
	spanXSpinBox->setRange(0, std::numeric_limits<double>::max());
	spanXSpinBox->setAccelerated(true);

	bailoutRadiusSpinBox = new QDoubleSpinBox;
	bailoutRadiusSpinBox->setDecimals(0);
	bailoutRadiusSpinBox->setRange(1, std::numeric_limits<double>::max());
	bailoutRadiusSpinBox->setAccelerated(true);

	maxIterationsSpinBox = new QSpinBox;
	maxIterationsSpinBox->setRange(1, std::numeric_limits<int>::max());
	maxIterationsSpinBox->setAccelerated(true);

	QFormLayout *formLayout = new QFormLayout;
	formLayout->addRow(tr("Fractal formula:"), fractalFormulaComboBox);
	formLayout->addRow(tr("p:"), pParamSpinBox);
	formLayout->addRow(tr("c (Re):"), cParamReSpinBox);
	formLayout->addRow(tr("c (Im):"), cParamImSpinBox);
	formLayout->addRow(tr("Center X:"), centerXSpinBox);
	formLayout->addRow(tr("Center Y:"), centerYSpinBox);
	formLayout->addRow(tr("Span X:"), spanXSpinBox);
	formLayout->addRow(tr("Bailout radius:"), bailoutRadiusSpinBox);
	formLayout->addRow(tr("Max iterations:"), maxIterationsSpinBox);
	this->setLayout(formLayout);

	updateBoxesValues(fractal);
	updateSpaceBoxesSingleSteps();
	updateCParamReSingleStep();
	updateCParamImSingleStep();
	updateBoxesEnabledValue();

	connect(spanXSpinBox, SIGNAL(valueChanged(double)), this, SLOT(updateSpaceBoxesSingleSteps()));
	connect(cParamReSpinBox, SIGNAL(valueChanged(double)), this, SLOT(updateCParamReSingleStep()));
	connect(cParamImSpinBox, SIGNAL(valueChanged(double)), this, SLOT(updateCParamImSingleStep()));
	connect(fractalFormulaComboBox, SIGNAL(currentIndexChanged(int)),
		this, SLOT(updateBoxesEnabledValue()));
}

void FractalConfigWidget::blockBoxesSignals(bool block)
{
	fractalFormulaComboBox->blockSignals(block);
	pParamSpinBox->blockSignals(block);
	cParamReSpinBox->blockSignals(block);
	cParamImSpinBox->blockSignals(block);
	centerXSpinBox->blockSignals(block);
	centerYSpinBox->blockSignals(block);
	spanXSpinBox->blockSignals(block);
	bailoutRadiusSpinBox->blockSignals(block);
	maxIterationsSpinBox->blockSignals(block);
}

void FractalConfigWidget::updateBoxesValues(Fractal &fractal)
{
	blockBoxesSignals(true);
	fractalFormulaComboBox->setCurrentIndex((int)fractal.fractalFormula);
	pParamSpinBox->setValue(fractal.p);
	cParamReSpinBox->setValue(crealF(fractal.c));
	cParamImSpinBox->setValue(cimagF(fractal.c));
	centerXSpinBox->setValue(fractal.centerX);
	centerYSpinBox->setValue(fractal.centerY);
	spanXSpinBox->setValue(fractal.spanX);
	bailoutRadiusSpinBox->setValue(fractal.escapeRadius);
	maxIterationsSpinBox->setValue(fractal.maxIter);
	updateSpaceBoxesSingleSteps();
	updateCParamReSingleStep();
	updateCParamImSingleStep();
	blockBoxesSignals(false);
}

void FractalConfigWidget::updateSpaceBoxesSingleSteps()
{
	double spanX = spanXSpinBox->value();
	if (spanX <= 0) {
		centerXSpinBox->setSingleStep(MIN_SINGLE_STEP);
		centerYSpinBox->setSingleStep(MIN_SINGLE_STEP);
		spanXSpinBox->setSingleStep(MIN_SINGLE_STEP);
	} else {
		centerXSpinBox->setSingleStep(spanX / 5);
		centerYSpinBox->setSingleStep(spanX / 5);
		spanXSpinBox->setSingleStep(spanX * 0.3);
	}
}

void FractalConfigWidget::updateCParamReSingleStep()
{
	double cParamRe = cParamReSpinBox->value();
	if (cParamRe <= 0) {
		cParamReSpinBox->setSingleStep(MIN_SINGLE_STEP);
	} else {
		cParamReSpinBox->setSingleStep(cParamRe / 5);
	}
}

void FractalConfigWidget::updateCParamImSingleStep()
{
	double cParamIm = cParamImSpinBox->value();
	if (cParamIm <= 0) {
		cParamImSpinBox->setSingleStep(MIN_SINGLE_STEP);
	} else {
		cParamImSpinBox->setSingleStep(cParamIm / 5);
	}
}

void FractalConfigWidget::updateBoxesEnabledValue()
{
	FractalFormula formula = (FractalFormula)fractalFormulaComboBox->currentIndex();

	switch (formula) {
	case FRAC_MANDELBROT:
		pParamSpinBox->setEnabled(false);
		cParamReSpinBox->setEnabled(false);
		cParamImSpinBox->setEnabled(false);
		break;
	case FRAC_MANDELBROTP:
		pParamSpinBox->setEnabled(true);
		cParamReSpinBox->setEnabled(false);
		cParamImSpinBox->setEnabled(false);
		break;
	case FRAC_JULIA:
		pParamSpinBox->setEnabled(false);
		cParamReSpinBox->setEnabled(true);
		cParamImSpinBox->setEnabled(true);
		break;
	case FRAC_JULIAP:
	case FRAC_RUDY:
		pParamSpinBox->setEnabled(true);
		cParamReSpinBox->setEnabled(true);
		cParamImSpinBox->setEnabled(true);
		break;
	default:
		pParamSpinBox->setEnabled(true);
		cParamReSpinBox->setEnabled(true);
		cParamImSpinBox->setEnabled(true);
		break;
	}
}

