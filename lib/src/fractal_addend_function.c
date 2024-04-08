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
	(char *)("triangleinequality")
};

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

	uint_fast32_t i;
	uint_fast32_t nb_elem = sizeof(AddendFunctionStr) / sizeof(char *);
	for (i = 0; i < nb_elem; ++i) {
		if (strcmp(AFStr, AddendFunctionStr[i]) == 0) {
			res = (AddendFunction)i;
			break;
		}
	}
	if (i == nb_elem) {
		error("Unknown addend function \'%s\'.\n", str);
	}

	return res;
}
