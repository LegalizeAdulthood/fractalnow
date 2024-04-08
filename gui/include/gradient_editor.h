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

#ifndef __GRADIENT_EDITOR_H__
#define __GRADIENT_EDITOR_H__

#include "gradient_label.h"
#include "shade_widget.h"

class GradientEditor : public QWidget
{
	Q_OBJECT

	public:
	GradientEditor(QWidget *parent = 0);

	void setGradientStops(const QGradientStops &stops);
	QSize sizeHint() const { return QSize(480,320); }

	public slots:
	void pointsUpdated();

	signals:
	void gradientStopsChanged(const QGradientStops &stops);

	private:
	GradientLabel *m_gradient_label;
	ShadeWidget *m_red_shade;
	ShadeWidget *m_green_shade;
	ShadeWidget *m_blue_shade;
};

#endif

