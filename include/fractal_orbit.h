/*
 *  fractal_orbit.h -- part of fractal2D
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
  * \file fractal_orbit.h
  * \brief Header file related to (partial) fractal orbits.
  * \author Marc Pegon
  */
 
#ifndef __FRACTAL_ORBIT_H__
#define __FRACTAL_ORBIT_H__

#include "floating_point.h"
#include <stdint.h>

/**
 * \struct s_FractalOrbit
 * \brief A (partial) fractal orbit.
 *
 * A partial fractal orbit is normally the complex number
 * sequence when computing fractal (i.e. when iterating 
 * z_n = z_{n-1}^p + c for example) at one point.
 * Here we do not store the sequence of complex numbers
 * but the sequence of norms of complex numbers, plus
 * additional things like minimum (mn) and maximum (Mn)
 * bound for |z| at each iteration.
 */
typedef struct s_FractalOrbit {
	uint_fast32_t maxIterations;
 /*<! Maximum number of iterations (or orbit maximum size).*/
	uint_fast32_t N;
 /*<! Current number of iterations (obtained at last computation).*/
	uint_fast32_t limitPrecision;
	FLOAT *rn;
 /*<! Sequence of |zn|.*/
	FLOAT *mn;
 /*<! Sequence of minimum bound for |zn|, i.e. ||z{n-1}|^p-|c||.*/
	FLOAT *Mn;
 /*<! Sequence of maximum bound for |zn|, i.e. |z{n-1}|^p+|c|.*/
	FLOAT *tn;
 /*<! For internal use (ouput of addend function for each zn, mn, Mn).*/
} FractalOrbit;

/**
 * \fn void CreateOrbit(FractalOrbit *orbit, uint_fast32_t maxIterations)
 * \brief Create partial orbit.
 *
 * \param orbit Pointer to orbit structure to initialize.
 * \param maxIterations Maximum number of iterations for orbit.
 */
void CreateOrbit(FractalOrbit *orbit, uint_fast32_t maxIterations);

/**
 * \fn void FreeOrbit(FractalOrbit *orbit)
 * \brief Free orbit structure.
 *
 * \param orbit Pointer to orbit structure to free.
 */
void FreeOrbit(FractalOrbit *orbit);

#endif
