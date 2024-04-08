/*
 *  misc.c -- part of FractalNow
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

#include "misc.h"
#include "error.h"
#include <complex.h>
#include <ctype.h>

void toLowerCase(char *str)
{
	for (; *str != '\0'; ++str) {
		*str = tolower(*str);
	}
}

void *safeMalloc(const char *name, uint_least64_t size)
{
	if (size > SIZE_MAX) {
		FractalNow_alloc_error(name);
	}
	void *res = malloc((size_t)size);
	if (size > 0 && res == NULL) {
		FractalNow_alloc_error(name);
	}

	return res;
}

void *safeRealloc(const char *name, void *ptr, uint_least64_t size)
{
	if (size > SIZE_MAX) {
		FractalNow_alloc_error(name);
	}
	void *res = realloc(ptr, (size_t)size);
	if (size > 0 && res == NULL) {
		FractalNow_alloc_error(name);
	}

	return res;
}

void *safeCalloc(const char *name, uint_least64_t nmemb, uint_least64_t size)
{
	if (nmemb > SIZE_MAX || size > SIZE_MAX) {
		FractalNow_alloc_error(name);
	}
	void *res = calloc((size_t)nmemb, (size_t)size);
	if (size > 0 && res == NULL) {
		FractalNow_alloc_error(name);
	}

	return res;
}

int mpcIsInteger(const mpc_t x) {
	if (mpfr_cmp_ui(mpc_imagref(x), 0) != 0) {
		return 0;
	} else {
		return mpfr_integer_p(mpc_realref(x));
	}
}

int complexIsInteger(long double complex x) {
	if (cimagl(x) != 0) {
		return 0;
	} else {
		long double fptr, iptr;
		fptr = modfl(creall(x), &iptr);

		return (fptr == 0);
	}
}

#define BUILD_cipow(ftype, suffix) \
/* Function assumes that y >= 1 and computes x^y. */\
ftype complex cipow##suffix(ftype complex x, uint_fast32_t y)\
{\
	ftype complex res;\
\
	if (y == 0) {\
		res = 1;\
	}\
\
	ftype complex rem;\
	rem = 1;\
	res = x;\
	while (y > 1) {\
		if (y % 2) {\
			rem *= res;\
			--y;\
		}\
		y >>= 1;\
		res *= res;\
	}\
\
	res *= rem;\
\
	return res;\
}
BUILD_cipow(float, f)
BUILD_cipow(double, )
BUILD_cipow(long double, l)

