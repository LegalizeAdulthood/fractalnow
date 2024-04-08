/*
 *  fractal_counting_function.c -- part of fractal2D
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
FLOAT DiscreteIterationCount(Fractal *fractal, uint_fast32_t N, FLOAT rN)
{
	(void)rN;
	(void)fractal;

	return (FLOAT)N;
}

/* Assumes rN < escapeRadius */
FLOAT ContinuousIterationCount(Fractal *fractal, uint_fast32_t N, FLOAT rN)
{
	return N+(fractal->escapeRadiusP-rN)/(fractal->escapeRadiusP-fractal->escapeRadius);
}

/* Assumes rN < escapeRadius */
FLOAT SmoothIterationCount(Fractal *fractal, uint_fast32_t N, FLOAT rN)
{
	return N+1+logF(fractal->logEscapeRadius/logF(rN))/fractal->logP;
}

const CountingFunctionPtr CountingFunctionsArray[] = {
	DiscreteIterationCount,
	ContinuousIterationCount,
	SmoothIterationCount
};

const char *CountingFunctionStr[] = {
	(char *)"discrete",
	(char *)"continuous",
	(char *)"smooth"
};

const char *CountingFunctionDescStr[] = {
	(char *)"Discrete iteration",
	(char *)"Continuous iteration",
	(char *)"Smooth iteration"
};

uint_fast32_t nbCountingFunctions = sizeof(CountingFunctionStr) / sizeof(char *);

CountingFunction GetCountingFunction(const char *str)
{
	int len = strlen(str);
	if (len > 255) {
		error("Unknown counting function \'%s\'.\n", str);
	}

	char CFStr[256];
	strcpy(CFStr, str);
	toLowerCase(CFStr);
	CountingFunction res;

	uint_fast32_t i;
	for (i = 0; i < nbCountingFunctions; ++i) {
		if (strcmp(CFStr, CountingFunctionStr[i]) == 0) {
			res = (CountingFunction)i;
			break;
		}
	}
	if (i == nbCountingFunctions) {
		error("Unknown counting function \'%s\'.\n", str);
	}

	return res;
}

CountingFunctionPtr GetCountingFunctionPtr(CountingFunction countingFunction)
{
	return CountingFunctionsArray[(int)countingFunction];
}
