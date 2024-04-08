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

#include "float_precision.h"
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
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
#define ENGINE_DECL_VAR_AF_TRIANGLEINEQUALITY(size,fprec) \
FLOATTYPE(fprec) shiftSN_AF[size];\
uint_fast32_t zeros_AF[size];\
uint_fast32_t shiftZeros_AF[size];\
COMPLEX_FLOATTYPE(fprec) prevZP_AF;\
FLOATTYPE(fprec) nPrevZP_AF;\
FLOATTYPE(fprec) mn_AF;\
FLOATTYPE(fprec) Mn_AF;\
FLOATTYPE(fprec) rn_AF;\
FLOATTYPE(fprec) diff_AF;\
FLOATTYPE(fprec) absC_AF;\
FLOATTYPE(fprec) tmp_AF;\
uint_fast32_t currentIndex_AF;\
uint_fast32_t previousIndex_AF;

#define ENGINE_INIT_VAR_AF_TRIANGLEINEQUALITY(size,fprec) \
for (uint_fast32_t i = 0; i < size; ++i) {\
	initF(fprec,data->shiftSN_AF[i]);\
}\
cinitF(fprec,data->prevZP_AF);\
initF(fprec,data->nPrevZP_AF);\
initF(fprec,data->mn_AF);\
initF(fprec,data->Mn_AF);\
initF(fprec,data->rn_AF);\
initF(fprec,data->diff_AF);\
initF(fprec,data->absC_AF);\
initF(fprec,data->tmp_AF);

#define ENGINE_CLEAR_VAR_AF_TRIANGLEINEQUALITY(size,fprec) \
for (uint_fast32_t i = 0; i < size; ++i) {\
	clearF(fprec,data->shiftSN_AF[i]);\
}\
cclearF(fprec,data->prevZP_AF);\
clearF(fprec,data->nPrevZP_AF);\
clearF(fprec,data->mn_AF);\
clearF(fprec,data->Mn_AF);\
clearF(fprec,data->rn_AF);\
clearF(fprec,data->diff_AF);\
clearF(fprec,data->absC_AF);\
clearF(fprec,data->tmp_AF);

#define LOOP_INIT_AF_TRIANGLEINEQUALITY(size,fprec) \
cfromUiF(fprec, data->prevZP_AF, 0);\
fromUiF(fprec, data->nPrevZP_AF, 0);\
fromUiF(fprec, data->mn_AF, 0);\
fromUiF(fprec, data->Mn_AF, 0);\
fromUiF(fprec, data->rn_AF, 0);\
fromUiF(fprec, data->diff_AF, 0);\
cabsF(fprec,data->absC_AF,data->c);\
data->currentIndex_AF = 0;\
data->previousIndex_AF = size-1;\
for (uint_fast32_t i = 0; i < size; ++i) {\
	fromUiF(fprec, data->SN_IM[i], 0);\
	data->zeros_AF[i] = 0;\
}

#define LOOP_ITERATION_AF_TRIANGLEINEQUALITY(size,fprec) \
{\
uint_fast32_t m = 1;\
if (data->n >= m) {\
	csubF(fprec,data->prevZP_AF,data->z,data->c);\
	cabsF(fprec,data->nPrevZP_AF,data->prevZP_AF);\
	subF(fprec,data->mn_AF,data->nPrevZP_AF,data->absC_AF);\
	fabsF(fprec,data->mn_AF,data->mn_AF);\
	addF(fprec,data->Mn_AF,data->nPrevZP_AF,data->absC_AF);\
	sqrtF(fprec,data->rn_AF,data->normZ);\
	subF(fprec,data->diff_AF,data->Mn_AF,data->mn_AF);\
	data->zeros_AF[data->currentIndex_AF] = data->zeros_AF[data->previousIndex_AF];\
	if (!eq_uiF(fprec,data->diff_AF,0)) {\
		/* Avoid division by zero. */\
		subF(fprec,data->tmp_AF,data->rn_AF,data->mn_AF);\
		divF(fprec,data->tmp_AF,data->tmp_AF,data->diff_AF);\
		addF(fprec,data->SN_IM[data->currentIndex_AF],data->SN_IM[data->previousIndex_AF],data->tmp_AF);\
	} else {\
		assignF(fprec, data->SN_IM[data->currentIndex_AF], data->SN_IM[data->previousIndex_AF]);\
		/* Counting zeros in order to divide by*/\
		/* the exact number of terms added to SN.*/\
		++data->zeros_AF[data->currentIndex_AF];\
	}\
	data->previousIndex_AF = data->currentIndex_AF;\
	data->currentIndex_AF = (data->currentIndex_AF + 1) % size;\
}\
}

#define LOOP_END_AF_TRIANGLEINEQUALITY(size,fprec) \
{\
uint_fast32_t m = 1;\
if (data->n >= m+size-1) {\
	for (uint_fast32_t i = 0; i < size; ++i) {\
		assignF(fprec, data->shiftSN_AF[i], data->SN_IM[i]);\
		data->shiftZeros_AF[i] = data->zeros_AF[i];\
	}\
	for (uint_fast32_t i = 0; i < size; ++i) {\
		data->zeros_AF[i] = data->shiftZeros_AF[(data->previousIndex_AF+size-i) % size];\
		if (data->zeros_AF[i] == data->n+1-m-i) {\
			fromUiF(fprec, data->SN_IM[i], 0);\
		} else {\
			div_uiF(fprec, data->SN_IM[i], data->shiftSN_AF[(data->previousIndex_AF+size-i) %\
				size], data->n+1-m-i-data->zeros_AF[i]);\
		}\
	}\
} else {\
	/* Result undefined. 0 chosen. */\
	for (uint_fast32_t i = 0; i < size; ++i) {\
		fromUiF(fprec, data->SN_IM[i], 0);\
	}\
}\
}
/*********************************************************/

/***********************AF_CURVATURE**********************/
#define ENGINE_DECL_VAR_AF_CURVATURE(size,fprec) \
FLOATTYPE(fprec) shiftSN_AF[size];\
uint_fast32_t zeros_AF[size];\
uint_fast32_t shiftZeros_AF[size];\
COMPLEX_FLOATTYPE(fprec) znm1_AF;\
COMPLEX_FLOATTYPE(fprec) znm2_AF;\
COMPLEX_FLOATTYPE(fprec) diff_AF;\
COMPLEX_FLOATTYPE(fprec) ctmp_AF;\
FLOATTYPE(fprec) tmp_AF;\
uint_fast32_t currentIndex_AF;\
uint_fast32_t previousIndex_AF;

#define ENGINE_INIT_VAR_AF_CURVATURE(size,fprec) \
for (uint_fast32_t i = 0; i < size; ++i) {\
	initF(fprec, data->shiftSN_AF[i]);\
}\
cinitF(fprec, data->znm1_AF);\
cinitF(fprec, data->znm2_AF);\
cinitF(fprec, data->diff_AF);\
cinitF(fprec, data->ctmp_AF);\
initF(fprec, data->tmp_AF);

#define ENGINE_CLEAR_VAR_AF_CURVATURE(size,fprec) \
cclearF(fprec, data->znm1_AF);\
cclearF(fprec, data->znm2_AF);\
cclearF(fprec, data->diff_AF);\
cclearF(fprec, data->ctmp_AF);\
clearF(fprec, data->tmp_AF);\
for (uint_fast32_t i = 0; i < size; ++i) {\
	clearF(fprec, data->shiftSN_AF[i]);\
}

#define LOOP_INIT_AF_CURVATURE(size,fprec) \
cfromUiF(fprec, data->znm1_AF, 0);\
cfromUiF(fprec, data->znm2_AF, 0);\
cfromUiF(fprec, data->diff_AF, 0);\
data->currentIndex_AF = 0;\
data->previousIndex_AF = size-1;\
for (uint_fast32_t i = 0; i < size; ++i) {\
	fromUiF(fprec, data->SN_IM[i], 0);\
	data->zeros_AF[i] = 0;\
}

#define LOOP_ITERATION_AF_CURVATURE(size,fprec) \
{\
uint_fast32_t m = 1;\
if (data->n >= m+1) {\
	csubF(fprec,data->diff_AF,data->znm1_AF,data->znm2_AF);\
	data->zeros_AF[data->currentIndex_AF] = data->zeros_AF[data->previousIndex_AF];\
	if (!ceq_siF(fprec,data->diff_AF,0)) {\
		/* Avoid division by zero. */\
		csubF(fprec,data->ctmp_AF,data->z,data->znm1_AF);\
		cdivF(fprec,data->ctmp_AF,data->ctmp_AF,data->diff_AF);\
		cargF(fprec,data->tmp_AF,data->ctmp_AF);\
		fabsF(fprec,data->tmp_AF,data->tmp_AF);\
		addF(fprec,data->SN_IM[data->currentIndex_AF],data->SN_IM[data->previousIndex_AF],data->tmp_AF);\
	} else {\
		assignF(fprec,data->SN_IM[data->currentIndex_AF],data->SN_IM[data->previousIndex_AF]);\
		/* Counting zeros in order to divide by*/\
		/* the exact number of terms added to SN.*/\
		++data->zeros_AF[data->currentIndex_AF];\
	}\
	data->previousIndex_AF = data->currentIndex_AF;\
	data->currentIndex_AF = (data->currentIndex_AF + 1) % size;\
}\
cassignF(fprec, data->znm2_AF, data->znm1_AF);\
cassignF(fprec, data->znm1_AF, data->z);\
}

#define LOOP_END_AF_CURVATURE(size,fprec) \
{\
uint_fast32_t m = 1;\
if (data->n >= m+size) {\
	for (uint_fast32_t i = 0; i < size; ++i) {\
		assignF(fprec, data->shiftSN_AF[i], data->SN_IM[i]);\
		data->shiftZeros_AF[i] = data->zeros_AF[i];\
	}\
	for (uint_fast32_t i = 0; i < size; ++i) {\
		data->zeros_AF[i] = data->shiftZeros_AF[(data->previousIndex_AF+size-i) % size];\
		if (data->zeros_AF[i] == data->n-m-i) {\
			fromUiF(fprec, data->SN_IM[i], 0);\
		} else {\
			div_uiF(fprec, data->SN_IM[i], data->shiftSN_AF[(data->previousIndex_AF+size-i) %\
				size], data->n-m-i-data->zeros_AF[i]);\
		}\
	}\
} else {\
	/* Result undefined. 0 chosen. */\
	for (uint_fast32_t i = 0; i < size; ++i) {\
		fromUiF(fprec, data->SN_IM[i], 0);\
	}\
}\
}
/*********************************************************/

/************************AF_STRIPE************************/
#define ENGINE_DECL_VAR_AF_STRIPE(size,fprec) \
uint_fast32_t currentIndex_AF ;\
uint_fast32_t previousIndex_AF;\
FLOATTYPE(fprec) tmp_AF ;\
FLOATTYPE(fprec) shiftSN_AF[size];

#define ENGINE_INIT_VAR_AF_STRIPE(size,fprec) \
initF(fprec, data->tmp_AF);\
for (uint_fast32_t i = 0; i < size; ++i) {\
	initF(fprec, data->shiftSN_AF[i]);\
}

#define ENGINE_CLEAR_VAR_AF_STRIPE(size,fprec) \
clearF(fprec,data->tmp_AF);\
for (uint_fast32_t i = 0; i < size; ++i) {\
	clearF(fprec, data->shiftSN_AF[i]);\
}

#define LOOP_INIT_AF_STRIPE(size,fprec) \
data->currentIndex_AF = 0;\
data->previousIndex_AF = size-1;\
for (uint_fast32_t i = 0; i < size; ++i) {\
	fromUiF(fprec, data->SN_IM[i], 0);\
}

#define LOOP_ITERATION_AF_STRIPE(size,fprec) \
{\
uint_fast32_t m = 1;\
if (data->n >= m) {\
	cargF(fprec,data->tmp_AF,data->z);\
	mul_dF(fprec,data->tmp_AF,data->tmp_AF,render->stripeDensity);\
	sinF(fprec,data->tmp_AF,data->tmp_AF);\
	add_uiF(fprec,data->tmp_AF,data->tmp_AF,1);\
	addF(fprec,data->SN_IM[data->currentIndex_AF],data->SN_IM[data->previousIndex_AF],data->tmp_AF);\
	data->previousIndex_AF = data->currentIndex_AF;\
	data->currentIndex_AF = (data->currentIndex_AF + 1) % size;\
}\
}

#define LOOP_END_AF_STRIPE(size,fprec) \
{\
uint_fast32_t m = 1;\
if (data->n >= m+size-1) {\
	for (uint_fast32_t i = 0; i < size; ++i) {\
		assignF(fprec, data->shiftSN_AF[i], data->SN_IM[i]);\
	}\
	for (uint_fast32_t i = 0; i < size; ++i) {\
		div_uiF(fprec, data->SN_IM[i], data->shiftSN_AF[(data->previousIndex_AF+size-i) %\
			size], 2*(data->n+1-m-i));\
	}\
} else {\
	/* Result undefined. 0 chosen. */\
	for (uint_fast32_t i = 0; i < size; ++i) {\
		fromUiF(fprec, data->SN_IM[i], 0);\
	}\
}\
}
/*********************************************************/

#ifdef __cplusplus
}
#endif

#endif

