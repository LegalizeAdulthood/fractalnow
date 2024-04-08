/*
 *  fractal_formula.h -- part of FractalNow
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
#include <complex.h>
#include <stdint.h>

#ifndef complex
#define complex _Complex
#endif

/**
 * \enum e_FractalFormula
 * \brief Fractal formula.
 */
/**
 * \typedef FractalFormula
 * \brief Convenient typedef for enum FractalFormula.
 */
typedef enum e_FractalFormula {
	FRAC_MANDELBROT = 0,
 /*!< Mandelbrot fractal.*/
	FRAC_MULTIBROT,
 /*!< Mandelbrot type fractal with custom p parameter.*/
	FRAC_JULIA,
 /*!< Julia fractal.*/
	FRAC_MULTIJULIA,
 /*!< Julia type fractal with custom p parameter.*/
	FRAC_BURNINGSHIP,
 /*!< Burning ship.*/
	FRAC_JULIABURNINGSHIP,
 /*!< Julia burning ship.*/
	FRAC_MANDELBAR,
 /*!< Mandelbar.*/
 	FRAC_JULIABAR,
 /*!< Juliabar.*/
	FRAC_RUDY
 /*!< Rudy fractal (with custom p parameter).*/
} FractalFormula;

/**
 * \var nbFractalFormulas
 * \brief Number of fractal formulas.
 */
extern const uint_fast32_t nbFractalFormulas;

/**
 * \var fractalFormulaStr
 * \brief Strings of fractal formulas.
 */
extern const char *fractalFormulaStr[];

/**
 * \var fractalFormulaDescStr
 * \brief More descriptive strings for fractal formulas.
 */
extern const char *fractalFormulaDescStr[];

/**
 * \fn int GetFractalFormula(FractalFormula *fractalFormula, const char *str)
 * \brief Get fractal formula from string.
 *
 * Function is case insensitive.
 * Possible strings are :
 * - "mandelbrot" for FRAC_MANDELBROT
 * - "mandelbrotp" for FRAC_MANDELBROTP
 * - "julia" for FRAC_JULIA
 * - "juliap" for FRAC_JULIAP
 * - "burningship" for FRAC_BURNINGSHIP
 * - "juliaburningship" for FRAC_JULIABURNINGSHIP
 * - "mandelbar" for FRAC_MANDELBAR
 * - "juliabar" for FRAC_JULIABAR
 * - "rudy" for FRAC_RUDY
 *
 * \param fractalFormula Fractal formula destination.
 * \param str String specifying fractal formula.
 * \return 0 in case of success, 1 in case of failure.
 */
int GetFractalFormula(FractalFormula *fractalFormula, const char *str);

/* Function assumes that y >= 1 and computes x^y. */
static inline FLOATT complex cipowF(FLOATT complex x, uint_fast32_t y)
{
	if (y == 0) {
		return 1;
	}

	FLOATT complex rem = 1, res = x;
	while (y > 1) {
		if (y % 2) {
			rem *= res;
			--y;
		}
		y >>= 1;
		res *= res;
	}

	return res*rem;
}

#define cpowPINT(x,y) cipowF(x,y)
#define cpowPFLOATT(x,y) cpowF(x,y)

/*********************FRAC_MANDELBROT*********************/
#define LOOP_INIT_FRAC_MANDELBROT \
z = 0;\
c = pixel;

#define LOOP_ITERATION_FRAC_MANDELBROT(ptype) \
z = z*z+c;
/*********************************************************/


/**********************FRAC_MULTIBROT*********************/
#define LOOP_INIT_FRAC_MULTIBROT \
z = 0;\
c = pixel;

#define LOOP_ITERATION_FRAC_MULTIBROT(ptype) \
z = cpow##ptype(z,fractal->p##ptype)+c;
/*********************************************************/


/***********************FRAC_JULIA************************/
#define LOOP_INIT_FRAC_JULIA \
z = pixel;\
c = fractal->c;

#define LOOP_ITERATION_FRAC_JULIA(ptype) \
z = z*z+c;
/*********************************************************/


/*********************FRAC_MULTIJULIA*********************/
#define LOOP_INIT_FRAC_MULTIJULIA \
z = pixel;\
c = fractal->c;

#define LOOP_ITERATION_FRAC_MULTIJULIA(ptype) \
z = cpow##ptype(z,fractal->p##ptype)+c;
/*********************************************************/

/*********************FRAC_BURNINGSHIP********************/
#define LOOP_INIT_FRAC_BURNINGSHIP \
z = 0;\
c = pixel;

#define LOOP_ITERATION_FRAC_BURNINGSHIP(ptype) \
z = cpow##ptype(fabsF(crealF(z))+I*fabsF(cimagF(z)),fractal->p##ptype)+c;
/*********************************************************/

/*******************FRAC_JULIABURNINGSHIP******************/
#define LOOP_INIT_FRAC_JULIABURNINGSHIP \
z = pixel;\
c = fractal->c;

#define LOOP_ITERATION_FRAC_JULIABURNINGSHIP(ptype) \
z = cpow##ptype(fabsF(crealF(z))+I*fabsF(cimagF(z)),fractal->p##ptype)+c;
/*********************************************************/

/**********************FRAC_MANDELBAR*********************/
#define LOOP_INIT_FRAC_MANDELBAR \
z = 0;\
c = pixel;

#define LOOP_ITERATION_FRAC_MANDELBAR(ptype) \
z = cpow##ptype(conjF(z),fractal->p##ptype)+c;
/*********************************************************/

/**********************FRAC_JULIABAR**********************/
#define LOOP_INIT_FRAC_JULIABAR \
z = pixel;\
c = fractal->c;

#define LOOP_ITERATION_FRAC_JULIABAR(ptype) \
z = cpow##ptype(conjF(z),fractal->p##ptype)+c;
/*********************************************************/

/************************FRAC_RUDY************************/
#define LOOP_INIT_FRAC_RUDY \
z = 0;\
c = pixel;

#define LOOP_ITERATION_FRAC_RUDY(ptype) \
z = cpow##ptype(z,fractal->p##ptype)+fractal->c*z+c;
/*********************************************************/

#ifdef __cplusplus
}
#endif

#endif
