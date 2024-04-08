/*
 *  fractal_orbit.c -- part of fractal2D
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

#include "fractal_orbit.h"
#include "misc.h"

void CreateOrbit(FractalOrbit *orbit, uint_fast32_t maxIterations)
{
	orbit->maxIterations = maxIterations;
	orbit->N = 0;
	orbit->limitPrecision = 0;
	orbit->rn = (FLOAT *)safeMalloc("orbit (rn)",(maxIterations+1)*sizeof(FLOAT));
	orbit->mn = (FLOAT *)safeMalloc("orbit (mn)",(maxIterations+1)*sizeof(FLOAT));
	orbit->Mn = (FLOAT *)safeMalloc("orbit (Mn)",(maxIterations+1)*sizeof(FLOAT));
	orbit->tn = (FLOAT *)safeMalloc("orbit (tn)",(maxIterations+1)*sizeof(FLOAT));
}

void FreeOrbit(FractalOrbit *orbit)
{
	free(orbit->rn);
	free(orbit->mn);
	free(orbit->Mn);
	free(orbit->tn);
}

