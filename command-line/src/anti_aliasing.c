/*
 *  anti_aliasing.c -- part of fractal2D
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

#include "anti_aliasing.h"
#include "error.h"
#include "misc.h"
#include <stdlib.h>
#include <string.h>

const char *AntiAliasingMethodStr[] = {
	(char *)"none",
	(char *)"blur",
	(char *)"oversampling",
	(char *)"adaptive"
};

AntiAliasingMethod GetAAM(const char *str)
{
	int len = strlen(str);
	if (len > 255) {
		error("Unknown anti-aliasing method \'%s\'.\n", str);
	}

	char AAMStr[256];
	strcpy(AAMStr, str);
	toLowerCase(AAMStr);
	AntiAliasingMethod res;

	uint_fast32_t i;
	uint_fast32_t nb_elem = sizeof(AntiAliasingMethodStr) / sizeof(char *);
	for (i = 0; i < nb_elem; ++i) {
		if (strcmp(AAMStr, AntiAliasingMethodStr[i]) == 0) {
			res = (AntiAliasingMethod)i;
			break;
		}
	}
	if (i == nb_elem) {
		error("Unknown anti-aliasing method \'%s\'.\n", str);
	}

	return res;
}

