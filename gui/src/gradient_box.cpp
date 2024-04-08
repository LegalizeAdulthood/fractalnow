/*
 *  gradient_box.cpp -- part of FractalNow
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

#include "gradient_box.h"
#include "gradient_dialog.h"
#include "gradient_label.h"
#include <QHBoxLayout>
#include <QToolButton>

static QGradientStops getGradientStops(const Gradient &gradient)
{
	uint_fast32_t nbStops = gradient.nbStops;
	Color c;
	QGradientStops gradientStops;
	for (uint_fast32_t i = 0; i < nbStops; ++i) {
		c = gradient.colorStop[i];
		gradientStops << QGradientStop(gradient.positionStop[i], QColor(c.r, c.g, c.b));
	}
	return gradientStops;
}

void GradientBox::initGradientBox(const QGradientStops &gradientStops)
{
	this->gradientStops = gradientStops;

	QHBoxLayout *hBoxLayout = new QHBoxLayout;
	QToolButton *editGradientButton = new QToolButton();
	editGradientButton->setText(tr("..."));
	connect(editGradientButton, SIGNAL(clicked()), this, SLOT(openGradientDialog()));

	gradientLabel = new GradientLabel(this);
	gradientLabel->setGradientStops(gradientStops);

	hBoxLayout->addWidget(gradientLabel);
	hBoxLayout->addWidget(editGradientButton);
	hBoxLayout->setSizeConstraint(QLayout::SetNoConstraint);
	hBoxLayout->setStretch(0, 1);

	this->setLayout(hBoxLayout);
}

GradientBox::GradientBox(const QGradientStops &gradientStops, QWidget *parent) : QWidget(parent)
{
	initGradientBox(gradientStops);
}

GradientBox::GradientBox(const Gradient &gradient, QWidget *parent) : QWidget(parent)
{
	QGradientStops gradientStops = getGradientStops(gradient);
	initGradientBox(gradientStops);
}

void GradientBox::setGradientStops(const QGradientStops &gradientStops)
{
	this->gradientStops = gradientStops;
	gradientLabel->setGradientStops(gradientStops);
}

void GradientBox::setGradient(const Gradient &gradient)
{
	QGradientStops gradientStops(getGradientStops(gradient));
	setGradientStops(gradientStops);
}

void GradientBox::openGradientDialog()
{
	GradientDialog dialog(gradientStops, this);
	if (dialog.exec() == QDialog::Accepted) {
		QGradientStops newGradientStops(dialog.currentGradientStops());
		if (newGradientStops != gradientStops) {
			gradientStops = newGradientStops;
			gradientLabel->setGradientStops(gradientStops);
			emit gradientStopsChanged(gradientStops);
		}
	}
}

