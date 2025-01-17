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

#ifndef __HOVERPOINTS_H__
#define __HOVERPOINTS_H__

#include <QtGui>
#include <QApplication>
#include <QWidget>

QT_FORWARD_DECLARE_CLASS(QBypassWidget)

class HoverPoints : public QObject
{
	Q_OBJECT

	public:
	enum PointShape {
		CircleShape,
		RectangleShape
	};

	enum LockType {
		LockToLeft   = 0x01,
		LockToRight  = 0x02,
		LockToTop    = 0x04,
		LockToBottom = 0x08
	};

	enum SortType {
		NoSort,
		XSort,
		YSort
	};

	enum ConnectionType {
		NoConnection,
		LineConnection,
		CurveConnection
	};

	HoverPoints(QWidget *widget, PointShape shape);

	bool eventFilter(QObject *object, QEvent *event);

	void paintPoints();

	inline QRectF boundingRect() const;
	void setBoundingRect(const QRectF &boundingRect) { m_bounds = boundingRect; }

	QPolygonF points() const { return m_points; }
	void setPoints(const QPolygonF &points);

	QSizeF pointSize() const { return m_pointSize; }
	void setPointSize(const QSizeF &size) { m_pointSize = size; }

	SortType sortType() const { return m_sortType; }
	void setSortType(SortType sortType) { m_sortType = sortType; }

	ConnectionType connectionType() const { return m_connectionType; }
	void setConnectionType(ConnectionType connectionType) { m_connectionType = connectionType; }

	void setConnectionPen(const QPen &pen) { m_connectionPen = pen; }
	void setShapePen(const QPen &pen) { m_pointPen = pen; }
	void setShapeBrush(const QBrush &brush) { m_pointBrush = brush; }

	void setPointLock(int pos, LockType lock) { m_locks[pos] = lock; }

	void setEditable(bool editable) { m_editable = editable; }
	bool editable() const { return m_editable; }

	public slots:
	void setEnabled(bool enabled);
	void setDisabled(bool disabled) { setEnabled(!disabled); }

	signals:
	void pointsChanged(const QPolygonF &points);

	public:
	void firePointChange();

	private:
	inline QRectF pointBoundingRect(int i) const;
	void movePoint(int i, const QPointF &newPos, bool emitChange = true);

	QWidget *m_widget;

	QPolygonF m_points;
	QRectF m_bounds;
	PointShape m_shape;
	SortType m_sortType;
	ConnectionType m_connectionType;

	QVector<uint> m_locks;

	QSizeF m_pointSize;
	int m_currentIndex;
	bool m_editable;
	bool m_enabled;

	QHash<int, int> m_fingerPointMapping;

	QPen m_pointPen;
	QBrush m_pointBrush;
	QPen m_connectionPen;
};

inline QRectF HoverPoints::pointBoundingRect(int i) const
{
	QPointF p = m_points.at(i);
	qreal w = m_pointSize.width();
	qreal h = m_pointSize.height();
	qreal x = p.x() - w / 2;
	qreal y = p.y() - h / 2;
	return QRectF(x, y, w, h);
}

inline QRectF HoverPoints::boundingRect() const
{
	if (m_bounds.isEmpty()) {
		return m_widget->rect();
	} else {
		return m_bounds;
	}
}

#endif

