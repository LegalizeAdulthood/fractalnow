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

	res.r = C1.r * weight1 + C2.r * weight2;
	res.g = C1.g * weight1 + C2.g * weight2;
	res.b = C1.b * weight1 + C2.b * weight2;

	return res;
}

