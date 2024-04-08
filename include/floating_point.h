/*
 *  floating_point.h -- part of fractal2D
 *
 *  Copyright (c) 2011 Marc Pegon <pe.marc@free.fr>
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
  * \file floating_point.h
  * \brief Header file defining FLOAT-related constants.
  * \author Marc Pegon
  */
 
#ifndef __FLOATING_POINT_H__
#define __FLOATING_POINT_H__

#include <float.h>
#include <complex.h>
#include <math.h>

/**
 * \def FLOAT_PRECISION
 * \brief Specifies the precision of floating point numbers at compile time.
 *
 * \see FLOAT for more details.
 */
#ifndef FLOAT_PRECISION
	#define FLOAT_PRECISION 1
#endif

/**
 * \def FLOAT
 * \brief Floating point number type.
 *
 * Depending of FLOAT_PRECISION, FLOAT will be define either as float(0),
 * double(1), or long double(2).
 */
#if FLOAT_PRECISION == 0
	#define FLOAT float
	#define PRINTF_FLOAT_TYPE_SPECIFIER "%.*G"
	#define SCANF_FLOAT_TYPE_SPECIFIER "%f"
	#define FLOAT_DIG FLT_DIG
	#define strtoF(nptr,endptr) strtof(nptr,endptr)
	#define crealF(z) crealf(z)
	#define cimagF(z) cimagf(z)
	#define roundF(x) roundf(x)
	#define fabsF(x) fabsf(x)
	#define frexpF(x,exp) frexpf(x,exp)
	#define ldexpF(x,exp) ldexpf(x,exp)
	#define powF(x,y) powf(x,y)
#elif FLOAT_PRECISION == 2
	#define FLOAT long double
	#define PRINTF_FLOAT_TYPE_SPECIFIER "%.*LG"
	#define SCANF_FLOAT_TYPE_SPECIFIER "%Lf"
	#define FLOAT_DIG LDBL_DIG
	#define strtoF(nptr,endptr) strtold(nptr,endptr)
	#define crealF(z) creall(z)
	#define cimagF(z) cimagl(z)
	#define roundF(x) roundl(x)
	#define fabsF(x) fabsl(x)
	#define frexpF(x,exp) frexpl(x,exp)
	#define ldexpF(x,exp) ldexpl(x,exp)
	#define powF(x,y) powl(x,y)
#else
	#define FLOAT double
	#define PRINTF_FLOAT_TYPE_SPECIFIER "%.*lG"
	#define SCANF_FLOAT_TYPE_SPECIFIER "%lf"
	#define FLOAT_DIG DBL_DIG
	#define strtoF(nptr,endptr) strtod(nptr,endptr)
	#define crealF(z) creal(z)
	#define cimagF(z) cimag(z)
	#define roundF(x) round(x)
	#define fabsF(x) fabs(x)
	#define frexpF(x,exp) frexp(x,exp)
	#define ldexpF(x,exp) ldexp(x,exp)
	#define powF(x,y) pow(x,y)
#endif

#endif
