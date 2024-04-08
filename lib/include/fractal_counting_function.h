/*
 *  fractal_counting_function.h -- part of fractal2D
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
  * \file fractal_counting_function.h
  * \brief Header file related to iteration count method.
  * \author Marc Pegon
  */
 
#ifndef __FRACTAL_COUNTING_FUNCTION_H__
#define __FRACTAL_COUNTING_FUNCTION_H__

#ifdef __cplusplus
extern "C" {
#endif

#include "floating_point.h"

struct s_Fractal;

/**
 * \typedef CountingFunction
 * \brief Possible counting functions.
 *
 * There are different ways to "count" the number of iterations
 * when computing fractal :
 * it can be either simply the number of iterations itself (which
 * is discrete), or more advanced stuff like continuous or smooth
 * iteration count.
 * Some are not even about counting but estimating distance to
 * fractal space...
 * 
 */
typedef enum e_CountingFunction {
	CF_DISCRETE = 0,
 /*<! Discrete iteration count. */
	CF_CONTINUOUS,
 /*<! Continuous iteration count. */
	CF_SMOOTH
 /*<! Smooth iteration count. */
} CountingFunction;

/**
 * \var nbCountingFunctions
 * \brief Number of counting functions.
 */
extern uint_fast32_t nbCountingFunctions;

/**
 * \var CountingFunctionStr
 * \brief Strings of counting functions.
 */
extern const char *CountingFunctionStr[];

/**
 * \var CountingFunctionDescStr
 * \brief More descriptive strings for counting functions.
 */
extern const char *CountingFunctionDescStr[];

/**
 * \typedef CountingFunctionPtr
 * \brief Counting function type.
 */
typedef FLOAT (*CountingFunctionPtr)(struct s_Fractal *fractal, uint_fast32_t N, FLOAT rN);

/**
 * \fn CountingFunction GetCountingFunction(const char *str)
 * \brief Get counting function from string.
 *
 * Function is case insensitive.
 * Possible strings are :
 * - "discrete" for discrete iteration count
 * - "continuous" for continuous iteration count
 * - "smooth" for smooth iteration count
 * Exit with error in case of failure.
 *
 * \param str String specifying counting function.
 * \return Corresponding counting function.
 */
CountingFunction GetCountingFunction(const char *str);

/**
 * \fn CountingFunctionPtr GetCountingFunctionPtr(CountingFunction countingFunction)
 * \brief Get counting function ptr from counting function enum value.
 *
 * \param countingFunction Counting function enum value.
 * \return Corresponding counting function ptr.
 */
CountingFunctionPtr GetCountingFunctionPtr(CountingFunction countingFunction);

#ifdef __cplusplus
}
#endif

#endif
