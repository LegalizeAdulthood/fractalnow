/*
 *  gradient_label.cpp -- part of FractalNow
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

#include "gradient_label.h"

#include <QPainter>

GradientLabel::GradientLabel(QWidget *parent) :
	QLabel(parent)
{
	gradientStops << QGradientStop(0, QColor::fromRgb(0x0));
	gradientStops << QGradientStop(1, QColor::fromRgb(0xFFFFFF));
}

void GradientLabel::setGradientStops(const QGradientStops &gradientStops)
{
	this->gradientStops = gradientStops;
	update();
}

void GradientLabel::paintEvent(QPaintEvent *event)
{
	Q_UNUSED(event);
	QPainter painter(this);
	QLinearGradient qGradient(rect().topLeft(), rect().topRight());
	qGradient.setStops(gradientStops);
	painter.setBrush(qGradient);
	painter.setPen(Qt::NoPen);
	painter.drawRect(rect());
}

