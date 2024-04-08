/*
 *  fractal_transfer_function.h -- part of fractal2D
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
  * \file fractal_transfer_function.h
  * \brief Header file related to transfer functions.
  * \author Marc Pegon
  */
 
#ifndef __FRACTAL_TRANSFER_FUNCTION_H__
#define __FRACTAL_TRANSFER_FUNCTION_H__

#ifdef __cplusplus
extern "C" {
#endif

#include "floating_point.h"
#include <stdint.h>

/**
 * \enum e_TransferFunction
 * \brief Possible transfer functions.
 *
 * Transfer function are used to map fractal values to colors correctly.
 */
/**
 * \typedef TransferFunction
 * \brief Convenient typedef for enum TransferFunction.
 */
typedef enum e_TransferFunction {
	TF_LOG = 0,
 /*<! Logarithm. */
	TF_CUBEROOT,
 /*<! Cube root. */
	TF_SQUAREROOT,
 /*<! Square root. */
	TF_IDENTITY,
 /*<! Identity. */
	TF_SQUARE,
 /*<! Square. */
	TF_CUBE,
 /*<! Cube. */
	TF_EXP
 /*<! Exponential. */
} TransferFunction;

/**
 * \var nbTransferFunctions
 * \brief Number of transfer functions.
 */
extern const uint_fast32_t nbTransferFunctions;

/**
 * \var transferFunctionStr
 * \brief Strings of transfer functions.
 */
extern const char *transferFunctionStr[];

/**
 * \var transferFunctionDescStr
 * \brief More descriptive strings for fractal addend functions.
 */
extern const char *transferFunctionDescStr[];

/**
 * \typedef TransferFunctionPtr
 * \brief Transfer function ptr type.
 */
typedef FLOAT (*TransferFunctionPtr)(FLOAT x);

/**
 * \fn int GetTransferFunction(TransferFunction *transferFunction, const char *str)
 * \brief Get transfer function enum value from string.
 *
 * Function is case insensitive.
 * Possible strings are :
 * - "log" for logarithm
 * - 'cuberoot" for cube root
 * - "squareroot" for square root
 * - "identity" for identity
 * - "square" for square
 * - "cube" for cube
 * - "exp" for exponential
 *
 * \param transferFunction Transfer function destination.
 * \param str String specifying transfer function.
 * \return 0 in case of success, 1 in case of failure.
 */
int GetTransferFunction(TransferFunction *transferFunction, const char *str);

/**
 * \fn TransferFunctionPtr GetTransferFunctionPtr(TransferFunction transferFunction)
 * \brief Get transfer function ptr from transfer function enum value.
 *
 * \param transferFunction Transfer function enum value.
 * \return Corresponding transfer function ptr.
 */
TransferFunctionPtr GetTransferFunctionPtr(TransferFunction transferFunction);

#ifdef __cplusplus
}
#endif

#endif
