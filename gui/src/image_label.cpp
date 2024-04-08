/*
 *  image_label.cpp -- part of fractal2D
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
 
#include "image_label.h"
#include <QDebug>
#include <QPainter>

ImageLabel::ImageLabel() : QLabel()
{
	image = NULL;
}

void ImageLabel::paintEvent(QPaintEvent *event)
{
	(void)event;

	if (image != 0) {
		QPainter painter(this);
		painter.setRenderHint(QPainter::SmoothPixmapTransform);
		QRect rect(this->rect());
		if (image->rect().height() != 0 && rect.height() != 0) {
			double imageRatio = image->rect().width() / (double)image->rect().height();
			double rectRatio = rect.width() / (double)rect.height();
			if (rectRatio > imageRatio) {
				rect.setWidth(rect.height() * imageRatio);
			} else if (rectRatio < imageRatio) {
				rect.setHeight(rect.width() / imageRatio);
			}
			rect.moveCenter(this->rect().center());
		}
		painter.drawImage(rect, *image, image->rect());
	}
}

QSize ImageLabel::sizeHint() const
{
	if (image == NULL) {
		return QSize(0, 0);
	} else {
		return image->size();
	}
}

void ImageLabel::setImage(QImage *newImage)
{
	image = newImage;
	if (image != NULL) {
		resize(newImage->size());
	}
}
