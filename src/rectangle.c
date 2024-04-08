/*
 *  rectangle.c -- part of fractal2D
 *
 *  Copyright (c) 2011 Marc Pegon <pe.marc@free.fr>
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
 
#include "rectangle.h"

void InitRectangle(Rectangle *rectangle, uint32_t x1, uint32_t y1, uint32_t x2, uint32_t y2)
{
	rectangle->x1 = x1;
	rectangle->y1 = y1;
	rectangle->x2 = x2;
	rectangle->y2 = y2;
}

void CutRectangleInHalf(Rectangle rectangle, Rectangle *out1, Rectangle *out2)
{
	uint32_t width = rectangle.x2 - rectangle.x1;
	uint32_t height = rectangle.y2 - rectangle.y1;
	if (width >= height) {
		width /= 2;
		InitRectangle(out1, rectangle.x1, rectangle.y1, rectangle.x1 + width, rectangle.y2);
		InitRectangle(out2, rectangle.x1 + width + 1, rectangle.y1, rectangle.x2, rectangle.y2);
	} else {
		height /= 2;
		InitRectangle(out1, rectangle.x1, rectangle.y1, rectangle.x2, rectangle.y1 + height);
		InitRectangle(out2, rectangle.x1, rectangle.y1 + height + 1, rectangle.x2, rectangle.y2);
	}
}

void CutRectangleInN(Rectangle rectangle, uint32_t N, Rectangle *out)
{
	uint32_t nbRectangles = 1;
	uint32_t tmpNbRectangles;
	InitRectangle(&out[0], rectangle.x1, rectangle.y1, rectangle.x2, rectangle.y2);
	while (nbRectangles < N) {
		tmpNbRectangles = nbRectangles;
		for (uint32_t i = 0; i < tmpNbRectangles; i++) {
			CutRectangleInHalf(out[i], &out[i], &out[nbRectangles]);
			nbRectangles++;

			if (nbRectangles == N) {
				break;
			}
		}
	}
}
