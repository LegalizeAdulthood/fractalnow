/*
 *  fractal_config_widget.cpp -- part of FractalNow
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
#include "fractal_config_widget.h"

#include <limits>

#include <QLabel>
#include <QFormLayout>

FractalConfigWidget::FractalConfigWidget(const Fractal &fractal) : QWidget()
{
	fractalFormulaComboBox = new QComboBox;
	for (uint_fast32_t i = 0; i < nbFractalFormulas; ++i) {
		fractalFormulaComboBox->addItem(fractalFormulaDescStr[i]);
	}

	pParamReSpinBox = new MPFRSpinBox;
	pParamReSpinBox->setDecimals(DEFAULT_DECIMALS_NUMBER);
	pParamReSpinBox->setRange(0, 100);
	pParamReSpinBox->setAccelerated(true);

	pParamImSpinBox = new MPFRSpinBox;
	pParamImSpinBox->setDecimals(DEFAULT_DECIMALS_NUMBER);
	pParamImSpinBox->setRange(0, 100);
	pParamImSpinBox->setAccelerated(true);

	cParamReSpinBox = new MPFRSpinBox;
	cParamReSpinBox->setDecimals(DEFAULT_DECIMALS_NUMBER);
	cParamReSpinBox->setRange(-100, 100);
	cParamReSpinBox->setAccelerated(true);

	cParamImSpinBox = new MPFRSpinBox;
	cParamImSpinBox->setDecimals(DEFAULT_DECIMALS_NUMBER);
	cParamImSpinBox->setRange(-100, 100);
	cParamImSpinBox->setAccelerated(true);

	centerXSpinBox = new MPFRSpinBox;
	centerXSpinBox->setRange(-100, 100);
	centerXSpinBox->setAccelerated(true);
	centerXSpinBox->setNotation(MPFRSpinBox::ScientificNotation);

	centerYSpinBox = new MPFRSpinBox;
	centerYSpinBox->setRange(-100, 100);
	centerYSpinBox->setAccelerated(true);
	centerYSpinBox->setNotation(MPFRSpinBox::ScientificNotation);

	spanXSpinBox = new MPFRSpinBox;
	spanXSpinBox->setRange(0, std::numeric_limits<double>::max());
	spanXSpinBox->setAccelerated(true);
	spanXSpinBox->setNotation(MPFRSpinBox::ScientificNotation);

	bailoutRadiusSpinBox = new QDoubleSpinBox;
	bailoutRadiusSpinBox->setDecimals(0);
	bailoutRadiusSpinBox->setRange(1, std::numeric_limits<double>::max());
	bailoutRadiusSpinBox->setAccelerated(true);

	maxIterationsSpinBox = new QSpinBox;
	maxIterationsSpinBox->setRange(1, std::numeric_limits<int>::max());
	maxIterationsSpinBox->setAccelerated(true);

	QFormLayout *formLayout = new QFormLayout;
	formLayout->addRow(tr("Fractal formula:"), fractalFormulaComboBox);
	formLayout->addRow(tr("p (Re):"), pParamReSpinBox);
	formLayout->addRow(tr("p (Im):"), pParamImSpinBox);
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

	connect(spanXSpinBox, SIGNAL(valueChanged(const mpfr_t *)), this, SLOT(updateSpaceBoxesSingleSteps()));
	connect(cParamReSpinBox, SIGNAL(valueChanged(const mpfr_t *)), this, SLOT(updateCParamReSingleStep()));
	connect(cParamImSpinBox, SIGNAL(valueChanged(const mpfr_t *)), this, SLOT(updateCParamImSingleStep()));
	connect(fractalFormulaComboBox, SIGNAL(currentIndexChanged(int)),
		this, SLOT(updateBoxesEnabledValue()));
}

void FractalConfigWidget::blockBoxesSignals(bool block)
{
	fractalFormulaComboBox->blockSignals(block);
	pParamReSpinBox->blockSignals(block);
	pParamImSpinBox->blockSignals(block);
	cParamReSpinBox->blockSignals(block);
	cParamImSpinBox->blockSignals(block);
	centerXSpinBox->blockSignals(block);
	centerYSpinBox->blockSignals(block);
	spanXSpinBox->blockSignals(block);
	bailoutRadiusSpinBox->blockSignals(block);
	maxIterationsSpinBox->blockSignals(block);
}

void FractalConfigWidget::updateBoxesValues(const Fractal &fractal)
{
	blockBoxesSignals(true);
	fractalFormulaComboBox->setCurrentIndex((int)fractal.fractalFormula);
	pParamReSpinBox->setValue(&mpc_realref(fractal.p));
	pParamImSpinBox->setValue(&mpc_imagref(fractal.p));
	cParamReSpinBox->setValue(&mpc_realref(fractal.c));
	cParamImSpinBox->setValue(&mpc_imagref(fractal.c));
	centerXSpinBox->setValue(&fractal.centerX);
	centerYSpinBox->setValue(&fractal.centerY);
	spanXSpinBox->setValue(&fractal.spanX);
	bailoutRadiusSpinBox->setValue(fractal.escapeRadius);
	maxIterationsSpinBox->setValue(fractal.maxIter);
	updateBoxesEnabledValue();
	updateSpaceBoxesSingleSteps();
	updateCParamReSingleStep();
	updateCParamImSingleStep();
	blockBoxesSignals(false);
}

void FractalConfigWidget::updateSpaceBoxesSingleSteps()
{
	const mpfr_t * spanX = spanXSpinBox->value();
	if (mpfr_cmp_ui(*spanX, 0) <= 0) {
		centerXSpinBox->setSingleStep(MIN_SINGLE_STEP);
		centerYSpinBox->setSingleStep(MIN_SINGLE_STEP);
		spanXSpinBox->setSingleStep(MIN_SINGLE_STEP);
	} else {
		mpfr_t step;
		mpfr_init(step);

		mpfr_div_ui(step, *spanX, 5, MPFR_RNDN);
		centerXSpinBox->setSingleStep(&step);
		centerYSpinBox->setSingleStep(&step);
		mpfr_mul_d(step, *spanX, 0.3, MPFR_RNDN);
		spanXSpinBox->setSingleStep(&step);
		mpfr_clear(step);
	}
}

void FractalConfigWidget::updateCParamReSingleStep()
{
	const mpfr_t *cParamRe = cParamReSpinBox->value();
	if (mpfr_cmp_ui(*cParamRe, 0) <= 0) {
		cParamReSpinBox->setSingleStep(MIN_SINGLE_STEP);
	} else {
		mpfr_t step;
		mpfr_init(step);
		mpfr_div_ui(step, *cParamRe, 5, MPFR_RNDN);
		cParamReSpinBox->setSingleStep(&step);
		mpfr_clear(step);
	}
}

void FractalConfigWidget::updateCParamImSingleStep()
{
	const mpfr_t *cParamIm = cParamImSpinBox->value();
	if (mpfr_cmp_ui(*cParamIm, 0) <= 0) {
		cParamImSpinBox->setSingleStep(MIN_SINGLE_STEP);
	} else {
		mpfr_t step;
		mpfr_init(step);
		mpfr_div_ui(step, *cParamIm, 5, MPFR_RNDN);
		cParamImSpinBox->setSingleStep(&step);
		mpfr_clear(step);
	}
}

void FractalConfigWidget::updateBoxesEnabledValue()
{
	FractalFormula formula = (FractalFormula)fractalFormulaComboBox->currentIndex();

	switch (formula) {
	case FRAC_MANDELBROT:
		pParamReSpinBox->setEnabled(false);
		pParamImSpinBox->setEnabled(false);
		cParamReSpinBox->setEnabled(false);
		cParamImSpinBox->setEnabled(false);
		break;
	case FRAC_MULTIBROT:
	case FRAC_BURNINGSHIP:
	case FRAC_MANDELBAR:
		pParamReSpinBox->setEnabled(true);
		pParamImSpinBox->setEnabled(true);
		cParamReSpinBox->setEnabled(false);
		cParamImSpinBox->setEnabled(false);
		break;
	case FRAC_JULIA:
		pParamReSpinBox->setEnabled(false);
		pParamImSpinBox->setEnabled(false);
		cParamReSpinBox->setEnabled(true);
		cParamImSpinBox->setEnabled(true);
		break;
	case FRAC_MULTIJULIA:
	case FRAC_JULIABURNINGSHIP:
	case FRAC_JULIABAR:
	case FRAC_RUDY:
		pParamReSpinBox->setEnabled(true);
		pParamImSpinBox->setEnabled(true);
		cParamReSpinBox->setEnabled(true);
		cParamImSpinBox->setEnabled(true);
		break;
	default:
		pParamReSpinBox->setEnabled(true);
		pParamImSpinBox->setEnabled(true);
		cParamReSpinBox->setEnabled(true);
		cParamImSpinBox->setEnabled(true);
		break;
	}
}

