/*
 *  misc.h -- part of FractalNow
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
  * \file misc.h
  * \brief Header file containing miscellaneous functions.
  * \author Marc Pegon
  */
 
#ifndef __MISC_H__
#define __MISC_H__

#include "float_precision.h"
#include <inttypes.h>
#include <stdlib.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * \def UNUSED(x)
 * \brief Macro for unused variables.
 */
#define UNUSED(x) (void)x

/**
 * \fn void toLowerCase(char *str)
 * \brief Convert string letter to lower case.
 *
 * str must be NULL-terminated (undefined behabiour
 * otherwise).
 *
 * \param str String to convert.
 */
void toLowerCase(char *str);

/**
 * \fn void *safeMalloc(const char *name, uint_least64_t size)
 * \brief Safe memory allocation.
 *
 * Exit with error if memory allocation fails.
 *
 * \param name Name to print in case of allocation failure.
 * \param size Number of bytes to allocate.
 * \return Pointer to the allocated memory.
 */
void *safeMalloc(const char *name, uint_least64_t size);

/**
 * \fn void *safeRealloc(const char *name, void *ptr, uint_least64_t size)
 * \brief Safe memory reallocation.
 *
 * Exit with error if memory reallocation fails.
 *
 * \param name Name to print in case of allocation failure.
 * \param ptr Pointer to memory to reallocate.
 * \param size Number of bytes to allocate.
 * \return Pointer to the allocated memory.
 */
void *safeRealloc(const char *name, void *ptr, uint_least64_t size);

/**
 * \fn void void *safeCalloc(const char *name, uint_least64_t nmemb, uint_least64_t size)
 * \brief Safe memory allocation.
 *
 * Exit with error if memory allocation fails.
 *
 * \param name Name to print in case of allocation failure.
 * \param nmemb Number of elements in array to allocate.
 * \param size Size of each element in bytes.
 * \return Pointer to the allocated memory.
 */
void *safeCalloc(const char *name, uint_least64_t nmemb, uint_least64_t size);

#ifdef _ENABLE_MP_FLOATS
/**
 * \fn int mpcIsInteger(const mpc_t x)
 * \brief Test if a mpc complex is a (real) integer.
 *
 * \param x Complex to be tested.
 * \return 1 if complex is a (real) integer, 0 otherwise.
 */
int mpcIsInteger(const mpc_t x);
#endif

#ifdef __cplusplus
}
#endif

#endif
