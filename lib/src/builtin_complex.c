/*
 *  builtin_complex.c -- part of FractalNow
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

#include "builtin_complex.h"

#ifdef BUILD_COMPLEX
#undef BUILD_COMPLEX
#endif

#define BUILD_COMPLEX(type, suffix) \
inline Complex_##suffix cbuild_##suffix(type re, type im)\
{\
	Complex_##suffix res = { re, im };\
\
	return res;\
}\
\
inline int ceq_##suffix(Complex_##suffix a, Complex_##suffix b)\
{\
	return (a.re == b.re) && (a.im == b.im);\
}\
\
inline type creal_##suffix(Complex_##suffix a)\
{\
	return a.re;\
}\
\
inline type cimag_##suffix(Complex_##suffix a)\
{\
	return a.im;\
}\
\
inline Complex_##suffix conj_##suffix(Complex_##suffix a)\
{\
	return cbuild_##suffix(a.re, -a.im);\
}\
\
inline type cnorm_##suffix(Complex_##suffix a)\
{\
	return (a.re*a.re)+(a.im*a.im);\
}\
\
inline type cabs_##suffix(Complex_##suffix a)\
{\
	return sqrt##suffix(cnorm_##suffix(a));\
}\
\
inline Complex_##suffix cadd_##suffix(Complex_##suffix a, Complex_##suffix b)\
{\
	return cbuild_##suffix(a.re+b.re, a.im+b.im);\
}\
\
inline Complex_##suffix csub_##suffix(Complex_##suffix a, Complex_##suffix b)\
{\
	return cbuild_##suffix(a.re-b.re, a.im-b.im);\
}\
\
inline Complex_##suffix csqr_##suffix(Complex_##suffix a)\
{\
	type re2 = a.re*a.re;\
	type im2 = a.im*a.im;\
	type re_x_im = a.re*a.im;\
\
	return cbuild_##suffix(re2-im2, 2*re_x_im);\
}\
\
inline Complex_##suffix cmul_##suffix(Complex_##suffix a, Complex_##suffix b)\
{\
	return cbuild_##suffix(a.re*b.re-a.im*b.im, a.re*b.im+b.re*a.im);\
}\
\
inline Complex_##suffix cdiv_##suffix(Complex_##suffix a, Complex_##suffix b)\
{\
	Complex_##suffix tmp = cmul_##suffix(a, conj_##suffix(b));\
\
	type norm = cnorm_##suffix(b);\
\
	return cbuild_##suffix(tmp.re/norm, tmp.im/norm);\
}\
\
inline type carg_##suffix(Complex_##suffix a)\
{\
	return atan2##suffix(a.im, a.re);\
}\
\
inline Complex_##suffix cexp_##suffix(Complex_##suffix a)\
{\
	type exp_re = exp##suffix(a.re);\
	return cbuild_##suffix(exp_re*cos##suffix(a.im), exp_re*sin##suffix(a.im));\
}\
\
inline Complex_##suffix clog_##suffix(Complex_##suffix a)\
{\
	return cbuild_##suffix(log##suffix(cabs_##suffix(a)), carg_##suffix(a));\
}\
\
inline Complex_##suffix cipow_##suffix(Complex_##suffix x, uint_fast32_t y)\
{\
	Complex_##suffix res;\
\
	if (y == 0) {\
		res = cbuild_##suffix(1, 0);\
	}\
\
	Complex_##suffix rem = cbuild_##suffix(1, 0);\
	res = x;\
	while (y > 1) {\
		if (y % 2) {\
			rem = cmul_##suffix(rem, res);\
			--y;\
		}\
		y >>= 1;\
			res = csqr_##suffix(res);\
	}\
\
	res = cmul_##suffix(res, rem);\
\
	return res;\
}\
\
inline Complex_##suffix cpow_##suffix(Complex_##suffix a, Complex_##suffix b)\
{\
	return cexp_##suffix(cmul_##suffix(b, clog_##suffix(a)));\
}\
\
inline int cisinteger_##suffix(Complex_##suffix x)\
{\
	if (cimag_##suffix(x) != 0) {\
		return 0;\
	} else {\
		type fptr, iptr;\
		fptr = modf##suffix(creal_##suffix(x), &iptr);\
\
		return (fptr == 0);\
	}\
}

BUILD_COMPLEX(float, f)
BUILD_COMPLEX(double, )
BUILD_COMPLEX(long double, l)

#undef BUILD_COMPLEX

