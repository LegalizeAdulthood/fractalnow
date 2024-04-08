/*
 *  fractal_loop.c -- part of fractal2D
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
#include "fractal_loop.h"
#include <inttypes.h>

inline static FLOAT cnorm2F(FLOAT complex z) {
	FLOAT real = crealF(z);
	FLOAT imag = cimagF(z);
	
	return (real*real+imag*imag);
}

/* Function assumes that y >= 1 and compute x^y. */
static inline FLOAT complex ciPowF(FLOAT complex x, uint_fast32_t y)
{
	FLOAT complex rem = 1, res = x;
	do {
		if (y % 2) {
			rem *= x;
			--y;
		}
		y >>= 1;
		res *= res;
	} while (y > 1);

	return res*rem;
}

static inline void MandelbrotJuliaSimpleLoop(Fractal *fractal, FractalOrbit *orbit,
						FLOAT complex z0, FLOAT complex c)
{
	FLOAT complex z = z0;
	FLOAT normZ2 = cnorm2F(z);
	uint_fast32_t N;
	for (N=0; N<fractal->maxIter && normZ2 < fractal->escapeRadius2; N++) {
		z = z*z+c;
		normZ2 = cnorm2F(z);
	}
	orbit->N = N;
	orbit->rn[N] = sqrtF(normZ2);
}

static inline void MandelbrotJuliaPIntSimpleLoop(Fractal *fractal, FractalOrbit *orbit,
							FLOAT complex z0, FLOAT complex c)
{
	FLOAT complex z = z0;
	FLOAT normZ2 = cnorm2F(z);
	uint_fast32_t N;
	for (N=0; N<fractal->maxIter && normZ2 < fractal->escapeRadius2; N++) {
		z = ciPowF(z,fractal->p_int)+c;
		normZ2 = cnorm2F(z);
	}
	orbit->N = N;
	orbit->rn[N] = sqrtF(normZ2);
}

static inline void MandelbrotJuliaPFloatSimpleLoop(Fractal *fractal, FractalOrbit *orbit,
							FLOAT complex z0, FLOAT complex c)
{
	FLOAT complex z = z0;
	FLOAT normZ2 = cnorm2F(z);
	uint_fast32_t N;
	for (N=0; N<fractal->maxIter && normZ2 < fractal->escapeRadius2; N++) {
		z = cpowF(z,fractal->p)+c;
		normZ2 = cnorm2F(z);
	}
	orbit->N = N;
	orbit->rn[N] = sqrtF(normZ2);
}

static inline void MandelbrotJuliaAverageLoop(Fractal *fractal, FractalOrbit *orbit,
						FLOAT complex z0, FLOAT complex c)
{
	FLOAT complex z = z0;
	FLOAT normZ = cabsF(z);
	FLOAT complex zP = 0;
	FLOAT normZP = 0;
	FLOAT normC = cabsF(c);
	uint_fast32_t N;
	FLOAT *rn = orbit->rn;
	FLOAT *mn = orbit->mn;
	FLOAT *Mn = orbit->Mn;
	rn[0] = normZ;
	mn[0] = 0;
	Mn[0] = 0;

	for (N=1; N<=fractal->maxIter && normZ < fractal->escapeRadius; N++) {
		zP = z*z;
		normZP = cabsF(zP);
		mn[N] = fabsF(normZP-normC);
		Mn[N] = normZP+normC;
		z = zP+c;
		normZ = cabsF(z);
		rn[N] = normZ;
	}
	--N;
	orbit->N = N;
}

static inline void MandelbrotJuliaPIntAverageLoop(Fractal *fractal, FractalOrbit *orbit,
							FLOAT complex z0, FLOAT complex c)
{
	FLOAT complex z = z0;
	FLOAT normZ = cabsF(z);
	FLOAT complex zP = 0;
	FLOAT normZP = 0;
	FLOAT normC = cabsF(c);
	uint_fast32_t N;
	FLOAT *rn = orbit->rn;
	FLOAT *mn = orbit->mn;
	FLOAT *Mn = orbit->Mn;
	rn[0] = normZ;
	mn[0] = 0;
	Mn[0] = 0;

	for (N=1; N<=fractal->maxIter && normZ < fractal->escapeRadius; N++) {
		zP = ciPowF(z, fractal->p_int);
		normZP = cabsF(zP);
		mn[N] = fabsF(normZP-normC);
		Mn[N] = normZP+normC;
		z = zP+c;
		normZ = cabsF(z);
		rn[N] = normZ;
	}
	--N;
	orbit->N = N;
}

static inline void MandelbrotJuliaPFloatAverageLoop(Fractal *fractal, FractalOrbit *orbit,
							FLOAT complex z0, FLOAT complex c)
{
	FLOAT complex z = z0;
	FLOAT normZ = cabsF(z);
	FLOAT complex zP = 0;
	FLOAT normZP = 0;
	FLOAT normC = cabsF(c);
	uint_fast32_t N;
	FLOAT *rn = orbit->rn;
	FLOAT *mn = orbit->mn;
	FLOAT *Mn = orbit->Mn;
	rn[0] = normZ;
	mn[0] = 0;
	Mn[0] = 0;

	for (N=1; N<=fractal->maxIter && normZ < fractal->escapeRadius; N++) {
		zP = cpowF(z, fractal->p);
		normZP = cabsF(zP);
		mn[N] = fabsF(normZP-normC);
		Mn[N] = normZP+normC;
		z = zP+c;
		normZ = cabsF(z);
		rn[N] = normZ;
	}
	--N;
	orbit->N = N;
}

void MandelbrotSimpleLoop(Fractal *fractal, FractalOrbit *orbit, FLOAT complex z)
{
	MandelbrotJuliaSimpleLoop(fractal, orbit, 0, z);
}

void MandelbrotPIntSimpleLoop(Fractal *fractal, FractalOrbit *orbit, FLOAT complex z)
{
	MandelbrotJuliaPIntSimpleLoop(fractal, orbit, 0, z);
}

void MandelbrotPFloatSimpleLoop(Fractal *fractal, FractalOrbit *orbit, FLOAT complex z)
{
	MandelbrotJuliaPFloatSimpleLoop(fractal, orbit, 0, z);
}

void MandelbrotAverageLoop(Fractal *fractal, FractalOrbit *orbit, FLOAT complex z)
{
	MandelbrotJuliaAverageLoop(fractal, orbit, 0, z);
}

void MandelbrotPIntAverageLoop(Fractal *fractal, FractalOrbit *orbit, FLOAT complex z)
{
	MandelbrotJuliaPIntAverageLoop(fractal, orbit, 0, z);
}

void MandelbrotPFloatAverageLoop(Fractal *fractal, FractalOrbit *orbit, FLOAT complex z)
{
	MandelbrotJuliaPFloatAverageLoop(fractal, orbit, 0, z);
}

void JuliaSimpleLoop(Fractal *fractal, FractalOrbit *orbit, FLOAT complex z)
{
	MandelbrotJuliaSimpleLoop(fractal, orbit, z, fractal->c);
}

void JuliaPIntSimpleLoop(Fractal *fractal, FractalOrbit *orbit, FLOAT complex z)
{
	MandelbrotJuliaPIntSimpleLoop(fractal, orbit, z, fractal->c);
}

void JuliaPFloatSimpleLoop(Fractal *fractal, FractalOrbit *orbit, FLOAT complex z)
{
	MandelbrotJuliaPFloatSimpleLoop(fractal, orbit, z, fractal->c);
}

void JuliaAverageLoop(Fractal *fractal, FractalOrbit *orbit, FLOAT complex z)
{
	MandelbrotJuliaAverageLoop(fractal, orbit, z, fractal->c);
}

void JuliaPIntAverageLoop(Fractal *fractal, FractalOrbit *orbit, FLOAT complex z)
{
	MandelbrotJuliaPIntAverageLoop(fractal, orbit, z, fractal->c);
}

void JuliaPFloatAverageLoop(Fractal *fractal, FractalOrbit *orbit, FLOAT complex z)
{
	MandelbrotJuliaPFloatAverageLoop(fractal, orbit, z, fractal->c);
}

void RudyPIntSimpleLoop(Fractal *fractal, FractalOrbit *orbit, FLOAT complex z)
{
	FLOAT complex c = z;
	z = 0;
	FLOAT normZ2 = cnorm2F(z);
	uint_fast32_t N;
	for (N=0; N<fractal->maxIter && normZ2 < fractal->escapeRadius2; N++) {
		z = ciPowF(z,fractal->p_int)+fractal->c*z+c;
		normZ2 = cnorm2F(z);
	}
	orbit->N = N;
	orbit->rn[N] = sqrtF(normZ2);
}

void RudyPFloatSimpleLoop(Fractal *fractal, FractalOrbit *orbit, FLOAT complex z)
{
	FLOAT complex c = z;
	z = 0;
	FLOAT normZ2 = cnorm2F(z);
	uint_fast32_t N;
	for (N=0; N<fractal->maxIter && normZ2 < fractal->escapeRadius2; N++) {
		z = cpowF(z,fractal->p)+fractal->c*z+c;
		normZ2 = cnorm2F(z);
	}
	orbit->N = N;
	orbit->rn[N] = sqrtF(normZ2);
}

void RudyPIntAverageLoop(Fractal *fractal, FractalOrbit *orbit, FLOAT complex z)
{
	FLOAT complex c = z;
	z = 0;
	FLOAT normZ = cabsF(z);
	FLOAT complex zP = 0;
	FLOAT normZP = 0;
	FLOAT normC = cabsF(c);
	uint_fast32_t N;
	FLOAT *rn = orbit->rn;
	FLOAT *mn = orbit->mn;
	FLOAT *Mn = orbit->Mn;
	rn[0] = 0;
	mn[0] = 0;
	Mn[0] = 0;

	for (N=1; N<=fractal->maxIter && normZ < fractal->escapeRadius; N++) {
		zP = ciPowF(z,fractal->p_int)+fractal->c*z;
		normZP = cabsF(zP);
		mn[N] = fabsF(normZP-normC);
		Mn[N] = normZP+normC;
		z = zP+c;
		normZ = cabsF(z);
		rn[N] = normZ;
	}
	--N;
	orbit->N = N;
}

void RudyPFloatAverageLoop(Fractal *fractal, FractalOrbit *orbit, FLOAT complex z)
{
	FLOAT complex c = z;
	z = 0;
	FLOAT normZ = cabsF(z);
	FLOAT complex zP = 0;
	FLOAT normZP = 0;
	FLOAT normC = cabsF(c);
	uint_fast32_t N;
	FLOAT *rn = orbit->rn;
	FLOAT *mn = orbit->mn;
	FLOAT *Mn = orbit->Mn;
	rn[0] = 0;
	mn[0] = 0;
	Mn[0] = 0;

	for (N=1; N<=fractal->maxIter && normZ < fractal->escapeRadius; N++) {
		zP = cpowF(z,fractal->p)+fractal->c*z;
		normZP = cabsF(zP);
		mn[N] = fabsF(normZP-normC);
		Mn[N] = normZP+normC;
		z = zP+c;
		normZ = cabsF(z);
		rn[N] = normZ;
	}
	--N;
	orbit->N = N;
}

FractalLoop GetFractalLoop(FractalType fractalType, ColoringMethod coloringMethod,
				int p_is_integer)
{
	FractalLoop res;

	switch (fractalType) {
	case FRAC_MANDELBROT:
		if (coloringMethod == CM_SIMPLE) {
			res = MandelbrotSimpleLoop;
		} else {
			res = MandelbrotAverageLoop;
		}
		break;
	case FRAC_JULIA:
		if (coloringMethod == CM_SIMPLE) {
			res = JuliaSimpleLoop;
		} else {
			res = JuliaAverageLoop;
		}
		break;
	case FRAC_MANDELBROTP:
		if (coloringMethod == CM_SIMPLE) {
			if (p_is_integer) {
				res = MandelbrotPIntSimpleLoop;
			} else {
				res = MandelbrotPFloatSimpleLoop;
			}
		} else {
			if (p_is_integer) {
				res = MandelbrotPIntAverageLoop;
			} else {
				res = MandelbrotPFloatAverageLoop;
			}
		}
		break;
	case FRAC_JULIAP:
		if (coloringMethod == CM_SIMPLE) {
			if (p_is_integer) {
				res = JuliaPIntSimpleLoop;
			} else {
				res = JuliaPFloatSimpleLoop;
			}
		} else {
			if (p_is_integer) {
				res = JuliaPIntAverageLoop;
			} else {
				res = JuliaPFloatAverageLoop;
			}
		}
		break;
	case FRAC_RUDY:
		if (coloringMethod == CM_SIMPLE) {
			if (p_is_integer) {
				res = RudyPIntSimpleLoop;
			} else {
				res = RudyPFloatSimpleLoop;
			}
		} else {
			if (p_is_integer) {
				res = RudyPIntAverageLoop;
			} else {
				res = RudyPFloatAverageLoop;
			}
		}
		break;
	default:
		error("Unknown fractal type.\n");
		break;
	}

	return res;
}
