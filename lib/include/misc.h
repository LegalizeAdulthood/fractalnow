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

#ifndef complex
#define complex _Complex
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

/**
 * \fn float complex cipowf(float complex x, uint_fast32_t y)
 * \brief Compute positive integer power of a complex float.
 *
 * \param x Complex float to be raised.
 * \param y Exponent.
 * \return x raised to the power y.
 */
float complex cipowf(float complex x, uint_fast32_t y);

/**
 * \fn double complex cipow(double complex x, uint_fast32_t y)
 * \brief Compute positive integer power of a complex double.
 *
 * \param x Complex double to be raised to y.
 * \param y Exponent.
 * \return x raised to the power y.
 */
double complex cipow(double complex x, uint_fast32_t y);

/**
 * \fn long double complex cipowl(long double complex x, uint_fast32_t y)
 * \brief Compute positive integer power of a complex long double.
 *
 * \param x Complex long double to be raised to y.
 * \param y Exponent.
 * \return x raised to the power y.
 */
long double complex cipowl(long double complex x, uint_fast32_t y);

/**
 * \fn int complexIsInteger(long double complex x)
 * \brief Test if a long double complex is a (real) integer.
 *
 * \param x Complex to be tested.
 * \return 1 if complex is a (real) integer, 0 otherwise.
 */
int complexIsInteger(long double complex x);

/**
 * \fn int mpcIsInteger(const mpc_t x)
 * \brief Test whether or not mpc complex is an integer.
 *
 * \param x mpc complex to test.
 * \return 1 is x is an integer, 0 instead.
 */
int mpcIsInteger(const mpc_t x);

#ifdef __cplusplus
}
#endif

#endif
