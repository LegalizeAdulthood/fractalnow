/*
 *  color.c -- part of fractal2D
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

#include "color.h"
#include <stdlib.h>

inline Color ColorFromUint32(uint32_t color)
{
	Color res;
	res.r = GET_R(color);
	res.g = GET_G(color);
	res.b = GET_B(color);

	return res;
}

Color MixColors(Color C1, double weight1, Color C2, double weight2)
{
	Color res;

	res.r = C1.r * weight1 + C2.r * weight2;
	res.g = C1.g * weight1 + C2.g * weight2;
	res.b = C1.b * weight1 + C2.b * weight2;

	return res;
}

uint32_t ColorManhattanDistance(Color C1, Color C2)
{
	return abs(C1.r-C2.r)+abs(C1.g-C2.g)+abs(C1.b-C2.b);
}

inline double QuadHeterogeneity(Color C1, Color C2, Color C3, Color C4)
{
	Color avg;
	avg.r = (C1.r + C2.r + C3.r + C4.r) / 4;
	avg.g = (C1.g + C2.g + C3.g + C4.g) / 4;
	avg.b = (C1.b + C2.b + C3.b + C4.b) / 4;
	return (double)(ColorManhattanDistance(C1,avg)+
			ColorManhattanDistance(C2,avg)+
			ColorManhattanDistance(C3,avg)+
			ColorManhattanDistance(C4,avg)) / (4*3*255);
}

inline Color LinearInterpolation(Color C1, Color C2, Color C3, Color C4,
			double x, double y)
{
	Color res;
	res.r = (C1.r*(1.-x)+C2.r*x)*(1.-y) + (C3.r*(1.-x)+C4.r*x)*y;
	res.g = (C1.g*(1.-x)+C2.g*x)*(1.-y) + (C3.g*(1.-x)+C4.g*x)*y;
	res.b = (C1.b*(1.-x)+C2.b*x)*(1.-y) + (C3.b*(1.-x)+C4.b*x)*y;

	return res;
}
