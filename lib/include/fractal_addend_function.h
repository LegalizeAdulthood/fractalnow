/*
 *  fractal_addend_function.h -- part of fractal2D
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
  * \file fractal_addend_function.h
  * \brief Header file related to interpolation of fractal.
  * \author Marc Pegon
  */
 
#ifndef __FRACTAL_ADDEND_FUNCTION_H__
#define __FRACTAL_ADDEND_FUNCTION_H__

#ifdef __cplusplus
extern "C" {
#endif

#include "floating_point.h"
#include <stdint.h>

/**
 * \enum e_AddendFunction
 * \brief Fractal addend function.
 *
 * Addend functions are used to compute values of fractal
 * in case of average coloring (CM_AVERAGE).
 *
 * Addend functions are described by a sequence of instructions
 * at the initialization of the fractal loop, for each iteration,
 * and at the end of the fractal loop.
 * They take for parameter 'size' the number of average sums to
 * be computed. Those sums SN[0], SN[1], ..., SN[size-1] MUST be
 * computed (at least initialized) by the addend function, since
 * they will be used by the interpolation method.
 * 
 * Remark : in the fractal "literature", addend functions
 * are not exactly the functions THAT COMPUTE the average sums,
 * but functions USED when computing those sums.
 * Remark 2 : some sums might not be computable if the number of
 * iterations (in orbit) is too small (N must be >= size).
 */
typedef enum e_AddendFunction {
	AF_TRIANGLEINEQUALITY = 0,
	AF_CURVATURE,
	AF_STRIPE
} AddendFunction;

/**
 * \var nbAddendFunctions
 * \brief Number of addend functions.
 */
extern uint_fast32_t nbAddendFunctions;

/**
 * \var AddendFunctionStr
 * \brief Strings of addend function enum values.
 */
extern const char *AddendFunctionStr[];

/**
 * \var AddendFunctionDescStr
 * \brief More descriptive strings for addend functions.
 */
extern const char *AddendFunctionDescStr[];

/**
 * \fn AddendFunction GetAddendFunction(const char *str)
 * \brief Get addend function from string.
 *
 * Function is case insensitive.
 * Possible strings are :
 * - "triangleinequality" for triangle inequality
 * Exit with error in case of failure.
 *
 * \param str String specifying addend function.
 * \return Corresponding addend function.
 */
AddendFunction GetAddendFunction(const char *str);

/******************AF_TRIANGLEINEQUALITY******************/
#define LOOP_INIT_AF_TRIANGLEINEQUALITY(size) \
FLOAT zeros[size];\
FLOAT complex prevZP = 0;\
FLOAT nPrevZP = 0;\
FLOAT mn=0, Mn=0, rn=0;\
for (uint_fast32_t i = 0; i < size; ++i) {\
	SN[i] = 0;\
	zeros[i] = 0;\
}

#define LOOP_ITERATION_AF_TRIANGLEINEQUALITY(size) \
uint_fast32_t m = 1;\
if (n >= m) {\
	prevZP = z-c;\
	nPrevZP = cabs(prevZP);\
	mn = fabsF(nPrevZP-normC);\
	Mn = nPrevZP+normC;\
	rn = sqrtF(normZ2);\
	for (uint_fast32_t i = size-1; i > 0; --i) {\
		SN[i] = SN[i-1];\
		zeros[i] = zeros[i-1];\
	}\
	FLOAT diff = Mn-mn;\
	if (diff != 0) {\
		/* Avoid division by zero. */\
		SN[0] += (rn - mn) / diff;\
	} else {\
		/* Counting zeros in order to divide by*/\
		/* the exact number of terms added to SN.*/\
		++zeros[0];\
	}\
}

#define LOOP_END_AF_TRIANGLEINEQUALITY(size) \
uint_fast32_t m = 1;\
if (n >= m+size-1) {\
	for (uint_fast32_t i = 0; i < size; ++i) {\
		if (zeros[i] == n-m-i) {\
			SN[i] = 0;\
		} else {\
			SN[i] /= n+1-m-i-zeros[i];\
		}\
	}\
} else {\
	/* Result undefined. 0 chosen. */\
	for (uint_fast32_t i = 0; i < size; ++i) {\
		SN[i] = 0;\
	}\
}
/*********************************************************/

/***********************AF_CURVATURE**********************/
#define LOOP_INIT_AF_CURVATURE(size) \
FLOAT zeros[size];\
FLOAT complex znm1 = 0;\
FLOAT complex znm2 = 0;\
for (uint_fast32_t i = 0; i < size; ++i) {\
	SN[i] = 0;\
	zeros[i] = 0;\
}

#define LOOP_ITERATION_AF_CURVATURE(size) \
uint_fast32_t m = 1;\
if (n >= m+1) {\
	for (uint_fast32_t i = size-1; i > 0; --i) {\
		SN[i] = SN[i-1];\
		zeros[i] = zeros[i-1];\
	}\
	FLOAT complex diff = znm1-znm2;\
	if (diff != 0) {\
		/* Avoid division by zero. */\
		SN[0] += fabsF(cargF((z-znm1) / diff));\
	} else {\
		/* Counting zeros in order to divide by*/\
		/* the exact number of terms added to SN.*/\
		++zeros[0];\
	}\
}\
znm2 = znm1;\
znm1 = z;

#define LOOP_END_AF_CURVATURE(size) \
uint_fast32_t m = 1;\
if (n >= m+size) {\
	for (uint_fast32_t i = 0; i < size; ++i) {\
		if (zeros[i] == n-m-i) {\
			SN[i] = 0;\
		} else {\
			SN[i] /= n-m-i-zeros[i];\
		}\
	}\
} else {\
	/* Result undefined. 0 chosen. */\
	for (uint_fast32_t i = 0; i < size; ++i) {\
		SN[i] = 0;\
	}\
}
/*********************************************************/

/************************AF_STRIPE************************/
#define LOOP_INIT_AF_STRIPE(size) \
for (uint_fast32_t i = 0; i < size; ++i) {\
	SN[i] = 0;\
}

#define LOOP_ITERATION_AF_STRIPE(size) \
uint_fast32_t m = 1;\
if (n >= m) {\
	for (uint_fast32_t i = size-1; i > 0; --i) {\
		SN[i] = SN[i-1];\
	}\
	SN[0] += sinF(render->stripeDensity*cargF(z))+1;\
}

#define LOOP_END_AF_STRIPE(size) \
uint_fast32_t m = 1;\
if (n >= m+size-1) {\
	for (uint_fast32_t i = 0; i < size; ++i) {\
		SN[i] /= 2*(n+1-m-i);\
	}\
} else {\
	/* Result undefined. 0 chosen. */\
	for (uint_fast32_t i = 0; i < size; ++i) {\
		SN[i] = 0;\
	}\
}\
/*********************************************************/

#ifdef __cplusplus
}
#endif

#endif

