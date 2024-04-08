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

#include "float_precision.h"
#include <complex.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * \enum e_FractalFormula
 * \brief Fractal formula.
 */
/**
 * \typedef FractalFormula
 * \brief Convenient typedef for enum e_FractalFormula.
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
 * - "mandelbrotp" for FRAC_MULTIBROT
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

#define cpowPINT(fprec,res,x,y) cipowF(fprec,res,x,y)
#define cpowPFLOATT(fprec,res,x,y) cpowF(fprec,res,x,y)
#define cpowPTYPE(ptype,fprec,res,x,y) cpow##ptype(fprec,res,x,y)

#define fractalP_PINT data->fractalP_INT
#define fractalP_PFLOATT data->fractalP
#define fractalP(ptype) fractalP_##ptype

/*********************FRAC_MANDELBROT*********************/
#define ENGINE_DECL_VAR_FRAC_MANDELBROT(fprec)

#define ENGINE_INIT_VAR_FRAC_MANDELBROT(fprec) \
(void)NULL;

#define ENGINE_CLEAR_VAR_FRAC_MANDELBROT(fprec) \
(void)NULL;

#define LOOP_INIT_FRAC_MANDELBROT(fprec) \
cfromUiF(fprec, data->z, 0);\
cassignF(fprec, data->c, data->pixel);

#define LOOP_ITERATION_FRAC_MANDELBROT(ptype,fprec) \
csqrF(fprec,data->z,data->z);\
caddF(fprec,data->z,data->z,data->c);
/*********************************************************/

/**********************FRAC_MULTIBROT*********************/
#define ENGINE_DECL_VAR_FRAC_MULTIBROT(fprec)

#define ENGINE_INIT_VAR_FRAC_MULTIBROT(fprec) \
(void)NULL;

#define ENGINE_CLEAR_VAR_FRAC_MULTIBROT(fprec) \
(void)NULL;

#define LOOP_INIT_FRAC_MULTIBROT(fprec) \
cfromUiF(fprec, data->z, 0);\
cassignF(fprec, data->c, data->pixel);

#define LOOP_ITERATION_FRAC_MULTIBROT(ptype,fprec) \
cpowPTYPE(ptype,fprec,data->z,data->z,fractalP(ptype));\
caddF(fprec,data->z,data->z,data->c);
/*********************************************************/

/***********************FRAC_JULIA************************/
#define ENGINE_DECL_VAR_FRAC_JULIA(fprec)

#define ENGINE_INIT_VAR_FRAC_JULIA(fprec) \
(void)NULL;

#define ENGINE_CLEAR_VAR_FRAC_JULIA(fprec) \
(void)NULL;

#define LOOP_INIT_FRAC_JULIA(fprec) \
cassignF(fprec, data->z, data->pixel);\
cassignF(fprec, data->c, data->fractalC);

#define LOOP_ITERATION_FRAC_JULIA(ptype,fprec) \
csqrF(fprec,data->z,data->z);\
caddF(fprec,data->z,data->z,data->c);
/*********************************************************/

/*********************FRAC_MULTIJULIA*********************/
#define ENGINE_DECL_VAR_FRAC_MULTIJULIA(fprec)

#define ENGINE_INIT_VAR_FRAC_MULTIJULIA(fprec) \
(void)NULL;

#define ENGINE_CLEAR_VAR_FRAC_MULTIJULIA(fprec) \
(void)NULL;

#define LOOP_INIT_FRAC_MULTIJULIA(fprec) \
cassignF(fprec, data->z, data->pixel);\
cassignF(fprec, data->c, data->fractalC);

#define LOOP_ITERATION_FRAC_MULTIJULIA(ptype,fprec) \
cpowPTYPE(ptype,fprec,data->z,data->z,fractalP(ptype));\
caddF(fprec,data->z,data->z,data->c);
/*********************************************************/

/*********************FRAC_BURNINGSHIP********************/
#define ENGINE_DECL_VAR_FRAC_BURNINGSHIP(fprec) \
FLOATTYPE(fprec) absRealZ_FRAC;\
FLOATTYPE(fprec) absImagZ_FRAC;

#define ENGINE_INIT_VAR_FRAC_BURNINGSHIP(fprec) \
initF(fprec, data->absRealZ_FRAC);\
initF(fprec, data->absImagZ_FRAC);

#define ENGINE_CLEAR_VAR_FRAC_BURNINGSHIP(fprec) \
clearF(fprec, data->absRealZ_FRAC);\
clearF(fprec, data->absImagZ_FRAC);

#define LOOP_INIT_FRAC_BURNINGSHIP(fprec) \
cfromUiF(fprec, data->z, 0);\
cassignF(fprec, data->c, data->pixel);

#define LOOP_ITERATION_FRAC_BURNINGSHIP(ptype,fprec) \
crealF(fprec, data->absRealZ_FRAC, data->z);\
fabsF(fprec, data->absRealZ_FRAC, data->absRealZ_FRAC);\
cimagF(fprec, data->absImagZ_FRAC, data->z);\
fabsF(fprec, data->absImagZ_FRAC, data->absImagZ_FRAC);\
cfromReImF(fprec, data->z, data->absRealZ_FRAC, data->absImagZ_FRAC);\
cpowPTYPE(ptype,fprec,data->z,data->z,fractalP(ptype));\
caddF(fprec,data->z,data->z,data->c);
/*********************************************************/

/*******************FRAC_JULIABURNINGSHIP******************/
#define ENGINE_DECL_VAR_FRAC_JULIABURNINGSHIP(fprec) \
FLOATTYPE(fprec) absRealZ_FRAC;\
FLOATTYPE(fprec) absImagZ_FRAC;

#define ENGINE_INIT_VAR_FRAC_JULIABURNINGSHIP(fprec) \
initF(fprec, data->absRealZ_FRAC);\
initF(fprec, data->absImagZ_FRAC);

#define ENGINE_CLEAR_VAR_FRAC_JULIABURNINGSHIP(fprec) \
clearF(fprec, data->absRealZ_FRAC);\
clearF(fprec, data->absImagZ_FRAC);

#define LOOP_INIT_FRAC_JULIABURNINGSHIP(fprec) \
cassignF(fprec, data->z, data->pixel);\
cassignF(fprec, data->c, data->fractalC);

#define LOOP_ITERATION_FRAC_JULIABURNINGSHIP(ptype,fprec) \
crealF(fprec, data->absRealZ_FRAC, data->z);\
fabsF(fprec, data->absRealZ_FRAC, data->absRealZ_FRAC);\
cimagF(fprec, data->absImagZ_FRAC, data->z);\
fabsF(fprec, data->absImagZ_FRAC, data->absImagZ_FRAC);\
cfromReImF(fprec, data->z, data->absRealZ_FRAC, data->absImagZ_FRAC);\
cpowPTYPE(ptype,fprec,data->z,data->z,fractalP(ptype));\
caddF(fprec,data->z,data->z,data->c);
/*********************************************************/

/**********************FRAC_MANDELBAR*********************/
#define ENGINE_DECL_VAR_FRAC_MANDELBAR(fprec)

#define ENGINE_INIT_VAR_FRAC_MANDELBAR(fprec) \
(void)NULL;

#define ENGINE_CLEAR_VAR_FRAC_MANDELBAR(fprec) \
(void)NULL;

#define LOOP_INIT_FRAC_MANDELBAR(fprec) \
cfromUiF(fprec, data->z, 0);\
cassignF(fprec, data->c, data->pixel);

#define LOOP_ITERATION_FRAC_MANDELBAR(ptype,fprec) \
conjF(fprec, data->z, data->z);\
cpowPTYPE(ptype,fprec,data->z,data->z,fractalP(ptype));\
caddF(fprec,data->z,data->z,data->c);
/*********************************************************/

/**********************FRAC_JULIABAR**********************/
#define ENGINE_DECL_VAR_FRAC_JULIABAR(fprec)

#define ENGINE_INIT_VAR_FRAC_JULIABAR(fprec) \
(void)NULL;

#define ENGINE_CLEAR_VAR_FRAC_JULIABAR(fprec) \
(void)NULL;

#define LOOP_INIT_FRAC_JULIABAR(fprec) \
cassignF(fprec, data->z, data->pixel);\
cassignF(fprec, data->c, data->fractalC);

#define LOOP_ITERATION_FRAC_JULIABAR(ptype,fprec) \
conjF(fprec, data->z, data->z);\
cpowPTYPE(ptype,fprec,data->z,data->z,fractalP(ptype));\
caddF(fprec,data->z,data->z,data->c);
/*********************************************************/

/************************FRAC_RUDY************************/
#define ENGINE_DECL_VAR_FRAC_RUDY(fprec) \
COMPLEX_FLOATTYPE(fprec) cz_FRAC;

#define ENGINE_INIT_VAR_FRAC_RUDY(fprec) \
cinitF(fprec, data->cz_FRAC);

#define ENGINE_CLEAR_VAR_FRAC_RUDY(fprec) \
cclearF(fprec, data->cz_FRAC);

#define LOOP_INIT_FRAC_RUDY(fprec) \
cfromUiF(fprec, data->z, 0);\
cassignF(fprec, data->c, data->pixel);

#define LOOP_ITERATION_FRAC_RUDY(ptype,fprec) \
cmulF(fprec, data->cz_FRAC, data->fractalC, data->z);\
cpowPTYPE(ptype,fprec,data->z,data->z,fractalP(ptype));\
caddF(fprec,data->z,data->z,data->cz_FRAC);\
caddF(fprec,data->z,data->z,data->c);\
/*********************************************************/

#ifdef __cplusplus
}
#endif

#endif
