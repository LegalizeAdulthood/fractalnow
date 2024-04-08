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

#include "floating_point.h"
#include <stdint.h>

struct s_Fractal;
struct s_FractalOrbit;

/**
 * \typedef AddendFunction
 * \brief Fractal addend function.
 *
 * Addend functions are used to compute values of fractal
 * in case of average coloring (CM_AVERAGE).
 * Addend function are applied on an orbit and compute
 * a number of average sums to be used by an interpolation
 * function.
 * Remark : in the fractal "literature", addend functions
 * are not exactly the functions THAT COMPUTE the average sums,
 * but functions USED when computing those sums.
 * We justify this design because there exist different addend
 * functions, and it is more computationally efficient to branch
 * (i.e. select the addend function to use) outside loops
 * than inside (even if we used function pointers).
 * Remark2 : some sums might not be computable if the number of
 * iterations (in orbit) is too small (N must be >= size).
 *
 * \param orbit Orbit used to compute average sums.
 * \param SN Pointer to array in which to store computed average sums.
 * \param size Number of average sums to compute (size of SN).
 * \return 0 if the average sums were computed, 1 otherwise (N too small).
 */
typedef int (*AddendFunction)(struct s_FractalOrbit *orbit, FLOAT *SN, uint_fast32_t size);

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

#endif

