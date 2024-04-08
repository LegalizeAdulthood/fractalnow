/****************************************************************************
**
** Copyright (C) 2011 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the demonstration applications of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL$
** GNU Lesser General Public License Usage
** This file may be used under the terms of the GNU Lesser General Public
** License version 2.1 as published by the Free Software Foundation and
** appearing in the file LICENSE.LGPL included in the packaging of this
** file. Please review the following information to ensure the GNU Lesser
** General Public License version 2.1 requirements will be met:
** http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** In addition, as a special exception, Nokia gives you certain additional
** rights. These rights are described in the Nokia Qt LGPL Exception
** version 1.1, included in the file LGPL_EXCEPTION.txt in this package.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU General
** Public License version 3.0 as published by the Free Software Foundation
** and appearing in the file LICENSE.GPL included in the packaging of this
** file. Please review the following information to ensure the GNU General
** Public License version 3.0 requirements will be met:
** http://www.gnu.org/copyleft/gpl.html.
**
** Other Usage
** Alternatively, this file may be used in accordance with the terms and
** conditions contained in a signed written agreement between you and Nokia.
**
**
**
**
**
** $QT_END_LICENSE$
**
****************************************************************************/

#include "shade_widget.h"

ShadeWidget::ShadeWidget(ShadeType type, QWidget *parent)
    : QWidget(parent), m_shade_type(type), m_alpha_gradient(QLinearGradient())
{
	// Checkers background
	if (m_shade_type == ARGBShade) {
		QPixmap pm(20, 20);
		QPainter pmp(&pm);
		pmp.fillRect(0, 0, 10, 10, Qt::lightGray);
		pmp.fillRect(10, 10, 10, 10, Qt::lightGray);
		pmp.fillRect(0, 10, 10, 10, Qt::darkGray);
		pmp.fillRect(10, 0, 10, 10, Qt::darkGray);
		pmp.end();
		QPalette pal = palette();
		pal.setBrush(backgroundRole(), QBrush(pm));
		setAutoFillBackground(true);
		setPalette(pal);
	} else {
		setAttribute(Qt::WA_NoBackground);
	}

	QPolygonF points;
	points << QPointF(0, sizeHint().height())
	<< QPointF(sizeHint().width(), 0);

	m_hoverPoints = new HoverPoints(this, HoverPoints::CircleShape);
	//     m_hoverPoints->setConnectionType(HoverPoints::LineConnection);
	m_hoverPoints->setPoints(points);
	m_hoverPoints->setPointLock(0, HoverPoints::LockToLeft);
	m_hoverPoints->setPointLock(1, HoverPoints::LockToRight);
	m_hoverPoints->setSortType(HoverPoints::XSort);
	m_hoverPoints->setConnectionType(HoverPoints::LineConnection);

	setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred);

	connect(m_hoverPoints, SIGNAL(pointsChanged(QPolygonF)), this, SIGNAL(colorsChanged()));
}


QPolygonF ShadeWidget::points() const
{
	return m_hoverPoints->points();
}


unsigned int ShadeWidget::colorAt(int x)
{
	generateShade();

	QPolygonF pts = m_hoverPoints->points();
	for (int i=1; i < pts.size(); ++i) {
		if (pts.at(i-1).x() <= x && pts.at(i).x() >= x) {
			QLineF l(pts.at(i-1), pts.at(i));
			l.setLength(l.length() * ((x - l.x1()) / l.dx()));
			return m_shade.pixel(qRound(qMin(l.x2(), (qreal(m_shade.width() - 1)))),
					qRound(qMin(l.y2(), qreal(m_shade.height() - 1))));
		}
	}

	return 0;
}


void ShadeWidget::setGradientStops(const QGradientStops &stops)
{
	if (m_shade_type == ARGBShade) {
		m_alpha_gradient = QLinearGradient(0, 0, width(), 0);

		for (int i=0; i<stops.size(); ++i) {
			QColor c = stops.at(i).second;
			m_alpha_gradient.setColorAt(stops.at(i).first, QColor(c.red(),
							c.green(), c.blue()));
		}

		m_shade = QImage();
		generateShade();
		update();
	}
}


void ShadeWidget::paintEvent(QPaintEvent *)
{
	generateShade();

	QPainter p(this);
	p.drawImage(0, 0, m_shade);

	p.setPen(QColor(146, 146, 146));
	p.drawRect(0, 0, width() - 1, height() - 1);
}


void ShadeWidget::generateShade()
{
	if (m_shade.isNull() || m_shade.size() != size()) {
		if (m_shade_type == ARGBShade) {
			{
			m_shade = QImage(size(), QImage::Format_ARGB32);
			m_shade.fill(0);

			QPainter p(&m_shade);
			p.fillRect(rect(), m_alpha_gradient);

			p.setCompositionMode(QPainter::CompositionMode_DestinationIn);
			QLinearGradient fade(0, 0, 0, height());
			fade.setColorAt(0, QColor(0, 0, 0, 255));
			fade.setColorAt(1, QColor(0, 0, 0, 0));
			p.fillRect(rect(), fade);
			}
		} else {
			m_shade = QImage(size(), QImage::Format_RGB32);
			QLinearGradient shade(0, 0, 0, height());
			shade.setColorAt(1, Qt::black);

			if (m_shade_type == RedShade) {
				shade.setColorAt(0, Qt::red);
			} else if (m_shade_type == GreenShade) {
				shade.setColorAt(0, Qt::green);
			} else {
				shade.setColorAt(0, Qt::blue);
			}

			QPainter p(&m_shade);
			p.fillRect(rect(), shade);
		}
	}
}

