/****************************************************************************
**
** This file is based on some files of the demonstration applications of the
** Qt Toolkit.
**
** Copyright (C) 2011 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** Modified to be used as a part of FractalNow:
** Copyright (c) 2012 Marc Pegon <pe.marc@free.fr>
**
** This file can be distributed and/or modified under the terms of the GNU
** General Public License version 3.0 as published by the Free Software
** Foundation.
** You should have received a copy of the GNU General Public License
** along with this file; if not, write to the Free Software Foundation, Inc.,
** 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.
**
****************************************************************************/

#include "gradient_editor.h"
#include <QVBoxLayout>

GradientEditor::GradientEditor(QWidget *parent) : QWidget(parent)
{
	QVBoxLayout *vbox = new QVBoxLayout;
	vbox->setSpacing(1);
	vbox->setMargin(1);

	m_gradient_label = new GradientLabel;
	m_red_shade = new ShadeWidget(ShadeWidget::RedShade);
	m_green_shade = new ShadeWidget(ShadeWidget::GreenShade);
	m_blue_shade = new ShadeWidget(ShadeWidget::BlueShade);

	vbox->addWidget(m_gradient_label);
	vbox->addWidget(m_red_shade);
	vbox->addWidget(m_green_shade);
	vbox->addWidget(m_blue_shade);

	setLayout(vbox);

	connect(m_red_shade, SIGNAL(colorsChanged()), this, SLOT(pointsUpdated()));
	connect(m_green_shade, SIGNAL(colorsChanged()), this, SLOT(pointsUpdated()));
	connect(m_blue_shade, SIGNAL(colorsChanged()), this, SLOT(pointsUpdated()));
}


inline static bool x_less_than(const QPointF &p1, const QPointF &p2)
{
	return p1.x() < p2.x();
}


void GradientEditor::pointsUpdated()
{
	qreal w = m_red_shade->width();

	QGradientStops stops;

	QPolygonF points;

	points += m_red_shade->points();
	points += m_green_shade->points();
	points += m_blue_shade->points();

	qSort(points.begin(), points.end(), x_less_than);

	for (int i=0; i<points.size(); ++i) {
		qreal x = points.at(i).x();
		if (i>0 && int(x) == int(points.at(i-1).x())) {
			continue;
		}
		QColor color((0x00ff0000 & m_red_shade->colorAt(int(x))) >> 16,
		(0x0000ff00 & m_green_shade->colorAt(int(x))) >> 8,
		(0x000000ff & m_blue_shade->colorAt(int(x))));

		if (x / w > 1) {
			return;
		}

		stops << QGradientStop(x / w, color);
	}

	m_gradient_label->setGradientStops(stops);

	emit gradientStopsChanged(stops);
}


static void set_shade_points(const QPolygonF &points, ShadeWidget *shade)
{
	shade->hoverPoints()->setPoints(points);
	shade->hoverPoints()->setPointLock(0, HoverPoints::LockToLeft);
	shade->hoverPoints()->setPointLock(points.size() - 1, HoverPoints::LockToRight);
	shade->update();
}

void GradientEditor::setGradientStops(const QGradientStops &stops)
{
	QPolygonF pts_red, pts_green, pts_blue;

	m_gradient_label->setGradientStops(stops);
	qreal h_red = m_red_shade->height();
	qreal h_green = m_green_shade->height();
	qreal h_blue = m_blue_shade->height();

	for (int i=0; i<stops.size(); ++i) {
		qreal pos = stops.at(i).first;
		QRgb color = stops.at(i).second.rgba();
		pts_red << QPointF(pos * m_red_shade->width(), h_red - qRed(color) * h_red / 255);
		pts_green << QPointF(pos * m_green_shade->width(),
					h_green - qGreen(color) * h_green / 255);
		pts_blue << QPointF(pos * m_blue_shade->width(),
					h_blue - qBlue(color) * h_blue / 255);
	}

	set_shade_points(pts_red, m_red_shade);
	set_shade_points(pts_green, m_green_shade);
	set_shade_points(pts_blue, m_blue_shade);
}

