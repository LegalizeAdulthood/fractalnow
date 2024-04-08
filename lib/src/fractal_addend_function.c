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
#include "misc.h"
#include <string.h>

const char *AddendFunctionStr[] = {
	(char *)"triangleinequality",
	(char *)"curvature",
	(char *)"stripe"
};

const char *AddendFunctionDescStr[] = {
	(char *)"Triangle inequality",
	(char *)"Curvature",
	(char *)"Stripe"
};

uint_fast32_t nbAddendFunctions = sizeof(AddendFunctionStr) / sizeof(char *);

int GetAddendFunction(AddendFunction *addendFunction, const char *str)
{
	int res = 0;
	int len = strlen(str);
	if (len > 255) {
		fractal2D_werror("Unknown addend function \'%s\'.\n", str);
	}

	char AFStr[256];
	strcpy(AFStr, str);
	toLowerCase(AFStr);

	uint_fast32_t i;
	for (i = 0; i < nbAddendFunctions; ++i) {
		if (strcmp(AFStr, AddendFunctionStr[i]) == 0) {
			*addendFunction = (AddendFunction)i;
			break;
		}
	}
	if (i == nbAddendFunctions) {
		fractal2D_werror("Unknown addend function \'%s\'.\n", str);
	}

	end:
	return res;
}
