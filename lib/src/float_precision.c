/*
 *  float_precision.c -- part of FractalNow
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

#include "error.h"
#include "float_precision.h"
#include "misc.h"
#include <string.h>

const char *floatPrecisionStr[] = {
	(const char *)"single",
	(const char *)"double",
	(const char *)"ldouble",
	(const char *)"mp"
};

const char *floatPrecisionDescStr[] = {
	(const char *)"Single",
	(const char *)"Double",
	(const char *)"Long double",
	(const char *)"Multiple"
};

const uint_fast32_t nbFloatPrecisions = sizeof(floatPrecisionStr) / sizeof(const char *);

int GetFloatPrecision(FloatPrecision *floatPrecision, const char *str)
{
	int res = 0;
	size_t len = strlen(str);

	if (len > 255) {
		FractalNow_werror("Unknown float precision \'%s\'.\n", str);
	}

	char FPStr[256];
	strcpy(FPStr, str);
	toLowerCase(FPStr);

	uint_fast32_t i;
	for (i = 0; i < nbFloatPrecisions; ++i) {
		if (strcmp(FPStr, floatPrecisionStr[i]) == 0) {
			*floatPrecision = (FloatPrecision)i;
			break;
		}
	}

	if (i == nbFloatPrecisions) {
		FractalNow_werror("Unknown float precision \'%s\'.\n", str);
	}

	end:
	return res;
}

