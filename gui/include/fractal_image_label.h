/*
 *  fractal_image_label.h -- part of fractal2D
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
  * \file fractal_image_label.h
  * \brief Header file introducing ImageLabel class.
  *
  * \author Marc Pegon
  */

#ifndef _FRACTAL_IMAGE_LABEL__H__
#define _FRACTAL_IMAGE_LABEL__H__

#include <QImage>
#include <QLabel>
#include <QPaintEvent>

class FractalImageLabel : public QLabel
{
	Q_OBJECT
	 
	public:
	FractalImageLabel();
	void paintEvent(QPaintEvent *event);
	QSize sizeHint() const;
	QSize minimumSizeHint() const;

	void mousePressEvent(QMouseEvent *event);
	void mouseReleaseEvent(QMouseEvent *event);
	void mouseMoveEvent(QMouseEvent *event);
	void wheelEvent(QWheelEvent *event);

	private:
	QImage *image;

	public slots:
	void setImage(QImage *newImage);

	signals:
	void mousePressed(QMouseEvent *event);
	void mouseReleased(QMouseEvent *event);
	void mouseMoved(QMouseEvent *event);
	void mouseWheelRotated(QWheelEvent *event);
};

#endif
