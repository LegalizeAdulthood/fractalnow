/*
 *  fractal_iteration_count.h -- part of fractal2D
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
  * \file fractal_iteration_count.h
  * \brief Header file related to iteration count method.
  * \author Marc Pegon
  */
 
#ifndef __FRACTAL_ITERATION_COUNT_H__
#define __FRACTAL_ITERATION_COUNT_H__

#include "floating_point.h"

struct s_Fractal;
struct s_FractalOrbit;

/**
 * \typedef IterationCountFunction
 * \brief Iteration count function type.
 *
 * There are different ways to "count" the number of iterations
 * when computing fractal, i.e. to compute fractal value :
 * it can be either simply the number of iterations itself (which
 * is discrete), or more advanced stuff like continuous or smooth
 * iteration count.
 * 
 */
typedef FLOAT (*IterationCountFunction)(struct s_Fractal *fractal, struct s_FractalOrbit *orbit);

/**
 * \fn IterationCountFunction GetIterationCountFunction(const char *str)
 * \brief Get iteration count function from string.
 *
 * Function is case insensitive.
 * Possible strings are :
 * - "discrete" for discrete iteration count
 * - "continuous" for continuous iteration count
 * - "smooth" for smooth iteration count
 * Exit with error in case of failure.
 *
 * \param str String specifying iteration count function.
 * \return Corresponding iteration count function.
 */
IterationCountFunction GetIterationCountFunction(const char *str);

#endif
