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

#include "gradients.h"
#include "hoverpoints.h"

ShadeWidget::ShadeWidget(ShadeType type, QWidget *parent)
    : QWidget(parent), m_shade_type(type), m_alpha_gradient(QLinearGradient(0, 0, 0, 0))
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

    setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Fixed);

    connect(m_hoverPoints, SIGNAL(pointsChanged(QPolygonF)), this, SIGNAL(colorsChanged()));
}


QPolygonF ShadeWidget::points() const
{
    return m_hoverPoints->points();
}


uint ShadeWidget::colorAt(int x)
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
            m_alpha_gradient.setColorAt(stops.at(i).first, QColor(c.red(), c.green(), c.blue()));
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
            m_shade = QImage(size(), QImage::Format_ARGB32_Premultiplied);
            m_shade.fill(0);

            QPainter p(&m_shade);
            p.fillRect(rect(), m_alpha_gradient);

            p.setCompositionMode(QPainter::CompositionMode_DestinationIn);
            QLinearGradient fade(0, 0, 0, height());
            fade.setColorAt(0, QColor(0, 0, 0, 255));
            fade.setColorAt(1, QColor(0, 0, 0, 0));
            p.fillRect(rect(), fade);

        } else {
            m_shade = QImage(size(), QImage::Format_RGB32);
            QLinearGradient shade(0, 0, 0, height());
            shade.setColorAt(1, Qt::black);

            if (m_shade_type == RedShade)
                shade.setColorAt(0, Qt::red);
            else if (m_shade_type == GreenShade)
                shade.setColorAt(0, Qt::green);
            else
                shade.setColorAt(0, Qt::blue);

            QPainter p(&m_shade);
            p.fillRect(rect(), shade);
        }
    }


}


GradientEditor::GradientEditor(QWidget *parent)
    : QWidget(parent)
{
    QVBoxLayout *vbox = new QVBoxLayout(this);
    vbox->setSpacing(1);
    vbox->setMargin(1);

    m_red_shade = new ShadeWidget(ShadeWidget::RedShade, this);
    m_green_shade = new ShadeWidget(ShadeWidget::GreenShade, this);
    m_blue_shade = new ShadeWidget(ShadeWidget::BlueShade, this);
    m_alpha_shade = new ShadeWidget(ShadeWidget::ARGBShade, this);

    vbox->addWidget(m_red_shade);
    vbox->addWidget(m_green_shade);
    vbox->addWidget(m_blue_shade);
    vbox->addWidget(m_alpha_shade);

    connect(m_red_shade, SIGNAL(colorsChanged()), this, SLOT(pointsUpdated()));
    connect(m_green_shade, SIGNAL(colorsChanged()), this, SLOT(pointsUpdated()));
    connect(m_blue_shade, SIGNAL(colorsChanged()), this, SLOT(pointsUpdated()));
    connect(m_alpha_shade, SIGNAL(colorsChanged()), this, SLOT(pointsUpdated()));
}


inline static bool x_less_than(const QPointF &p1, const QPointF &p2)
{
    return p1.x() < p2.x();
}


void GradientEditor::pointsUpdated()
{
    qreal w = m_alpha_shade->width();

    QGradientStops stops;

    QPolygonF points;

    points += m_red_shade->points();
    points += m_green_shade->points();
    points += m_blue_shade->points();
    points += m_alpha_shade->points();

    qSort(points.begin(), points.end(), x_less_than);

    for (int i=0; i<points.size(); ++i) {
        qreal x = int(points.at(i).x());
        if (i < points.size() - 1 && x == points.at(i+1).x())
            continue;
        QColor color((0x00ff0000 & m_red_shade->colorAt(int(x))) >> 16,
                     (0x0000ff00 & m_green_shade->colorAt(int(x))) >> 8,
                     (0x000000ff & m_blue_shade->colorAt(int(x))),
                     (0xff000000 & m_alpha_shade->colorAt(int(x))) >> 24);

        if (x / w > 1)
            return;

        stops << QGradientStop(x / w, color);
    }

    m_alpha_shade->setGradientStops(stops);

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
    QPolygonF pts_red, pts_green, pts_blue, pts_alpha;

    qreal h_red = m_red_shade->height();
    qreal h_green = m_green_shade->height();
    qreal h_blue = m_blue_shade->height();
    qreal h_alpha = m_alpha_shade->height();

    for (int i=0; i<stops.size(); ++i) {
        qreal pos = stops.at(i).first;
        QRgb color = stops.at(i).second.rgba();
        pts_red << QPointF(pos * m_red_shade->width(), h_red - qRed(color) * h_red / 255);
        pts_green << QPointF(pos * m_green_shade->width(), h_green - qGreen(color) * h_green / 255);
        pts_blue << QPointF(pos * m_blue_shade->width(), h_blue - qBlue(color) * h_blue / 255);
        pts_alpha << QPointF(pos * m_alpha_shade->width(), h_alpha - qAlpha(color) * h_alpha / 255);
    }

    set_shade_points(pts_red, m_red_shade);
    set_shade_points(pts_green, m_green_shade);
    set_shade_points(pts_blue, m_blue_shade);
    set_shade_points(pts_alpha, m_alpha_shade);

}

