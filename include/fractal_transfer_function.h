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

#include "floating_point.h"

/**
 * \typedef TransferFunction
 * \brief Transfer function type to map fractal values to colors correctly.
 *
 * Possible transfer function are logarithm, cube root, square root,
 * zero, identity, square, cube, and exponential.
 */
typedef FLOAT (*TransferFunction)(FLOAT x);

/**
 * \fn TransferFunction GetTransferFunction(const char *str)
 * \brief Get transfer function from string.
 *
 * Function is case insensitive.
 * Possible strings are :
 * - "log" for logarithm
 * - 'cuberoot" for cube root
 * - "squareroot" for square root
 * - "zero" for zero
 * - "identity" for identity
 * - "square" for square
 * - "cube" for cube
 * - "exp" for exponential
 * Exit with error in case of failure.
 *
 * \param str String specifying transfer function.
 * \return Corresponding transfer function.
 */
TransferFunction GetTransferFunction(const char *str);

#endif
