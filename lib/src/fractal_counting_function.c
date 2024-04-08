/*
 *  fractal_counting_function.c -- part of FractalNow
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
#include "fractal_counting_function.h"
#include "error.h"
#include "misc.h"
#include <string.h>

/* Assumes rN < escapeRadius */
FLOATT DiscreteIterationCount(const Fractal *fractal, uint_fast32_t N, FLOATT rN)
{
	UNUSED(rN);
	UNUSED(fractal);

	return (FLOATT)N;
}

/* Assumes rN < escapeRadius */
FLOATT ContinuousIterationCount(const Fractal *fractal, uint_fast32_t N, FLOATT rN)
{
	return N+(fractal->escapeRadiusP-rN)/(fractal->escapeRadiusP-fractal->escapeRadius);
}

/* Assumes rN < escapeRadius */
FLOATT SmoothIterationCount(const Fractal *fractal, uint_fast32_t N, FLOATT rN)
{
	return N+1+logF(fractal->logEscapeRadius/logF(rN))/fractal->logNormP;
}

const CountingFunctionPtr countingFunctionsArray[] = {
	DiscreteIterationCount,
	ContinuousIterationCount,
	SmoothIterationCount
};

const char *countingFunctionStr[] = {
	(const char *)"discrete",
	(const char *)"continuous",
	(const char *)"smooth"
};

const char *countingFunctionDescStr[] = {
	(const char *)"Discrete iteration",
	(const char *)"Continuous iteration",
	(const char *)"Smooth iteration"
};

const uint_fast32_t nbCountingFunctions = sizeof(countingFunctionStr) / sizeof(const char *);

int GetCountingFunction(CountingFunction *countingFunction, const char *str)
{
	int res = 0;
	int len = strlen(str);
	if (len > 255) {
		FractalNow_werror("Unknown counting function \'%s\'.\n", str);
	}

	char CFStr[256];
	strcpy(CFStr, str);
	toLowerCase(CFStr);

	uint_fast32_t i;
	for (i = 0; i < nbCountingFunctions; ++i) {
		if (strcmp(CFStr, countingFunctionStr[i]) == 0) {
			*(countingFunction) = (CountingFunction)i;
			break;
		}
	}
	if (i == nbCountingFunctions) {
		FractalNow_werror("Unknown counting function \'%s\'.\n", str);
	}

	end:
	return res;
}

CountingFunctionPtr GetCountingFunctionPtr(CountingFunction countingFunction)
{
	return countingFunctionsArray[(int)countingFunction];
}
