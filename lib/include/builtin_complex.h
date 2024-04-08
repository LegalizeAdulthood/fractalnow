/*
 *  builtin_complex.h -- part of FractalNow
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

 /**
  * \file builtin_complex.h
  * \brief Header file for builtin complex numbers.
  * \author Marc Pegon
  */
 
#ifndef __BUILTIN_COMPLEX_H__
#define __BUILTIN_COMPLEX_H__

#include <stdint.h>
#include <math.h>

#ifdef __cplusplus
extern "C" {
#endif

#ifdef BUILD_COMPLEX
#undef BUILD_COMPLEX
#endif

#define BUILD_COMPLEX(type, suffix) \
typedef struct Complex_##suffix {\
	type re;\
	type im;\
} Complex_##suffix;\
\
Complex_##suffix cbuild_##suffix(type re, type im);\
int ceq_##suffix(Complex_##suffix a, Complex_##suffix b);\
type creal_##suffix(Complex_##suffix a);\
type cimag_##suffix(Complex_##suffix a);\
Complex_##suffix conj_##suffix(Complex_##suffix a);\
type cnorm_##suffix(Complex_##suffix a);\
type cabs_##suffix(Complex_##suffix a);\
Complex_##suffix cadd_##suffix(Complex_##suffix a, Complex_##suffix b);\
Complex_##suffix csub_##suffix(Complex_##suffix a, Complex_##suffix b);\
Complex_##suffix csqr_##suffix(Complex_##suffix a);\
Complex_##suffix cmul_##suffix(Complex_##suffix a, Complex_##suffix b);\
Complex_##suffix cdiv_##suffix(Complex_##suffix a, Complex_##suffix b);\
type carg_##suffix(Complex_##suffix a);\
Complex_##suffix cexp_##suffix(Complex_##suffix a);\
Complex_##suffix clog_##suffix(Complex_##suffix a);\
Complex_##suffix cipow_##suffix(Complex_##suffix x, uint_fast32_t y);\
Complex_##suffix cpow_##suffix(Complex_##suffix a, Complex_##suffix b);\
int cisinteger_##suffix(Complex_##suffix x);

BUILD_COMPLEX(float, f)
BUILD_COMPLEX(double, )
BUILD_COMPLEX(long double, l)
#undef BUILD_COMPLEX

#ifdef __cplusplus
}
#endif

#endif
