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
#include "error.h"
#include <stdlib.h>

inline Color ColorFromUint32(uint32_t color)
{
	Color res;
	res.r = GET_R8(color);
	res.g = GET_G8(color);
	res.b = GET_B8(color);
	res.bytesPerComponent = 1;

	return res;
}

inline Color ColorFromUint64(uint64_t color)
{
	Color res;
	res.r = GET_R16(color);
	res.g = GET_G16(color);
	res.b = GET_B16(color);
	res.bytesPerComponent = 2;

	return res;
}

Color MixColors(Color C1, FLOAT weight1, Color C2, FLOAT weight2)
{
	Color res;

	res.bytesPerComponent = C1.bytesPerComponent;
	res.r = C1.r * weight1 + C2.r * weight2;
	res.g = C1.g * weight1 + C2.g * weight2;
	res.b = C1.b * weight1 + C2.b * weight2;

	return res;
}

inline FLOAT ColorManhattanDistance(Color C1, Color C2)
{
	FLOAT res = 0;
	uint_fast32_t sum = (labs(C1.r-(long int)C2.r)+labs(C1.g-(long int)C2.g)+
			labs(C1.b-(long int)C2.b));

	switch (C1.bytesPerComponent) {
	case 1:
		res = sum / (FLOAT)(3*UINT8_MAX);
		break;
	case 2:
		res = sum / (FLOAT)(3*UINT16_MAX);
		break;
	default:
		error("Invalid bytes per component.\n");
		break;
	}

	return res;
}

inline FLOAT QuadAvgDissimilarity(Color C[4])
{
	Color avg;
	avg.r = (C[0].r + C[1].r + C[2].r + C[3].r) / 4;
	avg.g = (C[0].g + C[1].g + C[2].g + C[3].g) / 4;
	avg.b = (C[0].b + C[1].b + C[2].b + C[3].b) / 4;
	return (ColorManhattanDistance(C[0],avg)+
		ColorManhattanDistance(C[1],avg)+
		ColorManhattanDistance(C[2],avg)+
		ColorManhattanDistance(C[3],avg)) / 4;
}

inline Color QuadLinearInterpolation(Color C[4], FLOAT x, FLOAT y)
{
	Color res;
	res.bytesPerComponent = C[0].bytesPerComponent;
	res.r = (C[0].r*(1.-x)+C[1].r*x)*(1.-y) + (C[2].r*(1.-x)+C[3].r*x)*y;
	res.g = (C[0].g*(1.-x)+C[1].g*x)*(1.-y) + (C[2].g*(1.-x)+C[3].g*x)*y;
	res.b = (C[0].b*(1.-x)+C[1].b*x)*(1.-y) + (C[2].b*(1.-x)+C[3].b*x)*y;

	return res;
}
