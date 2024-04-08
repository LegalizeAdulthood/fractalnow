/*
 *  gradient_dialog.cpp -- part of fractal2D
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

#include "gradient_dialog.h"

GradientDialog::GradientDialog(const QGradientStops &gradientStops, QWidget *parent,
				Qt::WindowFlags f) : QDialog(parent,f)
{
	this->gradientStops = gradientStops;

	gradientEditor = new GradientEditor;
	gradientEditor->blockSignals(true);
	connect(gradientEditor, SIGNAL(gradientStopsChanged(const QGradientStops&)),
		this, SLOT(onGradientStopsChanged(const QGradientStops&)));

	QDialogButtonBox *dialogButtonBox = new QDialogButtonBox(this);
	QPushButton *cancelButton = dialogButtonBox->addButton(QDialogButtonBox::Cancel);
	connect(cancelButton, SIGNAL(clicked()), this, SLOT(reject()));
	QPushButton *OkButton = dialogButtonBox->addButton(QDialogButtonBox::Ok);
	connect(OkButton, SIGNAL(clicked()), this, SLOT(accept()));
	
	QVBoxLayout *vBoxLayout = new QVBoxLayout;
	vBoxLayout->addWidget(gradientEditor);
	vBoxLayout->addWidget(dialogButtonBox);

	setLayout(vBoxLayout);

	QTimer::singleShot(50, this, SLOT(initGradientEditor()));
}

QGradientStops GradientDialog::currentGradientStops()
{
	return gradientStops;
}

void GradientDialog::onGradientStopsChanged(const QGradientStops &gradientStops)
{
	this->gradientStops = gradientStops;
}

void GradientDialog::initGradientEditor()
{
	gradientEditor->setGradientStops(gradientStops);
	gradientEditor->blockSignals(false);
}

