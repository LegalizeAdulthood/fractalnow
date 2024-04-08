/*
 *  fractal_formula.h -- part of fractal2D
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
  * \file fractal_formula.h
  * \brief Header file related to fractal formulas and loop iterations.
  * \author Marc Pegon
  */
 
#ifndef __FRACTAL_FORMULA_H__
#define __FRACTAL_FORMULA_H__

#ifdef __cplusplus
extern "C" {
#endif

#include "floating_point.h"
#include <stdint.h>

/**
 * \enum e_FractalFormula
 * \brief Fractal formula.
 */
typedef enum e_FractalFormula {
	FRAC_MANDELBROT = 0,
 /*!< Mandelbrot fractal.*/
	FRAC_MANDELBROTP,
 /*!< Mandelbrot type fractal with custom p parameter.*/
	FRAC_JULIA,
 /*!< Julia fractal.*/
	FRAC_JULIAP,
 /*!< Julia type fractal with custom p parameter.*/
	FRAC_RUDY
 /*!< Rudy fractal (with custom p parameter).*/
} FractalFormula;

/**
 * \var FractalFormulaStr
 * \brief Strings of fractal formulas.
 */
extern const char *FractalFormulaStr[];

/**
 * \fn FractalFormula GetFractalFormula(const char *str)
 * \brief Get fractal formula from string.
 *
 * Function is case insensitive.
 * Possible strings are :
 * - "mandelbrot" for FRAC_MANDELBROT
 * - "mandelbrotp" for FRAC_MANDELBROTP
 * - "julia" for FRAC_JULIA
 * - "juliap" for FRAC_JULIAP
 * - "rudy" for FRAC_RUDY
 * 
 * Exit with error in case of failure (unknown fractal type).
 *
 * \param str String specifying fractal formula.
 * \return Corresponding fractal formula.
 */
FractalFormula GetFractalFormula(const char *str);

/* Function assumes that y >= 1 and computes x^y. */
static inline FLOAT complex cipowF(FLOAT complex x, uint_fast32_t y)
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

#define cpowPINT(x,y) cipowF(x,y)
#define cpowPFLOAT(x,y) cpowF(x,y)

/*********************FRAC_MANDELBROT*********************/
#define LOOP_INIT_FRAC_MANDELBROT \
z = 0;\
c = pixel;

#define LOOP_ITERATION_FRAC_MANDELBROT(ptype) \
z = z*z+c;
/*********************************************************/


/*********************FRAC_MANDELBROTP********************/
#define LOOP_INIT_FRAC_MANDELBROTP \
z = 0;\
c = pixel;

#define LOOP_ITERATION_FRAC_MANDELBROTP(ptype) \
z = cpow##ptype(z,fractal->p)+c;
/*********************************************************/


/***********************FRAC_JULIA************************/
#define LOOP_INIT_FRAC_JULIA \
z = pixel;\
c = fractal->c;

#define LOOP_ITERATION_FRAC_JULIA(ptype) \
z = z*z+c;
/*********************************************************/


/***********************FRAC_JULIAP***********************/
#define LOOP_INIT_FRAC_JULIAP \
z = pixel;\
c = fractal->c;

#define LOOP_ITERATION_FRAC_JULIAP(ptype) \
z = cpow##ptype(z,fractal->p)+c;
/*********************************************************/


/************************FRAC_RUDY************************/
#define LOOP_INIT_FRAC_RUDY \
z = 0;\
c = pixel;

#define LOOP_ITERATION_FRAC_RUDY(ptype) \
z = cpow##ptype(z,fractal->p)+fractal->c*z+c;
/*********************************************************/

#ifdef __cplusplus
}
#endif

#endif
