/*
 *  fractal_loop.h -- part of fractal2D
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
  * \file fractal_loop.h
  * \brief Header file related to fractal loops.
  * \author Marc Pegon
  */
 
#ifndef __FRACTAL_LOOP_H__
#define __FRACTAL_LOOP_H__

#include "fractal.h"
#include "fractal_coloring.h"
#include "floating_point.h"

struct s_FractalOrbit;

/**
 * \typedef FractalLoop
 * \brief Fractal loop function.
 *
 * Fractal loops are functions to compute the sequence of complex z0, z1, z2, ...
 * and the fractal orbit (more or less partially depending on coloring method).
 * For example, we do not need to compute the whole orbit for CM_SIMPLE
 * coloring method (as opposed to CM_AVERAGE), but only the number of iterations
 * and the last norm of z : hence different fractal loop functions, even for same
 * fractal types.
 * We can also use different fractal loop functions depending on whether p is an
 * integer or a float (branching outside loop is faster than branching inside).
 */
typedef void (*FractalLoop)(Fractal *fractal, struct s_FractalOrbit *orbit, FLOAT complex z);

/**
 * \fn FractalLoop GetFractalLoop(FractalType fractalType, ColoringMethod interpolationMethod, int p_is_integer)
 * \brief Get fractal loop function from fractal type, coloring method used, and p parameter.
 *
 * \param fractalType Fractal type.
 * \param coloringMethod Coloring method.
 * \param p_is_integer 0 or 1 depending on whether p fractal parameter i 0 or 1.
 * \return Corresponding fractal loop function.
 */
FractalLoop GetFractalLoop(FractalType fractalType, ColoringMethod coloringMethod,
				int p_is_integer);

#endif

