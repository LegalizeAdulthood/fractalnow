/*
 *  fractal_iteration_count.c -- part of fractal2D
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
#include "fractal_iteration_count.h"
#include "fractal_orbit.h"
#include "error.h"
#include "misc.h"
#include <string.h>

FLOAT discreteIterationCount(Fractal *fractal, FractalOrbit *orbit)
{
	uint_fast32_t N = orbit->N;
	FLOAT rN = orbit->rn[N];

	FLOAT res;
	if (rN < fractal->escapeRadius) {
		res = -(FLOAT)N;
	} else {
		res = (FLOAT)N;
	}
	return res;
}

FLOAT continuousIterationCount(Fractal *fractal, FractalOrbit *orbit)
{
	uint_fast32_t N = orbit->N;
	FLOAT rN = orbit->rn[N];

	FLOAT res;
	if (rN < fractal->escapeRadius) {
		res = -(FLOAT)N;
	} else {
		res = N+(fractal->escapeRadiusP-rN)/(fractal->escapeRadiusP-fractal->escapeRadius);
	}
	return res;
}

FLOAT smoothIterationCount(Fractal *fractal, FractalOrbit *orbit)
{
	uint_fast32_t N = orbit->N;
	FLOAT rN = orbit->rn[N];

	FLOAT res;
	if (rN < fractal->escapeRadius) {
		res = -(FLOAT)N;
	} else {
		res = N+1+logF(fractal->logEscapeRadius/logF(rN))/fractal->logP;
	}
	return res;
}

IterationCountFunction GetIterationCountFunction(const char *str)
{
	int len = strlen(str);
	if (len > 255) {
		error("Unknown iteration count type \'%s\'.\n", str);
	}

	char ICStr[256];
	strcpy(ICStr, str);
	toLowerCase(ICStr);

	if (strcmp(ICStr, "discrete") == 0) {
		return discreteIterationCount;
	} else if (strcmp(ICStr, "continuous") == 0) {
		return continuousIterationCount;
	} else if (strcmp(ICStr, "smooth") == 0) {
		return smoothIterationCount;
	} else {
		error("Unknown iteration count type \'%s\'.\n", str);
	}
}
