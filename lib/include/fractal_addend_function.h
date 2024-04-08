/*
 *  fractal_addend_function.h -- part of FractalNow
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
  * \brief Header file related to fractal addend functions.
  * \author Marc Pegon
  */
 
#ifndef __FRACTAL_ADDEND_FUNCTION_H__
#define __FRACTAL_ADDEND_FUNCTION_H__

#ifdef __cplusplus
extern "C" {
#endif

#include "floating_point.h"
#include <complex.h>
#include <stdint.h>

#ifndef complex
#define complex _Complex
#endif

/**
 * \enum e_AddendFunction
 * \brief Fractal addend function.
 *
 * Addend functions are used to compute values of fractal
 * in case of average coloring (CM_AVERAGECOLORING).
 *
 * Addend functions are described by a sequence of instructions
 * at the initialization of the fractal loop, for each iteration,
 * and at the end of the fractal loop.\n
 * They take for parameter 'size' the number of average sums to
 * be computed. Those sums SN[0], SN[1], ..., SN[size-1] *must*
 * be computed (at least initialized) by the addend function,
 * since they will be used by the interpolation method.
 * 
 * Remark : in the fractal "literature", addend functions
 * are not exactly the functions *that compute* the average
 * sums, but functions *used* when computing those sums.\n
 * Remark 2 : some sums might not be computable if the number of
 * iterations (in orbit) is too small (N must be >= size).
 */
/**
 * \typedef AddendFunction
 * \brief Convenient typedef for enum AddendFunction.
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
extern const uint_fast32_t nbAddendFunctions;

/**
 * \var addendFunctionStr
 * \brief Strings of addend function enum values.
 */
extern const char *addendFunctionStr[];

/**
 * \var addendFunctionDescStr
 * \brief More descriptive strings for addend functions.
 */
extern const char *addendFunctionDescStr[];

/**
 * \fn int GetAddendFunction(AddendFunction *addendFunction, const char *str)
 * \brief Get addend function from string.
 *
 * Function is case insensitive.
 * Possible strings are :
 * - "triangleinequality" for triangle inequality
 * - "curvature" for curvature
 * - "stripe" for stripe
 *
 * \param addendFunction AddendFunction destination.
 * \param str String specifying addend function.
 * \return 0 in case of success, 1 in case of failure.
 */
int GetAddendFunction(AddendFunction *addendFunction, const char *str);

/******************AF_TRIANGLEINEQUALITY******************/
#define LOOP_INIT_AF_TRIANGLEINEQUALITY(size) \
uint_fast32_t zeros[size];\
FLOATT complex prevZP = 0;\
FLOATT nPrevZP = 0;\
FLOATT mn=0, Mn=0, rn=0;\
uint_fast32_t currentIndex = 0;\
uint_fast32_t previousIndex = size-1;\
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
	FLOATT diff = Mn-mn;\
	zeros[currentIndex] = zeros[previousIndex];\
	SN[currentIndex] = SN[previousIndex];\
	if (diff != 0) {\
		/* Avoid division by zero. */\
		SN[currentIndex] += (rn - mn) / diff;\
	} else {\
		/* Counting zeros in order to divide by*/\
		/* the exact number of terms added to SN.*/\
		++zeros[currentIndex];\
	}\
	previousIndex = currentIndex;\
	currentIndex = (currentIndex + 1) % size;\
}

#define LOOP_END_AF_TRIANGLEINEQUALITY(size) \
uint_fast32_t m = 1;\
if (n >= m+size-1) {\
	FLOATT shiftSN[size];\
	uint_fast32_t shiftZeros[size];\
	for (uint_fast32_t i = 0; i < size; ++i) {\
		shiftSN[i] = SN[i];\
		shiftZeros[i] = zeros[i];\
	}\
	for (uint_fast32_t i = 0; i < size; ++i) {\
		zeros[i] = shiftZeros[(previousIndex+size-i) % size];\
		if (zeros[i] == n-m-i) {\
			SN[i] = 0;\
		} else {\
			SN[i] = shiftSN[(previousIndex+size-i) % size] / (n+1-m-i-zeros[i]);\
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
uint_fast32_t zeros[size];\
FLOATT complex znm1 = 0;\
FLOATT complex znm2 = 0;\
FLOATT complex diff = 0;\
uint_fast32_t currentIndex = 0;\
uint_fast32_t previousIndex = size-1;\
for (uint_fast32_t i = 0; i < size; ++i) {\
	SN[i] = 0;\
	zeros[i] = 0;\
}

#define LOOP_ITERATION_AF_CURVATURE(size) \
uint_fast32_t m = 1;\
if (n >= m+1) {\
	diff = znm1-znm2;\
	SN[currentIndex] = SN[previousIndex];\
	zeros[currentIndex] = zeros[previousIndex];\
	if (diff != 0) {\
		/* Avoid division by zero. */\
		SN[currentIndex] += fabsF(cargF((z-znm1) / diff));\
	} else {\
		/* Counting zeros in order to divide by*/\
		/* the exact number of terms added to SN.*/\
		++zeros[currentIndex];\
	}\
	previousIndex = currentIndex;\
	currentIndex = (currentIndex + 1) % size;\
}\
znm2 = znm1;\
znm1 = z;

#define LOOP_END_AF_CURVATURE(size) \
uint_fast32_t m = 1;\
if (n >= m+size) {\
	FLOATT shiftSN[size];\
	uint_fast32_t shiftZeros[size];\
	for (uint_fast32_t i = 0; i < size; ++i) {\
		shiftSN[i] = SN[i];\
		shiftZeros[i] = zeros[i];\
	}\
	for (uint_fast32_t i = 0; i < size; ++i) {\
		zeros[i] = shiftZeros[(previousIndex+size-i) % size];\
		if (zeros[i] == n-m-i) {\
			SN[i] = 0;\
		} else {\
			SN[i] = shiftSN[(previousIndex+size-i) % size] / (n-m-i-zeros[i]);\
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
uint_fast32_t currentIndex = 0;\
uint_fast32_t previousIndex = size-1;\
for (uint_fast32_t i = 0; i < size; ++i) {\
	SN[i] = 0;\
}

#define LOOP_ITERATION_AF_STRIPE(size) \
uint_fast32_t m = 1;\
if (n >= m) {\
	SN[currentIndex] = SN[previousIndex] + sinF(render->stripeDensity*cargF(z))+1;\
	previousIndex = currentIndex;\
	currentIndex = (currentIndex + 1) % size;\
}

#define LOOP_END_AF_STRIPE(size) \
uint_fast32_t m = 1;\
if (n >= m+size-1) {\
	FLOATT shiftSN[size];\
	for (uint_fast32_t i = 0; i < size; ++i) {\
		shiftSN[i] = SN[i];\
	}\
	for (uint_fast32_t i = 0; i < size; ++i) {\
		SN[i] = shiftSN[(previousIndex+size-i) % size] / (2*(n+1-m-i));\
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

