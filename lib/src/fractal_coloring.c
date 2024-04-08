/*
 *  fractal_coloring.c -- part of fractal2D
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

#include "fractal.h"
#include "fractal_coloring.h"
#include "misc.h"
#include <string.h>

const char *ColoringMethodStr[] = {
	(char *)"simple",
	(char *)"average"
};

ColoringMethod GetColoringMethod(const char *str)
{
	int len = strlen(str);
	if (len > 255) {
		error("Unknown coloring method \'%s\'.\n", str);
	}

	char CMStr[256];
	strcpy(CMStr, str);
	toLowerCase(CMStr);
	ColoringMethod res;

	uint_fast32_t i;
	uint_fast32_t nb_elem = sizeof(ColoringMethodStr) / sizeof(char *);
	for (i = 0; i < nb_elem; ++i) {
		if (strcmp(CMStr, ColoringMethodStr[i]) == 0) {
			res = (ColoringMethod)i;
			break;
		}
	}
	if (i == nb_elem) {
		error("Unknown coloring method \'%s\'.\n", str);
	}

	return res;
}

InterpolationMethod GetInterpolationMethod(const char *str)
{
	int len = strlen(str);
	if (len > 255) {
		error("Unknown interpolation method \'%s\'.\n", str);
	}

	char IMStr[256];
	strcpy(IMStr, str);
	toLowerCase(IMStr);
	InterpolationMethod res;

	if (strcmp(IMStr, "none") == 0) {
		res = IM_NONE;
	} else if (strcmp(IMStr, "linear") == 0) {
		res = IM_LINEAR;
	} else if (strcmp(IMStr, "spline") == 0) {
		res = IM_SPLINE;
	} else {
		error("Unknown interpolation method \'%s\'.\n", str);
	}

	return res;
}

