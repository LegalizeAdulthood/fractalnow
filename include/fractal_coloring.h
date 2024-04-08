/*
 *  fractal_coloring.h -- part of fractal2D
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
  * \file fractal_coloring.h
  * \brief Header file related to fractal coloring.
  *
  * Contains coloring methods and interpolation.
  *
  * \author Marc Pegon
  */
 
#ifndef __FRACTAL_COLORING_H__
#define __FRACTAL_COLORING_H__

#include "floating_point.h"
#include <stdint.h>

struct s_Fractal;
struct s_FractalOrbit;
struct s_RenderingParameters;

/**
 * \enum e_ColoringMethod
 * \brief Possible coloring methods.
 *
 * For simple coloring method, fractal value (before applying transfer function
 * and multiplier) is simply the result of iteration count function.
 * For average sums coloring method, fractal value is computed by interpolation
 * function thanks to addend function and iteration count function.
 */
typedef enum e_ColoringMethod {
	CM_SIMPLE = 0,
 /*<! Simple coloring method.*/
	CM_AVERAGE
 /*<! Average sums coloring method.*/
} ColoringMethod;
ColoringMethod GetColoringMethod(const char *str);


/**
 * \typedef InterpolationFunction
 * \brief Interpolation function type.
 *
 * Compute fractal value from orbit and iteration count, only when coloring method
 * is average sums (CM_AVERAGE).
 */
typedef FLOAT (*InterpolationFunction)(FLOAT iterationCount, struct s_FractalOrbit *orbit,
					struct s_RenderingParameters *render);

/**
 * \fn InterpolationFunction GetInterpolationFunction(const char *str)
 * \brief Get interpolation function from string.
 *
 * Function is case insensitive.
 * Possible strings are :
 * - "none" for no interpolation (only one average sum)
 * - "linear" for linear interpolation (two average sums)
 * - "spline" for spline interpolation (five average sums)
 * Exit with error in case of failure.
 *
 * \param str String specifying interpolation function.
 * \return Corresponding interpolation function.
 */
InterpolationFunction GetInterpolationFunction(const char *str);

#endif

