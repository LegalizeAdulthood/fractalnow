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

#ifndef __SHADE_WIDGET_H__
#define __SHADE_WIDGET_H__

#include "hoverpoints.h"

class ShadeWidget : public QWidget
{
	Q_OBJECT
	public:
	enum ShadeType {
		RedShade,
		GreenShade,
		BlueShade,
		ARGBShade
	};

	ShadeWidget(ShadeType type, QWidget *parent = 0);

	void setGradientStops(const QGradientStops &stops);

	void paintEvent(QPaintEvent *e);

	QSize sizeHint() const { return QSize(150, 40); }
	QPolygonF points() const;

	HoverPoints *hoverPoints() const { return m_hoverPoints; }

	uint colorAt(int x);

	signals:
	void colorsChanged();

	private:
	void generateShade();

	ShadeType m_shade_type;
	HoverPoints *m_hoverPoints;
	QLinearGradient m_alpha_gradient;
	QImage m_shade;
};

#endif

