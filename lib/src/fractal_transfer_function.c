/*
 *  fractal_transfer_function.c -- part of fractal2D
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

#include "fractal_transfer_function.h"
#include "error.h"
#include "floating_point.h"
#include "misc.h"
#include <stdlib.h>
#include <string.h>

FLOAT logp1F(FLOAT x)
{
	return logF(1+x);
}

FLOAT cubertF(FLOAT x)
{
	return powF(x, 1./3);
}

FLOAT identityF(FLOAT x)
{
	return x;
}

FLOAT squareF(FLOAT x)
{
	return x*x;
}

FLOAT cubeF(FLOAT x)
{
	return x*x*x;
}

const TransferFunctionPtr TransferFunctionsArray[] = {
	logp1F,
	cubertF,
	SQRTF,
	identityF,
	squareF,
	cubeF,
	EXPF
};

const char *TransferFunctionStr[] = {
	(char *)"log",
	(char *)"cuberoot",
	(char *)"squareroot",
	(char *)"identity",
	(char *)"square",
	(char *)"cube",
	(char *)"exp"
};

const char *TransferFunctionDescStr[] = {
	(char *)"Logarithm",
	(char *)"Cube root",
	(char *)"Square root",
	(char *)"Identity",
	(char *)"Square",
	(char *)"Cube",
	(char *)"Exponential"
};

uint_fast32_t nbTransferFunctions = sizeof(TransferFunctionStr) / sizeof(char *);

int GetTransferFunction(TransferFunction *transferFunction, const char *str)
{
	int res = 0;
	size_t len = strlen(str);

	if (len > 255) {
		fractal2D_werror("Unknown transfer function \'%s\'.\n", str);
	}

	char TFStr[256];
	strcpy(TFStr, str);
	toLowerCase(TFStr);

	uint_fast32_t i;
	for (i = 0; i < nbTransferFunctions; ++i) {
		if (strcmp(TFStr, TransferFunctionStr[i]) == 0) {
			*(transferFunction) = (TransferFunction)i;
			break;
		}
	}
	if (i == nbTransferFunctions) {
		fractal2D_werror("Unknown transfer function \'%s\'.\n", str);
	}

	end:
	return res;
}

TransferFunctionPtr GetTransferFunctionPtr(TransferFunction transferFunction)
{
	return TransferFunctionsArray[(int)transferFunction];
}

