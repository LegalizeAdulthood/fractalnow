/*
 *  fractal_loop.c -- part of fractal2D
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

#include "fractal_loop.h"
#include "fractal_formula.h"
#include "fractal_addend_function.h"
#include "fractal_formula.h"
#include "macro_build_fractals.h"

/* Build Fractal loop functions. */
static inline FLOAT cnorm2F(FLOAT complex z) {
	FLOAT real = crealF(z);
	FLOAT imag = cimagF(z);
	
	return (real*real+imag*imag);
}

#define BUILD_FRACTAL_LOOP_FUNCTION(formula,ptype,coloring,addend,interpolation) \
FLOAT FractalLoop##formula##ptype##coloring##addend##interpolation( \
	Fractal *fractal, RenderingParameters *render, FLOAT complex pixel) \
{\
	FLOAT complex z, c;\
	FLOAT res = 0;\
	CountingFunction countingFunction = render->countingFunction;\
	(void)countingFunction;\
	LOOP_INIT_FRAC_##formula\
	LOOP_INIT_CM_##coloring(addend,interpolation)\
	FLOAT normC = cabsF(c);\
	(void)normC;\
	FLOAT normZ2 = cnorm2F(z);\
	uint_fast32_t n=1;\
	for (; n<fractal->maxIter && normZ2 < fractal->escapeRadius2; n++) {\
		LOOP_ITERATION_FRAC_##formula(ptype)\
		normZ2 = cnorm2F(z);\
		LOOP_ITERATION_CM_##coloring(addend,interpolation)\
	}\
	--n;\
	if (normZ2 < fractal->escapeRadius) {\
		return -1;\
	} else {\
		LOOP_END_CM_##coloring(addend,interpolation)\
	}\
	return res;\
}

#undef MACRO_BUILD_FRACTAL
#define MACRO_BUILD_FRACTAL(formula,ptype,coloring,addend,interpolation) \
	BUILD_FRACTAL_LOOP_FUNCTION(formula,ptype,coloring,addend,interpolation)
MACRO_BUILD_FRACTALS


/* Build GetFractalLoop function. */
#define VAL_PINT 1
#define VAL_PFLOAT 0

#define BUILD_GetFractalLoop(formula,ptype,coloring,addend,interpolation) \
if (fractalFormula == FRAC_##formula && p_is_integer == VAL_##ptype && \
	coloringMethod == CM_##coloring && addendFunction == AF_##addend && \
	interpolationMethod == IM_##interpolation) {\
	return FractalLoop##formula##ptype##coloring##addend##interpolation;\
}

#undef MACRO_BUILD_FRACTAL
#define MACRO_BUILD_FRACTAL(formula,ptype,coloring,addend,interpolation) \
	BUILD_GetFractalLoop(formula,ptype,coloring,addend,interpolation)

FractalLoop GetFractalLoop(FractalFormula fractalFormula, int p_is_integer, ColoringMethod coloringMethod,
				AddendFunction addendFunction, InterpolationMethod interpolationMethod)
{
	MACRO_BUILD_FRACTALS

	error("Could not get fractal loop function from parameters.\n");

	return NULL;
}

#undef VAL_PINT
#undef VAL_PFLOAT

