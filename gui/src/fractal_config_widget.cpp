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
#include "main_window.h"
#include <limits>
#include <QLabel>
#include <QVBoxLayout>

FractalConfigWidget::FractalConfigWidget(Fractal &fractal) : QWidget()
{
	QLabel *fractalFormulaLabel = new QLabel(tr("Fractal formula:"));
	fractalFormulaComboBox = new QComboBox;
	for (uint_fast32_t i = 0; i < nbFractalFormulas; ++i) {
		fractalFormulaComboBox->addItem(FractalFormulaDescStr[i]);
	}

	QLabel *pParamLabel = new QLabel(tr("p:"));
	pParamSpinBox = new QDoubleSpinBox;
	pParamSpinBox->setDecimals(DEFAULT_DECIMAL_NUMBER);
	pParamSpinBox->setRange(1, std::numeric_limits<double>::max());
	pParamSpinBox->setAccelerated(true);

	QLabel *cParamReLabel = new QLabel(tr("c (Re):"));
	cParamReSpinBox = new QDoubleSpinBox;
	cParamReSpinBox->setDecimals(DEFAULT_DECIMAL_NUMBER);
	cParamReSpinBox->setRange(-std::numeric_limits<double>::max(), std::numeric_limits<double>::max());
	cParamReSpinBox->setAccelerated(true);

	QLabel *cParamImLabel = new QLabel(tr("c (Im):"));
	cParamImSpinBox = new QDoubleSpinBox;
	cParamImSpinBox->setDecimals(DEFAULT_DECIMAL_NUMBER);
	cParamImSpinBox->setRange(-std::numeric_limits<double>::max(), std::numeric_limits<double>::max());
	cParamImSpinBox->setAccelerated(true);

	QLabel *centerXLabel = new QLabel(tr("Center X:"));
	centerXSpinBox = new QDoubleSpinBox;
	centerXSpinBox->setDecimals(DEFAULT_DECIMAL_NUMBER);
	centerXSpinBox->setRange(-std::numeric_limits<double>::max(), std::numeric_limits<double>::max());
	centerXSpinBox->setAccelerated(true);

	QLabel *centerYLabel = new QLabel(tr("Center Y:"));
	centerYSpinBox = new QDoubleSpinBox;
	centerYSpinBox->setDecimals(DEFAULT_DECIMAL_NUMBER);
	centerYSpinBox->setRange(-std::numeric_limits<double>::max(), std::numeric_limits<double>::max());
	centerYSpinBox->setAccelerated(true);

	QLabel *spanLabel = new QLabel(tr("Span:"));
	spanXSpinBox = new QDoubleSpinBox;
	spanXSpinBox->setDecimals(DEFAULT_DECIMAL_NUMBER);
	spanXSpinBox->setRange(0, std::numeric_limits<double>::max());
	spanXSpinBox->setAccelerated(true);

	QLabel *bailoutRadiusLabel = new QLabel(tr("Bailout radius:"));
	bailoutRadiusSpinBox = new QDoubleSpinBox;
	bailoutRadiusSpinBox->setDecimals(0);
	bailoutRadiusSpinBox->setRange(1, std::numeric_limits<double>::max());
	bailoutRadiusSpinBox->setAccelerated(true);

	QLabel *maxIterationsLabel = new QLabel(tr("Max iterations:"));
	maxIterationsSpinBox = new QSpinBox;
	maxIterationsSpinBox->setRange(1, std::numeric_limits<int>::max());
	maxIterationsSpinBox->setAccelerated(true);

	QGridLayout *gridLayout = new QGridLayout;
	gridLayout->addWidget(fractalFormulaLabel, 0, 0);
	gridLayout->addWidget(fractalFormulaComboBox, 0, 1);
	gridLayout->addWidget(pParamLabel, 1, 0);
	gridLayout->addWidget(pParamSpinBox, 1, 1);
	gridLayout->addWidget(cParamReLabel, 2, 0);
	gridLayout->addWidget(cParamReSpinBox, 2, 1);
	gridLayout->addWidget(cParamImLabel, 3, 0);
	gridLayout->addWidget(cParamImSpinBox, 3, 1);
	gridLayout->addWidget(centerXLabel, 4, 0);
	gridLayout->addWidget(centerXSpinBox, 4, 1);
	gridLayout->addWidget(centerYLabel, 5, 0);
	gridLayout->addWidget(centerYSpinBox, 5, 1);
	gridLayout->addWidget(spanLabel, 6, 0);
	gridLayout->addWidget(spanXSpinBox, 6, 1);
	gridLayout->addWidget(bailoutRadiusLabel, 7, 0);
	gridLayout->addWidget(bailoutRadiusSpinBox, 7, 1);
	gridLayout->addWidget(maxIterationsLabel, 8, 0);
	gridLayout->addWidget(maxIterationsSpinBox, 8, 1);
	gridLayout->setColumnStretch(1, 1);
	gridLayout->setRowStretch(9, 1);
	this->setLayout(gridLayout);

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
	fractalFormulaComboBox->setCurrentIndex((int)fractal.fractalFormula);
	pParamSpinBox->setValue(fractal.p);
	cParamReSpinBox->setValue(crealF(fractal.c));
	cParamImSpinBox->setValue(cimagF(fractal.c));
	centerXSpinBox->setValue(fractal.centerX);
	centerYSpinBox->setValue(fractal.centerY);
	spanXSpinBox->setValue(fractal.spanX);
	bailoutRadiusSpinBox->setValue(fractal.escapeRadius);
	maxIterationsSpinBox->setValue(fractal.maxIter);
}

void FractalConfigWidget::updateSpaceBoxesSingleSteps()
{
	double spanX = spanXSpinBox->value();
	centerXSpinBox->setSingleStep(spanX / 10);
	centerYSpinBox->setSingleStep(spanX / 10);
	spanXSpinBox->setSingleStep(spanX / 3);
}

void FractalConfigWidget::updateCParamReSingleStep()
{
	double cParamRe = cParamReSpinBox->value();
	cParamReSpinBox->setSingleStep(cParamRe / 10);
}

void FractalConfigWidget::updateCParamImSingleStep()
{
	double cParamIm = cParamImSpinBox->value();
	cParamImSpinBox->setSingleStep(cParamIm / 10);
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

