/*
 *  fractal_image_label.cpp -- part of fractal2D
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
 
#include "fractal_image_label.h"
#include <QDebug>
#include <QPainter>

FractalImageLabel::FractalImageLabel() : QLabel()
{
	image = NULL;
	setCursor(QCursor(Qt::OpenHandCursor));
	setContextMenuPolicy(Qt::CustomContextMenu);
}

void FractalImageLabel::paintEvent(QPaintEvent *event)
{
	(void)event;
	setFocusPolicy(Qt::StrongFocus);

	if (image != 0) {
		QPainter painter(this);
		painter.setRenderHint(QPainter::SmoothPixmapTransform);
		painter.drawImage(image->rect(), *image, image->rect());
	}
}

QSize FractalImageLabel::sizeHint() const
{
	if (image == NULL) {
		return QSize(0, 0);
	} else {
		return image->size();
	}
}

QSize FractalImageLabel::minimumSizeHint() const
{
	if (image == NULL) {
		return QSize(0, 0);
	} else {
		return image->size();
	}
}

void FractalImageLabel::setImage(QImage *newImage)
{
	image = newImage;
	if (image != NULL) {
		resize(newImage->size());
	}
}

void FractalImageLabel::mousePressEvent(QMouseEvent *event)
{
	emit mousePressed(event);
}

void FractalImageLabel::mouseReleaseEvent(QMouseEvent *event)
{
	emit mouseReleased(event);
}

void FractalImageLabel::mouseMoveEvent(QMouseEvent *event)
{
	emit mouseMoved(event);
}

void FractalImageLabel::wheelEvent(QWheelEvent *event)
{
	emit mouseWheelRotated(event);
}
