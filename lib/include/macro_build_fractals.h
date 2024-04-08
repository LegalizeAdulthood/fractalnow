/*
 *  macro_build_fractals.h -- part of FractalNow
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
  * \file macro_build_fractals.h
  * \brief Header file related to macros for building fractal loop functions.
  *
  * \author Marc Pegon
  */

#ifndef __MACRO_BUILD_FRACTALS_H__
#define __MACRO_BUILD_FRACTALS_H__

#ifdef __cplusplus
extern "C" {
#endif

#define FLOAT_PRECISIONS(x,y,z,t,u,v,w) MACRO_BUILD_FRACTAL(x,y,z,t,u,v,w)

#if defined(_ENABLE_MP_FLOATS) && defined(_ENABLE_LDOUBLE_FLOATS)
#define BUILD_FLOAT_PRECISIONS(x,y,z,t,u,v) \
	FLOAT_PRECISIONS(x,y,z,t,u,v,SINGLE) \
	FLOAT_PRECISIONS(x,y,z,t,u,v,DOUBLE) \
	FLOAT_PRECISIONS(x,y,z,t,u,v,LDOUBLE) \
	FLOAT_PRECISIONS(x,y,z,t,u,v,MP)
#elif defined(_ENABLE_MP_FLOATS) && !defined(_ENABLE_LDOUBLE_FLOATS)
#define BUILD_FLOAT_PRECISIONS(x,y,z,t,u,v) \
	FLOAT_PRECISIONS(x,y,z,t,u,v,SINGLE) \
	FLOAT_PRECISIONS(x,y,z,t,u,v,DOUBLE) \
	FLOAT_PRECISIONS(x,y,z,t,u,v,MP)
#elif !defined(_ENABLE_MP_FLOATS) && defined(_ENABLE_LDOUBLE_FLOATS)
#define BUILD_FLOAT_PRECISIONS(x,y,z,t,u,v) \
	FLOAT_PRECISIONS(x,y,z,t,u,v,SINGLE) \
	FLOAT_PRECISIONS(x,y,z,t,u,v,LDOUBLE) \
	FLOAT_PRECISIONS(x,y,z,t,u,v,DOUBLE)
#else
#define BUILD_FLOAT_PRECISIONS(x,y,z,t,u,v) \
	FLOAT_PRECISIONS(x,y,z,t,u,v,SINGLE) \
	FLOAT_PRECISIONS(x,y,z,t,u,v,DOUBLE)
#endif

#define INTERPOLATION_METHOD(x,y,z,t,u,v) BUILD_FLOAT_PRECISIONS(x,y,z,t,u,v)
#define BUILD_INTERPOLATION_METHODS(x,y,z,t,u) \
	INTERPOLATION_METHOD(x,y,z,t,u,NONE) \
	INTERPOLATION_METHOD(x,y,z,t,u,LINEAR) \
	INTERPOLATION_METHOD(x,y,z,t,u,SPLINE)
#define ADDEND_FUNCTION(x,y,z,t,u) BUILD_INTERPOLATION_METHODS(x,y,z,t,u)
#define BUILD_ADDEND_FUNCTIONS(x,y,z,t) \
	ADDEND_FUNCTION(x,y,z,t,TRIANGLEINEQUALITY) \
	ADDEND_FUNCTION(x,y,z,t,CURVATURE) \
	ADDEND_FUNCTION(x,y,z,t,STRIPE)
#define ITERATION_COUNT(x,y,z,t) BUILD_ADDEND_FUNCTIONS(x,y,z,t)
#define BUILD_ITERATION_COUNTS(x,y,z) \
	ITERATION_COUNT(x,y,z,DISCRETE)\
	ITERATION_COUNT(x,y,z,CONTINUOUS)\
	ITERATION_COUNT(x,y,z,SMOOTH)
#define COLORING_METHOD(x,y,z) BUILD_ITERATION_COUNTS(x,y,z)
#define BUILD_COLORING_METHODS(x,y) \
	COLORING_METHOD(x,y,ITERATIONCOUNT) \
	COLORING_METHOD(x,y,AVERAGECOLORING)
#define P_TYPE(x,y) BUILD_COLORING_METHODS(x,y)
#define BUILD_P_TYPES(x) \
	P_TYPE(x,PINT) \
	P_TYPE(x,PFLOATT)
#define FORMULA(x) BUILD_P_TYPES(x)
#define BUILD_FORMULAS \
	FORMULA(MANDELBROT)\
	FORMULA(MULTIBROT)\
	FORMULA(JULIA)\
	FORMULA(MULTIJULIA)\
	FORMULA(BURNINGSHIP)\
	FORMULA(JULIABURNINGSHIP)\
	FORMULA(MANDELBAR)\
 	FORMULA(JULIABAR)\
	FORMULA(RUDY)
#define MACRO_BUILD_FRACTALS BUILD_FORMULAS

#ifdef __cplusplus
}
#endif

#endif

