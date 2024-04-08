/*
 *  macro_build_fractals.h -- part of fractal2D
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

#define INTERPOLATION_METHOD(x,y,z,t,u) MACRO_BUILD_FRACTAL(x,y,z,t,u)
#define BUILD_INTERPOLATION_METHODS(x,y,z,t) \
	INTERPOLATION_METHOD(x,y,z,t,NONE) \
	INTERPOLATION_METHOD(x,y,z,t,LINEAR) \
	INTERPOLATION_METHOD(x,y,z,t,SPLINE)
#define ADDEND_FUNCTION(x,y,z,t) BUILD_INTERPOLATION_METHODS(x,y,z,t)
#define BUILD_ADDEND_FUNCTIONS(x,y,z) \
	ADDEND_FUNCTION(x,y,z,TRIANGLEINEQUALITY)
#define COLORING_METHOD(x,y,z) BUILD_ADDEND_FUNCTIONS(x,y,z)
#define BUILD_COLORING_METHODS(x,y) \
	COLORING_METHOD(x,y,SIMPLE) \
	COLORING_METHOD(x,y,AVERAGE)
#define P_TYPE(x,y) BUILD_COLORING_METHODS(x,y)
#define BUILD_P_TYPES(x) \
	P_TYPE(x,PINT) \
	P_TYPE(x,PFLOAT)
#define FORMULA(x) BUILD_P_TYPES(x)
#define BUILD_FORMULAS \
	FORMULA(MANDELBROT) \
	FORMULA(MANDELBROTP) \
	FORMULA(JULIA) \
	FORMULA(JULIAP) \
	FORMULA(RUDY)
#define MACRO_BUILD_FRACTALS BUILD_FORMULAS

#endif

