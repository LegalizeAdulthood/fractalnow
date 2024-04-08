/*
 *  fractal_iteration_count.h -- part of FractalNow
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
  * \file fractal_iteration_count.h
  * \brief Header file related to fractal iteration count.
  * \author Marc Pegon
  */
 
#ifndef __FRACTAL_ITERATION_COUNT_H__
#define __FRACTAL_ITERATION_COUNT_H__

#include "float_precision.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * \enum e_IterationCount
 * \brief Possible iteration counts.
 *
 * There are different ways to "count" the number of iterations
 * when computing fractal :\n
 * it can be either simply the number of iterations itself (which
 * is discrete), or more advanced stuff like continuous or smooth
 * iteration count.\n
 */
/**
 * \typedef IterationCount
 * \brief Convenient typedef for enum e_IterationCount.
 */
typedef enum e_IterationCount {
	IC_DISCRETE = 0,
 /*<! Discrete iteration count. */
	IC_CONTINUOUS,
 /*<! Continuous iteration count. */
	IC_SMOOTH
 /*<! Smooth iteration count. */
} IterationCount;

/**
 * \var nbIterationCounts
 * \brief Number of iteration counts.
 */
extern const uint_fast32_t nbIterationCounts;

/**
 * \var iterationCountStr
 * \brief Strings of iteration counts.
 */
extern const char *iterationCountStr[];

/**
 * \var iterationCountDescStr
 * \brief More descriptive strings for iteration counts.
 */
extern const char *iterationCountDescStr[];

/**
 * \fn int GetIterationCount(IterationCount *iterationCount, const char *str)
 * \brief Get iteration count from string.
 *
 * Function is case insensitive.\n
 * Possible strings are :
 * - "discrete" for discrete iteration count
 * - "continuous" for continuous iteration count
 * - "smooth" for smooth iteration count
 *
 * \param iterationCount Iteration count destination.
 * \param str String specifying counting function.
 * \return 0 in case of success, 1 in case of failure.
 */
int GetIterationCount(IterationCount *iterationCount, const char *str);

/***********************IC_DISCRETE***********************/
#define ENGINE_DECL_VAR_IC_DISCRETE(fprec)

#define ENGINE_INIT_VAR_IC_DISCRETE(fprec) \
(void)NULL;

#define ENGINE_CLEAR_VAR_IC_DISCRETE(fprec) \
(void)NULL;

#define DECL_INIT_VAR_IC_DISCRETE(fprec) \
(void)NULL;

#define CLEAR_VAR_IC_DISCRETE(fprec) \
(void)NULL;

#define COMPUTE_IC_DISCRETE(fprec) \
fromUiF(fprec, data->res, data->n);
/*********************************************************/

/**********************IC_CONTINUOUS**********************/
#define ENGINE_DECL_VAR_IC_CONTINUOUS(fprec) \
FLOATTYPE(fprec) escapeRadiusP_IC;\
FLOATTYPE(fprec) absZ_IC;\
FLOATTYPE(fprec) diff0_IC;\
FLOATTYPE(fprec) diff1_IC;

#define ENGINE_INIT_VAR_IC_CONTINUOUS(fprec) \
initF(fprec, data->escapeRadiusP_IC);\
cabsF(fprec, data->escapeRadiusP_IC, data->fractalP);\
powF(fprec, data->escapeRadiusP_IC, data->escapeRadius, data->escapeRadiusP_IC);\
initF(fprec, data->absZ_IC);\
initF(fprec, data->diff0_IC);\
initF(fprec, data->diff1_IC);

#define ENGINE_CLEAR_VAR_IC_CONTINUOUS(fprec) \
clearF(fprec, data->escapeRadiusP_IC);\
clearF(fprec, data->absZ_IC);\
clearF(fprec, data->diff0_IC);\
clearF(fprec, data->diff1_IC);

#define COMPUTE_IC_CONTINUOUS(fprec) \
sqrtF(fprec, data->absZ_IC, data->normZ);\
subF(fprec, data->diff0_IC, data->escapeRadiusP_IC, data->escapeRadius);\
subF(fprec, data->diff1_IC, data->escapeRadiusP_IC, data->absZ_IC);\
divF(fprec, data->diff0_IC, data->diff1_IC, data->diff0_IC);\
add_uiF(fprec, data->res, data->diff0_IC, data->n);
/*********************************************************/

/************************IC_SMOOTH************************/
#define ENGINE_DECL_VAR_IC_SMOOTH(fprec) \
FLOATTYPE(fprec) logAbsZ_IC;\
FLOATTYPE(fprec) logEscapeRadius_IC;\
FLOATTYPE(fprec) logAbsP_IC;

#define ENGINE_INIT_VAR_IC_SMOOTH(fprec) \
initF(fprec, data->logAbsZ_IC);\
initF(fprec, data->logEscapeRadius_IC);\
logF(fprec, data->logEscapeRadius_IC, data->escapeRadius);\
initF(fprec, data->logAbsP_IC);\
cabsF(fprec, data->logAbsP_IC, data->fractalP);\
logF(fprec, data->logAbsP_IC, data->logAbsP_IC);

#define ENGINE_CLEAR_VAR_IC_SMOOTH(fprec) \
clearF(fprec, data->logAbsZ_IC);\
clearF(fprec, data->logEscapeRadius_IC);\
clearF(fprec, data->logAbsP_IC);

#define COMPUTE_IC_SMOOTH(fprec) \
logF(fprec, data->logAbsZ_IC, data->normZ);\
div_uiF(fprec, data->logAbsZ_IC, data->logAbsZ_IC, 2);\
divF(fprec, data->res, data->logEscapeRadius_IC, data->logAbsZ_IC);\
logF(fprec, data->res, data->res);\
divF(fprec, data->res, data->res, data->logAbsP_IC);\
add_uiF(fprec, data->res, data->res, data->n+1);
/*********************************************************/

#ifdef __cplusplus
}
#endif

#endif
