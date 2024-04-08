/*
 *  rectangle.c -- part of FractalNow
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
#include "misc.h"
#include <stdlib.h>

inline void InitRectangle(Rectangle *rectangle, uint_fast32_t x1, uint_fast32_t y1, uint_fast32_t x2, uint_fast32_t y2)
{
	rectangle->x1 = x1;
	rectangle->y1 = y1;
	rectangle->x2 = x2;
	rectangle->y2 = y2;
}

Rectangle CopyRectangle(const Rectangle *rectangle)
{
	Rectangle res;
	InitRectangle(&res, rectangle->x1, rectangle->y1, rectangle->x2, rectangle->y2);
	return res;
}

void CutRectangleMaxSize(Rectangle src, uint_fast32_t size, Rectangle **out, uint_fast32_t *out_size)
{
	uint_fast32_t width = src.x2 - src.x1 + 1;
	uint_fast32_t height = src.y2 - src.y1 + 1;

	uint_fast32_t nb_x = width / size;
	uint_fast32_t nb_y = height / size;

	if (width % size > 0) {
		nb_x++;
	}
	if (height % size > 0) {
		nb_y++;
	}

	*out_size = nb_x*nb_y;
	*out = (Rectangle *)safeMalloc("rectangles", (*out_size)*sizeof(Rectangle));

	Rectangle *p_out = *out;
	uint_fast32_t y1 = src.y1;
	uint_fast32_t y2 = src.y1+size-1;
	for (; y1 <= src.y2; y1+=size, y2+=size) {
		y2 = (y2 > src.y2) ? src.y2 : y2;

		uint_fast32_t x1 = src.x1;
		uint_fast32_t x2 = src.x1+size-1;
		for (; x1 <= src.x2; x1+=size, x2+=size) {
			x2 = (x2 > src.x2) ? src.x2 : x2;

			p_out->x1 = x1;
			p_out->x2 = x2;
			p_out->y1 = y1;
			p_out->y2 = y2;
			++p_out;
		}
	}
}

int CutRectangleInHalf(Rectangle rectangle, Rectangle *out1, Rectangle *out2)
{
	uint_fast32_t width = rectangle.x2 - rectangle.x1;
	uint_fast32_t height = rectangle.y2 - rectangle.y1;
	
	if (width == 0 && height == 0) {
		return 1;
	}

	if (width >= height) {
		width /= 2;
		InitRectangle(out1, rectangle.x1, rectangle.y1, rectangle.x1 + width, rectangle.y2);
		InitRectangle(out2, rectangle.x1 + width + 1, rectangle.y1, rectangle.x2, rectangle.y2);
	} else {
		height /= 2;
		InitRectangle(out1, rectangle.x1, rectangle.y1, rectangle.x2, rectangle.y1 + height);
		InitRectangle(out2, rectangle.x1, rectangle.y1 + height + 1, rectangle.x2, rectangle.y2);
	}

	return 0;
}

int CutRectangleInN(Rectangle rectangle, uint_fast32_t N, Rectangle *out)
{
	uint_fast32_t width = rectangle.x2 + 1 - rectangle.x1;
	uint_fast32_t height = rectangle.y2 + 1 - rectangle.y1;
	if (width*height < N) {
		return 1;
	}

	uint_fast32_t nbRectangles = 1;
	uint_fast32_t tmpNbRectangles;
	InitRectangle(&out[0], rectangle.x1, rectangle.y1, rectangle.x2, rectangle.y2);
	while (nbRectangles < N) {
		tmpNbRectangles = nbRectangles;
		for (uint_fast32_t i = 0; i < tmpNbRectangles; i++) {
			if (CutRectangleInHalf(out[i], &out[i], &out[nbRectangles])) {
				/* This particular rectangle could not be in half.
				 * Assuming that the initial rectangle could be cut in N parts,
				 * we know that there still exists a rectangle that can be cut
				 * in half (until we have reached N rectangles).
				 * Just skip this one and try the next.
				 */
				 continue;
			} else {
				nbRectangles++;
			}

			if (nbRectangles == N) {
				break;
			}
		}
	}

	return 0;
}
