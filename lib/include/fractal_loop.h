/*
 *  fractal_loop.h -- part of fractal2D
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
  * \file fractal_loop.h
  * \brief Header file related to fractal loops.
  * \author Marc Pegon
  */
 
#ifndef __FRACTAL_LOOP_H__
#define __FRACTAL_LOOP_H__

#ifdef __cplusplus
extern "C" {
#endif

#include "fractal.h"
#include "fractal_coloring.h"
#include "floating_point.h"

struct s_RenderingParameters;

/**
 * \typedef FractalLoop
 * \brief Fractal loop function.
 *
 * Fractal loops are functions to compute the sequence of complex z0, z1, z2, ...
 * and do the coloring.
 * Depending on coloring method and other parameters, we do make the same calculations
 * inside the loop. That's why there are different fractal loops, for each case
 * (even for same fractal types).
 * The reason for all this is that branching inside the loop (cascading ifs) is slow, so
 * we generate different loops.
 *
 * Fractal loop functions are automatically created by C preprocessor for all the possible
 * types of p, coloring methods, addend functions and interpolation methods.
 * Here is (in pseudo-code) the code executed by the fractal loop :
 *	LOOP_INIT_FRAC_##formula
 *	LOOP_INIT_CM_##coloring(addend_function, interpolation_method)
 *	n = 1;
 *	while (n < maxIter && |z| < escapeRadius) {
 *		LOOP_ITERATION_FRAC_##formula(p_type)
 *		LOOP_ITERATION_CM_##coloring(addend_function, interpolation_method)
 *	}
 *	--n;
 *	if (|z| < escapeRadius) {
 *		return -1;
 *	} else {
 *		LOOP_END_CM_##coloring(addend_function, interpolation_method)
 *	}
 *
 * Coloring methods, addend functions and interpolation methods can use some already defined variables :
 * - z0 : initial value for z
 * - pixel : complex number (point of space) corresponding to pixel to be computed
 * - n : current iteration number
 * - z : current complex number
 * - normZ2 : |z]^2
 * - c : complex constant added by formula at each iteration
 * - normC : |c|
 * - countingFunction : counting function given by rendering parameters
 *
 * See coloring method, addend function and interpolation method for more details.
 * \see fractal_coloring_method.h
 * \see fractal_addend_function.h
 * \see fractal_interpolation_method.h
 */
typedef FLOAT (*FractalLoop)(const Fractal *fractal, const struct s_RenderingParameters *render,
				FLOAT complex pixel);

/**
 * \fn FractalLoop GetFractalLoop(FractalFormula fractalFormula, int p_is_integer, ColoringMethod coloringMethod, AddendFunction addendFunction, InterpolationMethod interpolationMethod)
 * \brief Get fractal loop function from fractal type, type of p, and coloring parameters.
 *
 * \param fractalFormula Fractal formula.
 * \param p_is_integer 0 or 1 depending on whether p fractal parameter i 0 or 1.
 * \param coloringMethod Coloring method.
 * \param addendFunction Addend function.
 * \param interpolationMethod Interpolation Method.
 * \return Corresponding fractal loop function.
 */
FractalLoop GetFractalLoop(FractalFormula fractalFormula, int p_is_integer, ColoringMethod coloringMethod,
				AddendFunction addendFunction, InterpolationMethod interpolationMethod);

#ifdef __cplusplus
}
#endif

#endif

