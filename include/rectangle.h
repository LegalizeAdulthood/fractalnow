/*
 *  rectangle.h -- part of fractal2D
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
 
#ifndef __RECTANGLE_H__
#define __RECTANGLE_H__

#include <stdint.h>

typedef struct s_Rectangle {
	uint32_t x1, y1;
	uint32_t x2, y2;
} Rectangle;

void InitRectangle(Rectangle *rectangle, uint32_t x1, uint32_t y1, uint32_t x2, uint32_t y2);

// Assumes that rectangle area is more than 1 pixel
void CutRectangleInHalf(Rectangle rectangle, Rectangle *out1, Rectangle *out2);

void CutRectangleInN(Rectangle rectangle, uint32_t N, Rectangle *out);

#endif
