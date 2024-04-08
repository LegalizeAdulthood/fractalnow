/*
 *  image_label.h -- part of fractal2D
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
 
 /**
  * \file help.h
  * \brief Header file introducing ImageLabel class.
  *
  * \author Marc Pegon
  */

#ifndef _IMAGE_LABEL__H__
#define _IMAGE_LABEL__H__

#include <QImage>
#include <QLabel>
#include <QPaintEvent>

class ImageLabel : public QLabel
{
	Q_OBJECT
	 
	public:
	ImageLabel();
	void paintEvent(QPaintEvent *event);
	QSize sizeHint() const;

	QImage *image;

	private:

	public slots:
	void setImage(QImage *newImage);
};

#endif
