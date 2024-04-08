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
#include "fractal_orbit.h"
#include "misc.h"
#include <string.h>

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

	if (strcmp(CMStr, "simple") == 0) {
		res = CM_SIMPLE;
	} else if (strcmp(CMStr, "average") == 0) {
		res = CM_AVERAGE;
	} else {
		error("Unknown coloring method \'%s\'.\n", str);
	}

	return res;
}

FLOAT identityInterpolationFunction(FLOAT iterationCount, FractalOrbit *orbit,
					RenderingParameters *render)
{
	(void)orbit;
	(void)render;

	return iterationCount;
}

extern inline FLOAT noInterpolationFunction(FLOAT iterationCount, FractalOrbit *orbit,
				RenderingParameters *render)
{
	FLOAT data;
	if (iterationCount < 0) {
		data = iterationCount;
	} else {
		FLOAT SN;
		if (render->addendFunction(orbit, &SN, 1)) {
			/* Could not compute addend function (N may still
			 * be too small).
			 * For example, triangle inequation needs at least
			 * 1 iteration, but it can be 0.
			 * In that case (average does not make any sense),
			 * return simple iteration count.
			 */
			data = iterationCount;
		} else {
			data = SN;
		}
	}

	return data;
}

extern inline FLOAT linearInterpolationFunction(FLOAT iterationCount, FractalOrbit *orbit,
					RenderingParameters *render)
{
	FLOAT data;
	if (iterationCount < 0) {
		data = iterationCount;
	} else {
		FLOAT SN[2];
		if (render->addendFunction(orbit, SN, 2)) {
			/* Linear interpolation impossible because number of
			 * iterations N is too small.
			 * Try no interpolation.
			 */
			return noInterpolationFunction(iterationCount, orbit, render);
		} else {
			/* Ok, linear interpolation is possible. */
			FLOAT d, unused;
			d = modfF(iterationCount, &unused);
			data = d*SN[0] + (1-d)*SN[1];
		}
	}

	return data;
}

/* Function assumes that y >= 1 and compute x^y. */
static inline FLOAT iPowF(FLOAT x, uint_fast32_t y)
{
	FLOAT rem = 1, res = x;
	do {
		if (y % 2) {
			rem *= x;
			--y;
		}
		y >>= 1;
		res *= res;
	} while (y > 1);

	return res*rem;
}

FLOAT splineInterpolationFunction(FLOAT iterationCount, FractalOrbit *orbit,
					RenderingParameters *render)
{
	FLOAT data;
	if (iterationCount < 0) {
		data = iterationCount;
	} else {
		FLOAT SN[4];
		if (render->addendFunction(orbit, SN, 4)) {
			/* Spline interpolation impossible.
			 * Try linear interpolation.
			 */
			return linearInterpolationFunction(iterationCount, orbit, render);
		} else {
			/* Ok, spline interpolation is possible. */
			FLOAT unused;
			FLOAT d1 = modfF(iterationCount, &unused);
			FLOAT d2 = d1*d1;
			FLOAT d3 = d1*d2;

			//data = frac_d*SN[0] + (1-frac_d)*SN[1];
			data = ((-d2+d3)*SN[0] +
				(d1+4*d2-3*d3)*SN[1] +
				(2-5*d2+3*d3)*SN[2] +
				(-d1+2*d2-d3)*SN[3]) /2;
		}
	}

	return data;
}

InterpolationFunction GetInterpolationFunction(const char *str)
{
	int len = strlen(str);
	if (len > 255) {
		error("Unknown interpolation method \'%s\'.\n", str);
	}

	char IMStr[256];
	strcpy(IMStr, str);
	toLowerCase(IMStr);
	InterpolationFunction res;

	if (strcmp(IMStr, "none") == 0) {
		res = noInterpolationFunction;
	} else if (strcmp(IMStr, "linear") == 0) {
		res = linearInterpolationFunction;
	} else if (strcmp(IMStr, "spline") == 0) {
		res = splineInterpolationFunction;;
	} else {
		error("Unknown interpolation method \'%s\'.\n", str);
	}

	return res;
}

