/*
 *  c99_complex_wrapper.c -- part of FractalNow
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

#include "c99_complex_wrapper.h"
#include <math.h>

#ifdef BUILD_CIPOW
#undef BUILD_CIPOW
#endif

#define BUILD_CIPOW(type, suffix) \
type complex cipow##suffix(type complex x, uint_fast32_t y)\
{\
	type complex res;\
\
	if (y == 0) {\
		res = 1;;\
	}\
\
	type complex rem = 1;\
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
}\
\

BUILD_CIPOW(float, f)
BUILD_CIPOW(double, )
BUILD_CIPOW(long double, l)
#undef BUILD_CIPOW

#ifdef BUILD_CNORM
#undef BUILD_CNORM
#endif

#define BUILD_CNORM(type, suffix) \
type cnorm##suffix(type complex x)\
{\
	return creal##suffix(x)*creal##suffix(x)+cimag##suffix(x)*cimag##suffix(x);\
}\
\

BUILD_CNORM(float, f)
BUILD_CNORM(double, )
BUILD_CNORM(long double, l)
#undef BUILD_CNORM

#ifdef BUILD_CISINTEGER
#undef BUILD_CISINTEGER
#endif

#define BUILD_CISINTEGER(type, suffix) \
int cisinteger##suffix(type complex x)\
{\
	if (cimag##suffix(x) != 0) {\
		return 0;\
	} else {\
		type fptr, iptr;\
		fptr = modf##suffix(creal##suffix(x), &iptr);\
\
		return (fptr == 0);\
	}\
}

BUILD_CISINTEGER(float, f)
BUILD_CISINTEGER(double, )
BUILD_CISINTEGER(long double, l)
#undef BUILD_CISINTEGER

