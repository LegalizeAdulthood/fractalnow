/*
 *  fractal_iteration_count.c -- part of FractalNow
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
#include "error.h"
#include "misc.h"
#include <string.h>

const char *iterationCountStr[] = {
	(const char *)"discrete",
	(const char *)"continuous",
	(const char *)"smooth"
};

const char *iterationCountDescStr[] = {
	(const char *)"Discrete iteration",
	(const char *)"Continuous iteration",
	(const char *)"Smooth iteration"
};

const uint_fast32_t nbIterationCounts = sizeof(iterationCountStr) / sizeof(const char *);

int GetIterationCount(IterationCount *iterationCount, const char *str)
{
	int res = 0;
	int len = strlen(str);
	if (len > 255) {
		FractalNow_werror("Unknown iteration count \'%s\'.\n", str);
	}

	char ICStr[256];
	strcpy(ICStr, str);
	toLowerCase(ICStr);

	uint_fast32_t i;
	for (i = 0; i < nbIterationCounts; ++i) {
		if (strcmp(ICStr, iterationCountStr[i]) == 0) {
			*(iterationCount) = (IterationCount)i;
			break;
		}
	}
	if (i == nbIterationCounts) {
		FractalNow_werror("Unknown iteration count \'%s\'.\n", str);
	}

	end:
	return res;
}

