/*
 *  color.h -- part of fractal2D
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

#ifndef __COLOR_H__
#define __COLOR_H__

#include <stdint.h>

#define GET_R(x) ((x & 0x00FF0000)>>16)
#define GET_G(x) ((x & 0x0000FF00)>>8)
#define GET_B(x) (x & 0x000000FF)

typedef struct s_Color {
	uint8_t r, g, b;
} Color;

Color ColorFromUint32(uint32_t color);
Color MixColors(Color C1, double weight1, Color C2, double weight2);
uint32_t ColorManhattanDistance(Color C1, Color C2);
double QuadHeterogeneity(Color C1, Color C2, Color C3, Color C4);
Color LinearInterpolation(Color C1, Color C2, Color C3, Color C4,
			double x, double y);

#endif
