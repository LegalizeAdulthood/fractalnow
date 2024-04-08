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
#ifdef _ENABLE_LDOUBLE_FLOATS
	(const char *)"ldouble",
#endif
#ifdef _ENABLE_MP_FLOATS
	(const char *)"mp"
#endif
};

const char *floatPrecisionDescStr[] = {
	(const char *)"Single",
	(const char *)"Double",
#ifdef _ENABLE_LDOUBLE_FLOATS
	(const char *)"Long double",
#endif
#ifdef _ENABLE_MP_FLOATS
	(const char *)"Multiple"
#endif
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

#ifdef _ENABLE_MP_FLOATS
int64_t fractalnow_mp_precision = DEFAULT_MP_PRECISION;

int64_t GetMinMPFloatPrecision()
{
	if (MPFR_PREC_MIN < INT64_MIN) {
		return INT64_MIN;
	} else {
		return (int64_t)MPFR_PREC_MIN;
	}
}

int64_t GetMaxMPFloatPrecision()
{
	if (MPFR_PREC_MAX > INT64_MAX) {
		return INT64_MAX;
	} else {
		return (int64_t)MPFR_PREC_MAX;
	}
}

int64_t GetMPFloatPrecision()
{
	return fractalnow_mp_precision;
}

void SetMPFloatPrecision(int64_t precision)
{
	if (precision >= MPFR_PREC_MIN && precision <= MPFR_PREC_MAX) {
		fractalnow_mp_precision = precision;
		mpfr_set_default_prec((mpfr_prec_t)fractalnow_mp_precision);
	}
}
#endif

