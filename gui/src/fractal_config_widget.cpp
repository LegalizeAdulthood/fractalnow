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
	QLabel *fractalFormulaLabel = new QLabel(QString("Fractal formula:"));
	fractalFormulaComboBox = new QComboBox;
	for (uint_fast32_t i = 0; i < nbFractalFormulas; ++i) {
		fractalFormulaComboBox->addItem(FractalFormulaDescStr[i]);
	}
	fractalFormulaComboBox->setCurrentIndex((int)fractal.fractalFormula);

	QLabel *pParamLabel = new QLabel(QString("p:"));
	pParamSpinBox = new QDoubleSpinBox;
	pParamSpinBox->setDecimals(DEFAULT_DECIMAL_NUMBER);
	pParamSpinBox->setRange(1, std::numeric_limits<double>::max());
	pParamSpinBox->setValue(fractal.p);
	pParamSpinBox->setAccelerated(true);

	QLabel *cParamReLabel = new QLabel(QString("c (Re):"));
	cParamReSpinBox = new QDoubleSpinBox;
	cParamReSpinBox->setDecimals(DEFAULT_DECIMAL_NUMBER);
	cParamReSpinBox->setRange(-std::numeric_limits<double>::max(), std::numeric_limits<double>::max());
	cParamReSpinBox->setValue(crealF(fractal.c));
	cParamReSpinBox->setAccelerated(true);

	QLabel *cParamImLabel = new QLabel(QString("c (Im):"));
	cParamImSpinBox = new QDoubleSpinBox;
	cParamImSpinBox->setDecimals(DEFAULT_DECIMAL_NUMBER);
	cParamImSpinBox->setRange(-std::numeric_limits<double>::max(), std::numeric_limits<double>::max());
	cParamImSpinBox->setValue(cimagF(fractal.c));
	cParamImSpinBox->setAccelerated(true);

	QLabel *centerXLabel = new QLabel(QString("Center X:"));
	centerXSpinBox = new QDoubleSpinBox;
	centerXSpinBox->setDecimals(DEFAULT_DECIMAL_NUMBER);
	centerXSpinBox->setRange(-std::numeric_limits<double>::max(), std::numeric_limits<double>::max());
	centerXSpinBox->setValue(fractal.centerX);
	centerXSpinBox->setAccelerated(true);

	QLabel *centerYLabel = new QLabel(QString("Center Y:"));
	centerYSpinBox = new QDoubleSpinBox;
	centerYSpinBox->setDecimals(DEFAULT_DECIMAL_NUMBER);
	centerYSpinBox->setRange(-std::numeric_limits<double>::max(), std::numeric_limits<double>::max());
	centerYSpinBox->setValue(fractal.centerY);
	centerYSpinBox->setAccelerated(true);

	QLabel *spanLabel = new QLabel(QString("Span:"));
	spanXSpinBox = new QDoubleSpinBox;
	spanXSpinBox->setDecimals(DEFAULT_DECIMAL_NUMBER);
	spanXSpinBox->setRange(0, std::numeric_limits<double>::max());
	spanXSpinBox->setValue(fractal.spanX);
	spanXSpinBox->setAccelerated(true);

	QLabel *bailoutRadiusLabel = new QLabel(QString("Bailout radius:"));
	bailoutRadiusSpinBox = new QDoubleSpinBox;
	bailoutRadiusSpinBox->setDecimals(0);
	bailoutRadiusSpinBox->setRange(1, std::numeric_limits<double>::max());
	bailoutRadiusSpinBox->setValue(fractal.escapeRadius);
	bailoutRadiusSpinBox->setAccelerated(true);

	QLabel *maxIterationsLabel = new QLabel(QString("Max iterations:"));
	maxIterationsSpinBox = new QSpinBox;
	maxIterationsSpinBox->setRange(1, std::numeric_limits<int>::max());
	maxIterationsSpinBox->setValue(fractal.maxIter);
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

	updateSpaceSingleSteps(fractal.spanX);
	updateCParamReSingleStep(crealF(fractal.c));
	updateCParamImSingleStep(cimagF(fractal.c));
	updateBoxesEnabledValue((int)fractal.fractalFormula);

	connect(spanXSpinBox, SIGNAL(valueChanged(double)), this, SLOT(updateSpaceSingleSteps(double)));
	connect(cParamReSpinBox, SIGNAL(valueChanged(double)), this, SLOT(updateCParamReSingleStep(double)));
	connect(cParamImSpinBox, SIGNAL(valueChanged(double)), this, SLOT(updateCParamImSingleStep(double)));
	connect(fractalFormulaComboBox, SIGNAL(currentIndexChanged(int)),
		this, SLOT(updateBoxesEnabledValue(int)));
}

void FractalConfigWidget::updateSpaceSingleSteps(double spanX)
{
	centerXSpinBox->setSingleStep(spanX / 10);
	centerYSpinBox->setSingleStep(spanX / 10);
	spanXSpinBox->setSingleStep(spanX / 10);
}

void FractalConfigWidget::updateCParamReSingleStep(double cParamRe)
{
	cParamReSpinBox->setSingleStep(cParamRe / 10);
}

void FractalConfigWidget::updateCParamImSingleStep(double cParamIm)
{
	cParamImSpinBox->setSingleStep(cParamIm / 10);
}

void FractalConfigWidget::updateBoxesEnabledValue(int fractalFormula)
{
	FractalFormula formula = (FractalFormula)fractalFormula;

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

