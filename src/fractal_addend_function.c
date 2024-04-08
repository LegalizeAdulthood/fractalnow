/*
 *  fractal_addend_function.c -- part of fractal2D
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
#include "fractal_addend_function.h"
#include "fractal_orbit.h"
#include "misc.h"
#include <string.h>

static inline uint_fast32_t min(uint_fast32_t x, uint_fast32_t y)
{
	if (x < y) {
		return x;
	} else {
		return y;
	}
}

int triangleInequalityAddendFunction(FractalOrbit *orbit, FLOAT *SN, uint_fast32_t size)
{
	uint_fast32_t N = orbit->N;
	FLOAT *rn = orbit->rn;
	FLOAT *mn = orbit->mn;
	FLOAT *Mn = orbit->Mn;
	uint_fast32_t m = 1;
	int res;

	if (N >= m+size-1) {
		FLOAT diff = 0;
		uint_fast32_t zeros = 0;
		for (uint_fast32_t n = m; n <= N; ++n) {
			diff = Mn[n]-mn[n];
			if (diff == 0) {
				orbit->tn[n] = 0;
				++zeros;
			} else {
				orbit->tn[n] = (rn[n]-mn[n]) / diff;
			}
		}
		for (uint_fast32_t i = 0; i < size; ++i) {
			SN[i] = 0;
			for (uint_fast32_t n = m; n <= N-i; ++n) {
				SN[i] += orbit->tn[n];
			}
			SN[i] /= N+1-m-i-zeros;
		}

		res = 0;
	} else {
		res = 1;
	}

	return res;
}

AddendFunction GetAddendFunction(const char *str)
{
	int len = strlen(str);
	if (len > 255) {
		error("Unknown addend function \'%s\'.\n", str);
	}

	char AFStr[256];
	strcpy(AFStr, str);
	toLowerCase(AFStr);
	AddendFunction res;

	if (strcmp(AFStr, "triangleinequality") == 0) {
		res = triangleInequalityAddendFunction;
	} else {
		error("Unknown addend function \'%s\'.\n", str);
	}

	return res;
}
