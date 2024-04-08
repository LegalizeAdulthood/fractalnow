/*
 *  fractal_counting_function.h -- part of FractalNow
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
  * \brief Header file related to fractal counting functions.
  * \author Marc Pegon
  */
 
#ifndef __FRACTAL_COUNTING_FUNCTION_H__
#define __FRACTAL_COUNTING_FUNCTION_H__

#ifdef __cplusplus
extern "C" {
#endif

#include "floating_point.h"

struct Fractal;

/**
 * \enum e_CountingFunction
 * \brief Possible counting functions.
 *
 * There are different ways to "count" the number of iterations
 * when computing fractal :\n
 * it can be either simply the number of iterations itself (which
 * is discrete), or more advanced stuff like continuous or smooth
 * iteration count.\n
 */
/**
 * \typedef CountingFunction
 * \brief Convenient typedef for enum CountingFunction.
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
extern const uint_fast32_t nbCountingFunctions;

/**
 * \var countingFunctionStr
 * \brief Strings of counting functions.
 */
extern const char *countingFunctionStr[];

/**
 * \var countingFunctionDescStr
 * \brief More descriptive strings for counting functions.
 */
extern const char *countingFunctionDescStr[];

/**
 * \typedef CountingFunctionPtr
 * \brief Counting function type.
 */
typedef FLOATT (*CountingFunctionPtr)(const struct Fractal *fractal, uint_fast32_t N, FLOATT rN);

/**
 * \fn FLOATT DiscreteIterationCount(const Fractal *fractal, uint_fast32_t N, FLOATT rN)
 * \brief Discrete iteration count.
 *
 * Assumes rN < escapeRadius.
 *
 * \param fractal Fractal.
 * \param N Last iteration number.
 * \param rN Norm of z for last iteration.
 * \return Discrete iteration count.
 */
FLOATT DiscreteIterationCount(const struct Fractal *fractal, uint_fast32_t N, FLOATT rN);

/**
 * \fn FLOATT ContinuousIterationCount(const Fractal *fractal, uint_fast32_t N, FLOATT rN)
 * \brief Continuous iteration count.
 *
 * Assumes rN < escapeRadius.
 *
 * \param fractal Fractal.
 * \param N Last iteration number.
 * \param rN Norm of z for last iteration.
 * \return Continuous iteration count.
 */
FLOATT ContinuousIterationCount(const struct Fractal *fractal, uint_fast32_t N, FLOATT rN);

/**
 * \fn FLOATT SmoothIterationCount(const Fractal *fractal, uint_fast32_t N, FLOATT rN)
 * \brief Smooth iteration count.
 *
 * Assumes rN < escapeRadius.
 *
 * \param fractal Fractal.
 * \param N Last iteration number.
 * \param rN Norm of z for last iteration.
 * \return Smooth iteration count.
 */
FLOATT SmoothIterationCount(const struct Fractal *fractal, uint_fast32_t N, FLOATT rN);

/**
 * \fn int GetCountingFunction(CountingFunction *countingFunction, const char *str)
 * \brief Get counting function from string.
 *
 * Function is case insensitive.\n
 * Possible strings are :
 * - "discrete" for discrete iteration count
 * - "continuous" for continuous iteration count
 * - "smooth" for smooth iteration count
 *
 * \param countingFunction Counting function destination.
 * \param str String specifying counting function.
 * \return 0 in case of success, 1 in case of failure.
 */
int GetCountingFunction(CountingFunction *countingFunction, const char *str);

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
