/*
 *  misc.h -- part of fractal2D
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

#ifdef __cplusplus
extern "C" {
#endif

#include "floating_point.h"
#include <inttypes.h>
#include <stdlib.h>

/**
 * \fn void toLowerCase(char *str)
 * \brief Convert string letter to lower case.
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
 * \fn int isInteger(FLOAT x)
 * \brief Test whether or not FLOAT is integer.
 *
 * \param x FLOAT to test.
 * \return 1 is x is an integer, 0 instead.
 */
int isInteger(FLOAT x);

#ifdef __cplusplus
}
#endif

#endif
