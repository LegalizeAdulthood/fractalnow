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
	#define PRIFLOAT ".*G"
	#define SCNFLOAT "f"
	#define FLOAT_DIG FLT_DIG
	#define fabsF(x) fabsf(x)
	#define cabsF(z) cabsf(z)
	#define crealF(z) crealf(z)
	#define cimagF(z) cimagf(z)
	#define cpowF(z,y) cpowf(z,y)
	#define roundF(x) roundf(x)
	#define floorF(x) floorf(x)
	#define powF(x,y) powf(x,y)
	#define fmaxF(x,y) fmaxf(x,y)
	#define modfF(x,y) modff(x,y)
	#define logF(x) logf(x)
	#define sqrtF(x) sqrtf(x)
	#define LOGF logf
	#define SQRTF sqrtf
	#define EXPF expf
#elif FLOAT_PRECISION == 2
	#define FLOAT long double
	#define PRIFLOAT ".*LG"
	#define SCNFLOAT "Lf"
	#define FLOAT_DIG LDBL_DIG
	#define fabsF(x) fabsl(x)
	#define cabsF(z) cabsl(z)
	#define crealF(z) creall(z)
	#define cimagF(z) cimagl(z)
	#define cpowF(z,y) cpowl(z,y)
	#define roundF(x) roundl(x)
	#define floorF(x) floorl(x)
	#define powF(x,y) powl(x,y)
	#define fmaxF(x,y) fmaxl(x,y)
	#define modfF(x,y) modfl(x,y)
	#define logF(x) logl(x)
	#define sqrtF(x) sqrtl(x)
	#define LOGF logl
	#define SQRTF sqrtl
	#define EXPF expl
#else
	#define FLOAT double
	#define PRIFLOAT ".*lG"
	#define SCNFLOAT "lf"
	#define FLOAT_DIG DBL_DIG
	#define fabsF(x) fabs(x)
	#define cabsF(z) cabs(z)
	#define crealF(z) creal(z)
	#define cimagF(z) cimag(z)
	#define cpowF(z,y) cpow(z,y)
	#define roundF(x) round(x)
	#define floorF(x) floor(x)
	#define powF(x,y) pow(x,y)
	#define fmaxF(x,y) fmax(x,y)
	#define modfF(x,y) modf(x,y)
	#define logF(x) log(x)
	#define sqrtF(x) sqrt(x)
	#define LOGF log
	#define SQRTF sqrt
	#define EXPF exp
#endif

#endif
