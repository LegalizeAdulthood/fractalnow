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

