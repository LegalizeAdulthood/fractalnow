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

/**
 * \enum e_ColoringMethod
 * \brief Possible coloring methods.
 *
 * For simple coloring method, fractal value (before applying transfer function
 * and multiplier) is simply the result of iteration count function.
 * For average sums coloring method, fractal value is computed by interpolation
 * function thanks to addend function and iteration count function.
 *
 * See addend function and interation count headers for more details.
 * \see fractal_addend_function.h
 * \see fractal_counting_function.h
 */
typedef enum e_ColoringMethod {
	CM_SIMPLE = 0,
 /*<! Simple coloring method.*/
	CM_AVERAGE
 /*<! Average sums coloring method.*/
} ColoringMethod;
ColoringMethod GetColoringMethod(const char *str);

/**
 * \enum e_InterpolationMethod
 * \brief Possible coloring methods.
 *
 * Interpolation methods use an addend function to compute a number of average
 * sums, from which they compute de fractal value.
 * For no interpolation, only 1 sum is to be computed, and is returned as is.
 * For linear and spline interpolation, respectively 2 and 4 sums are to be
 * computed by addend function, and the final value is computed from those
 * sums using the iteration count function.
 *
 * See addend function and interation count headers for more details.
 * \see fractal_addend_function.h
 * \see fractal_counting_function.h
 */
typedef enum e_InterpolationMethod {
	IM_NONE = 0,
 /*<! No interpolation (only one average sum).*/
	IM_LINEAR,
 /*<! Linear interpolation (two average sums), using fractal computing algorithm (smooth iteration for example).*/
	IM_SPLINE
 /*<! Spline interpolation (four average sums), using fractal computing algorithm.*/
} InterpolationMethod;

/**
 * \fn InterpolationMethod GetInterpolationMethod(const char *str) * \brief Get interpolation method from string.
 *
 * Function is case insensitive.
 * Possible strings are :
 * - "none" for no interpolation (only one average sum)
 * - "linear" for linear interpolation (two average sums)
 * - "spline" for spline interpolation (five average sums)
 * Exit with error in case of failure.
 *
 * \param str String specifying interpolation function.
 * \return Corresponding interpolation method.
 */
InterpolationMethod GetInterpolationMethod(const char *str);

/* Function assumes that y >= 1 and compute x^y. */
static inline FLOAT iPowF(FLOAT x, uint_fast32_t y)
{
	FLOAT rem = 1, res = x;
	do {
		if (y % 2) {
			rem *= x;
			--y;
		}
		y >>= 1;
		res *= res;
	} while (y > 1);

	return res*rem;
}

/*************************IM_NONE*************************/
#define LOOP_INIT_IM_NONE(addend) \
FLOAT SN[1];\
LOOP_INIT_AF_##addend(1)

#define LOOP_ITERATION_IM_NONE(addend) \
LOOP_ITERATION_AF_##addend(1)

#define LOOP_END_IM_NONE(addend) \
LOOP_END_AF_##addend(1)\
res = SN[0];
/*********************************************************/


/************************IM_LINEAR************************/
#define LOOP_INIT_IM_LINEAR(addend) \
FLOAT SN[2];\
LOOP_INIT_AF_##addend(2)

#define LOOP_ITERATION_IM_LINEAR(addend) \
LOOP_ITERATION_AF_##addend(2)

#define LOOP_END_IM_LINEAR(addend) \
LOOP_END_AF_##addend(2)\
res = countingFunction(fractal, n, sqrtF(normZ2));\
FLOAT d, unused;\
d = modfF(res, &unused);\
res = d*SN[0] + (1-d)*SN[1];
/*********************************************************/


/************************IM_SPLINE************************/
#define LOOP_INIT_IM_SPLINE(addend) \
FLOAT SN[4];\
LOOP_INIT_AF_##addend(4)

#define LOOP_ITERATION_IM_SPLINE(addend) \
LOOP_ITERATION_AF_##addend(4)

#define LOOP_END_IM_SPLINE(addend) \
LOOP_END_AF_##addend(4)\
res = countingFunction(fractal, n, sqrtF(normZ2));\
FLOAT unused, d1, d2, d3;\
d1 = modfF(res, &unused);\
d2 = d1*d1;\
d3 = d1*d2;\
res = ((-d2+d3)*SN[0] +\
	(d1+4*d2-3*d3)*SN[1] +\
	(2-5*d2+3*d3)*SN[2] +\
	(-d1+2*d2-d3)*SN[3]) /2;\
/*********************************************************/


/************************CM_SIMPLE************************/
#define LOOP_INIT_CM_SIMPLE(addend,interpolation) \
(void)NULL;

#define LOOP_ITERATION_CM_SIMPLE(addend,interpolation) \
(void)NULL;

#define LOOP_END_CM_SIMPLE(addend,interpolation) \
res = countingFunction(fractal, n, sqrtF(normZ2));
/*********************************************************/


/************************CM_AVERAGE***********************/
#define LOOP_INIT_CM_AVERAGE(addend,interpolation) \
LOOP_INIT_IM_##interpolation(addend)

#define LOOP_ITERATION_CM_AVERAGE(addend,interpolation) \
LOOP_ITERATION_IM_##interpolation(addend)

#define LOOP_END_CM_AVERAGE(addend,interpolation) \
LOOP_END_IM_##interpolation(addend)
/*********************************************************/

#endif

