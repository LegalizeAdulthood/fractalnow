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

const char *ColoringMethodDescStr[] = {
	(char *)"Simple",
	(char *)"Average"
};

uint_fast32_t nbColoringMethods = sizeof(ColoringMethodStr) / sizeof(char *);

int GetColoringMethod(ColoringMethod *coloringMethod, const char *str)
{
	int res = 0;
	int len = strlen(str);
	if (len > 255) {
		fractal2D_werror("Unknown coloring method \'%s\'.\n", str);
	}

	char CMStr[256];
	strcpy(CMStr, str);
	toLowerCase(CMStr);

	uint_fast32_t i;
	for (i = 0; i < nbColoringMethods; ++i) {
		if (strcmp(CMStr, ColoringMethodStr[i]) == 0) {
			*coloringMethod = (ColoringMethod)i;
			break;
		}
	}
	if (i == nbColoringMethods) {
		fractal2D_werror("Unknown coloring method \'%s\'.\n", str);
	}

	end:
	return res;
}

const char *InterpolationMethodStr[] = {
	(char *)"none",
	(char *)"linear",
	(char *)"spline"
};

const char *InterpolationMethodDescStr[] = {
	(char *)"None",
	(char *)"Linear",
	(char *)"Spline"
};

uint_fast32_t nbInterpolationMethods = sizeof(InterpolationMethodStr) / sizeof(char *);

int GetInterpolationMethod(InterpolationMethod *interpolationMethod, const char *str)
{
	int res = 0;
	int len = strlen(str);
	if (len > 255) {
		fractal2D_werror("Unknown interpolation method \'%s\'.\n", str);
	}

	char IMStr[256];
	strcpy(IMStr, str);
	toLowerCase(IMStr);

	uint_fast32_t i;
	for (i = 0; i < nbInterpolationMethods; ++i) {
		if (strcmp(IMStr, InterpolationMethodStr[i]) == 0) {
			*interpolationMethod = (InterpolationMethod)i;
			break;
		}
	}
	if (i == nbInterpolationMethods) {
		fractal2D_werror("Unknown interpolation method \'%s\'.\n", str);
	}

	end:
	return res;
}

