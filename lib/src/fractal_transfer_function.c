/*
 *  fractal_transfer_function.c -- part of FractalNow
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

FLOATT logp1logp1F(FLOATT x)
{
	return logF(logF(1+x)+1);
}
	
FLOATT logp1F(FLOATT x)
{
	return logF(1+x);
}

FLOATT cubertF(FLOATT x)
{
	return powF(x, 1./3);
}

FLOATT identityF(FLOATT x)
{
	return x;
}

FLOATT squareF(FLOATT x)
{
	return x*x;
}

FLOATT cubeF(FLOATT x)
{
	return x*x*x;
}

const TransferFunctionPtr transferFunctionsArray[] = {
	logp1logp1F,
	logp1F,
	cubertF,
	SQRTF,
	identityF,
	squareF,
	cubeF,
	EXPF
};

const char *transferFunctionStr[] = {
	(const char *)"loglog",
	(const char *)"log",
	(const char *)"cuberoot",
	(const char *)"squareroot",
	(const char *)"identity",
	(const char *)"square",
	(const char *)"cube",
	(const char *)"exp"
};

const char *transferFunctionDescStr[] = {
	(const char *)"Log log",
	(const char *)"Logarithm",
	(const char *)"Cube root",
	(const char *)"Square root",
	(const char *)"Identity",
	(const char *)"Square",
	(const char *)"Cube",
	(const char *)"Exponential"
};

const uint_fast32_t nbTransferFunctions = sizeof(transferFunctionStr) / sizeof(const char *);

int GetTransferFunction(TransferFunction *transferFunction, const char *str)
{
	int res = 0;
	size_t len = strlen(str);

	if (len > 255) {
		FractalNow_werror("Unknown transfer function \'%s\'.\n", str);
	}

	char TFStr[256];
	strcpy(TFStr, str);
	toLowerCase(TFStr);

	uint_fast32_t i;
	for (i = 0; i < nbTransferFunctions; ++i) {
		if (strcmp(TFStr, transferFunctionStr[i]) == 0) {
			*(transferFunction) = (TransferFunction)i;
			break;
		}
	}
	if (i == nbTransferFunctions) {
		FractalNow_werror("Unknown transfer function \'%s\'.\n", str);
	}

	end:
	return res;
}

TransferFunctionPtr GetTransferFunctionPtr(TransferFunction transferFunction)
{
	return transferFunctionsArray[(int)transferFunction];
}

