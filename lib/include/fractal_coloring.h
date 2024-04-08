/*
 *  fractal_coloring.h -- part of FractalNow
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

#include "float_precision.h"
#include "fractal_addend_function.h"
#include "fractal_iteration_count.h"
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * \enum e_ColoringMethod
 * \brief Possible coloring methods.
 *
 * For iteration count coloring method, fractal value (before applying transfer
 * function and multiplier) is simply the result of iteration count function.\n
 * For average sums coloring method, fractal value is computed by interpolation
 * function thanks to addend function and smooth iteration count.
 *
 * See addend function and interation count headers for more details.
 * \see fractal_addend_function.h
 * \see fractal_counting_function.h
 */
/**
 * \typedef ColoringMethod
 * \brief Convenient typedef for enum ColoringMethod.
 */
typedef enum e_ColoringMethod {
	CM_ITERATIONCOUNT = 0,
 /*<! Simple coloring method.*/
	CM_AVERAGECOLORING
 /*<! Average sums coloring method.*/
} ColoringMethod;

/**
 * \var nbColoringMethods
 * \brief Number of coloring methods.
 */
extern const uint_fast32_t nbColoringMethods;

/**
 * \var coloringMethodStr
 * \brief Strings of coloring method enum values.
 */
extern const char *coloringMethodStr[];

/**
 * \var coloringMethodDescStr
 * \brief More descriptive strings for coloring methods.
 */
extern const char *coloringMethodDescStr[];

/**
 * \fn int GetColoringMethod(ColoringMethod *coloringMethod, const char *str)
 *
 * Function is case insensitive.
 * Possible strings  are :
 * - "iterationcount" for CM_ITERATIONCOUNT
 * - "averagecoloring" for CM_AVERAGECOLORING
 * Exit with error in case of failure.
 *
 * \param coloringMethod Coloring method destination.
 * \param str String specifying coloring method.
 * \return 0 in case of success, 1 in case of failure.
 */
int GetColoringMethod(ColoringMethod *coloringMethod, const char *str);

/**
 * \enum e_InterpolationMethod
 * \brief Possible coloring methods.
 *
 * Interpolation methods use an addend function to compute a number of average
 * sums, from which they compute de fractal value.\n
 * For no interpolation, only 1 sum is to be computed, and is returned as is.\n
 * For linear and spline interpolation, respectively 2 and 4 sums are to be
 * computed by addend function, and the final value is computed from those
 * sums using the iteration count function.
 *
 * See addend function and interation count headers for more details.
 * \see fractal_addend_function.h
 * \see fractal_counting_function.h
 */
/**
 * \typedef InterpolationMethod
 * \brief Convenient typedef for enum InterpolationMethod.
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
 * \var nbInterpolationMethods
 * \brief Number of interpolation methods.
 */
extern const uint_fast32_t nbInterpolationMethods;

/**
 * \var interpolationMethodStr
 * \brief Strings of interpolation method enum values.
 */
extern const char *interpolationMethodStr[];

/**
 * \var interpolationMethodDescStr
 * \brief More descriptive strings for interpolation methods.
 */
extern const char *interpolationMethodDescStr[];

/**
 * \fn int GetInterpolationMethod(InterpolationMethod *interpolationMethod, const char *str)
 *
 * Function is case insensitive.\n
 * Possible strings are :
 * - "none" for no interpolation (only one average sum)
 * - "linear" for linear interpolation (two average sums)
 * - "spline" for spline interpolation (five average sums)
 *
 * \param interpolationMethod Interpolation method destination.
 * \param str String specifying interpolation function.
 * \return 0 in case of success, 1 in case of failure.
 */
int GetInterpolationMethod(InterpolationMethod *interpolationMethod, const char *str);

/********************CM_ITERATIONCOUNT********************/
#define ENGINE_DECL_VAR_CM_ITERATIONCOUNT(iterationcount,addend,interpolation,fprec) \
ENGINE_DECL_VAR_##iterationcount(fprec)

#define ENGINE_INIT_VAR_CM_ITERATIONCOUNT(iterationcount,addend,interpolation,fprec) \
ENGINE_INIT_VAR_##iterationcount(fprec)

#define ENGINE_CLEAR_VAR_CM_ITERATIONCOUNT(iterationcount,addend,interpolation,fprec) \
ENGINE_CLEAR_VAR_##iterationcount(fprec)

#define LOOP_INIT_CM_ITERATIONCOUNT(iterationcount,addend,interpolation,fprec) \
(void)NULL;

#define LOOP_ITERATION_CM_ITERATIONCOUNT(iterationcount,addend,interpolation,fprec) \
(void)NULL;

#define LOOP_END_CM_ITERATIONCOUNT(iterationcount,addend,interpolation,fprec) \
COMPUTE_##iterationcount(fprec);\
/*********************************************************/

/********************CM_AVERAGECOLORING*******************/
#define ENGINE_DECL_VAR_CM_AVERAGECOLORING(iterationcount,addend,interpolation,fprec) \
ENGINE_DECL_VAR_##interpolation(addend,fprec)

#define ENGINE_INIT_VAR_CM_AVERAGECOLORING(iterationcount,addend,interpolation,fprec) \
ENGINE_INIT_VAR_##interpolation(addend,fprec)

#define ENGINE_CLEAR_VAR_CM_AVERAGECOLORING(iterationcount,addend,interpolation,fprec) \
ENGINE_CLEAR_VAR_##interpolation(addend,fprec)

#define LOOP_INIT_CM_AVERAGECOLORING(iterationcount,addend,interpolation,fprec) \
LOOP_INIT_##interpolation(addend,fprec)

#define LOOP_ITERATION_CM_AVERAGECOLORING(iterationcount,addend,interpolation,fprec) \
LOOP_ITERATION_##interpolation(addend,fprec)

#define LOOP_END_CM_AVERAGECOLORING(iterationcount,addend,interpolation,fprec) \
LOOP_END_##interpolation(addend,fprec)
/*********************************************************/

/*************************IM_NONE*************************/
#define ENGINE_DECL_VAR_IM_NONE(addend,fprec) \
FLOATTYPE(fprec) SN_IM[1];\
ENGINE_DECL_VAR_##addend(1,fprec)

#define ENGINE_INIT_VAR_IM_NONE(addend,fprec) \
initF(fprec, data->SN_IM[0]);\
ENGINE_INIT_VAR_##addend(1,fprec)

#define ENGINE_CLEAR_VAR_IM_NONE(addend,fprec) \
clearF(fprec, data->SN_IM[0]);\
ENGINE_CLEAR_VAR_##addend(1,fprec)

#define LOOP_INIT_IM_NONE(addend,fprec) \
LOOP_INIT_##addend(1,fprec)

#define LOOP_ITERATION_IM_NONE(addend,fprec) \
LOOP_ITERATION_##addend(1,fprec)

#define LOOP_END_IM_NONE(addend,fprec) \
LOOP_END_##addend(1,fprec)\
assignF(fprec,data->res,data->SN_IM[0]);
/*********************************************************/

/************************IM_LINEAR************************/
#define ENGINE_DECL_VAR_IM_LINEAR(addend,fprec) \
ENGINE_DECL_VAR_IC_SMOOTH(fprec)\
FLOATTYPE(fprec) tmp_IM;\
FLOATTYPE(fprec) SN_IM[2];\
ENGINE_DECL_VAR_##addend(2,fprec)

#define ENGINE_INIT_VAR_IM_LINEAR(addend,fprec) \
ENGINE_INIT_VAR_IC_SMOOTH(fprec)\
initF(fprec, data->tmp_IM);\
initF(fprec, data->SN_IM[0]);\
initF(fprec, data->SN_IM[1]);\
ENGINE_INIT_VAR_##addend(2,fprec)

#define ENGINE_CLEAR_VAR_IM_LINEAR(addend,fprec) \
ENGINE_CLEAR_VAR_IC_SMOOTH(fprec)\
clearF(fprec, data->tmp_IM);\
clearF(fprec, data->SN_IM[0]);\
clearF(fprec, data->SN_IM[1]);\
ENGINE_CLEAR_VAR_##addend(2,fprec)

#define LOOP_INIT_IM_LINEAR(addend,fprec) \
LOOP_INIT_##addend(2,fprec)

#define LOOP_ITERATION_IM_LINEAR(addend,fprec) \
LOOP_ITERATION_##addend(2,fprec)

#define LOOP_END_IM_LINEAR(addend,fprec) \
LOOP_END_##addend(2,fprec)\
COMPUTE_IC_SMOOTH(fprec)\
modfF(fprec, data->tmp_IM, data->res, data->res);\
sub_uiF(fprec, data->tmp_IM, data->res, 1);\
mulF(fprec, data->tmp_IM, data->tmp_IM, data->SN_IM[1]);\
mulF(fprec, data->res, data->res, data->SN_IM[0]);\
subF(fprec, data->res, data->res, data->tmp_IM);
/*********************************************************/

/************************IM_SPLINE************************/
#define ENGINE_DECL_VAR_IM_SPLINE(addend,fprec) \
ENGINE_DECL_VAR_IC_SMOOTH(fprec)\
FLOATTYPE(fprec) tmp_IM;\
FLOATTYPE(fprec) d1_IM;\
FLOATTYPE(fprec) d2_IM;\
FLOATTYPE(fprec) d3_IM;\
FLOATTYPE(fprec) s1_IM;\
FLOATTYPE(fprec) s2_IM;\
FLOATTYPE(fprec) s3_IM;\
FLOATTYPE(fprec) SN_IM[4];\
ENGINE_DECL_VAR_##addend(4,fprec)

#define ENGINE_INIT_VAR_IM_SPLINE(addend,fprec) \
ENGINE_INIT_VAR_IC_SMOOTH(fprec)\
initF(fprec,data->tmp_IM);\
initF(fprec,data->d1_IM);\
initF(fprec,data->d2_IM);\
initF(fprec,data->d3_IM);\
initF(fprec,data->s1_IM);\
initF(fprec,data->s2_IM);\
initF(fprec,data->s3_IM);\
for (uint_fast32_t i = 0; i < 4; ++i) {\
	initF(fprec,data->SN_IM[i]);\
}\
ENGINE_INIT_VAR_##addend(4,fprec)

#define ENGINE_CLEAR_VAR_IM_SPLINE(addend,fprec) \
ENGINE_CLEAR_VAR_IC_SMOOTH(fprec)\
clearF(fprec,data->tmp_IM);\
clearF(fprec,data->d1_IM);\
clearF(fprec,data->d2_IM);\
clearF(fprec,data->d3_IM);\
clearF(fprec,data->s1_IM);\
clearF(fprec,data->s2_IM);\
clearF(fprec,data->s3_IM);\
for (uint_fast32_t i = 0; i < 4; ++i) {\
	clearF(fprec,data->SN_IM[i]);\
}\
ENGINE_CLEAR_VAR_##addend(4,fprec)

#define LOOP_INIT_IM_SPLINE(addend,fprec) \
LOOP_INIT_##addend(4,fprec)

#define LOOP_ITERATION_IM_SPLINE(addend,fprec) \
LOOP_ITERATION_##addend(4,fprec)

#define LOOP_END_IM_SPLINE(addend,fprec) \
LOOP_END_##addend(4,fprec)\
COMPUTE_IC_SMOOTH(fprec)\
modfF(fprec,data->tmp_IM,data->d1_IM,data->res);\
mulF(fprec,data->d2_IM,data->d1_IM,data->d1_IM);\
mulF(fprec,data->d3_IM,data->d1_IM,data->d2_IM);\
\
/* Compute s1 */\
mul_uiF(fprec,data->tmp_IM,data->d2_IM,4);\
addF(fprec,data->s1_IM,data->d1_IM,data->tmp_IM);\
mul_uiF(fprec,data->tmp_IM,data->d3_IM,3);\
subF(fprec,data->s1_IM,data->s1_IM,data->tmp_IM);\
mulF(fprec,data->s1_IM,data->s1_IM,data->SN_IM[1]);\
\
/* Compute s2 */\
mul_uiF(fprec,data->s2_IM,data->d3_IM,3);\
mul_uiF(fprec,data->tmp_IM,data->d2_IM,5);\
subF(fprec,data->s2_IM,data->s2_IM,data->tmp_IM);\
add_uiF(fprec,data->s2_IM,data->s2_IM,2);\
mulF(fprec,data->s2_IM,data->s2_IM,data->SN_IM[2]);\
\
/* Compute s3 */\
mul_uiF(fprec,data->s3_IM,data->d2_IM,2);\
subF(fprec,data->s3_IM,data->s3_IM,data->d1_IM);\
subF(fprec,data->s3_IM,data->s3_IM,data->d3_IM);\
mulF(fprec,data->s3_IM,data->s3_IM,data->SN_IM[3]);\
\
/* Compute res */\
subF(fprec,data->res,data->d3_IM,data->d2_IM);\
mulF(fprec,data->res,data->res,data->SN_IM[0]);\
addF(fprec,data->res,data->res,data->s1_IM);\
addF(fprec,data->res,data->res,data->s2_IM);\
addF(fprec,data->res,data->res,data->s3_IM);\
div_uiF(fprec,data->res,data->res,2);
/*********************************************************/

#ifdef __cplusplus
}
#endif

#endif

