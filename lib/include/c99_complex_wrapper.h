/*
 *  c99_complex_wrapper.h -- part of FractalNow
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
  * \file c99_complex_wrapper.h
  * \brief Header file for wrapper for standard C99 complex numbers.
  * \author Marc Pegon
  */
 
#ifndef __C99_COMPLEX_WRAPPER_H__
#define __C99_COMPLEX_WRAPPER_H__

#include <stdint.h>
#include <complex.h>

#ifdef __cplusplus
extern "C" {
#endif

#ifndef complex
#define complex _Complex
#endif

/**
 * \fn float cnormf(float complex a)
 * \brief Compute norm of float complex
 *
 * Norm is defined as re*re+im*im.
 *
 * \param a Complex number.
 * \return Norm of a.
 */
float cnormf(float complex a);

/**
 * \fn double cnorm(double complex a)
 * \brief Compute norm of double complex.
 *
 * Norm is defined as re*re+im*im.
 *
 * \param a Complex number.
 * \return Norm of a.
 */
double cnorm(double complex a);

/**
 * \fn long double cnorml(long double complex a)
 * \brief Compute norm of long double complex.
 *
 * Norm is defined as re*re+im*im.
 *
 * \param a Complex number.
 * \return Norm of a.
 */
long double cnorml(long double complex a);

/**
 * \fn float complex cipowf(float complex a, uint_fast32_t b)
 * \brief Compute integral power of float complex a.
 *
 * \param a Complex number.
 * \param b Integral power of return value.
 * \return a to the power b.
 */
float complex cipowf(float complex a, uint_fast32_t b);
double complex cipow(double complex a, uint_fast32_t b);
long double complex cipowl(long double complex a, uint_fast32_t b);

int cisintegerf(float complex a);
int cisinteger(double complex a);
int cisintegerl(long double complex a);

/**************************float**************************/
typedef float complex Complex_f;

#define cbuild_f(re, im) ((re)+I*(im))
#define ceq_f(a, b) ((a)==(b))
#define creal_f(a) crealf(a)
#define cimag_f(a) cimagf(a)
#define conj_f(a) conjf(a)
#define cnorm_f(a) cnormf(a)
#define cabs_f(a) cabsf(a)
#define cadd_f(a,b) ((a)+(b))
#define csub_f(a,b) ((a)-(b))
#define csqr_f(a) ((a)*(a))
#define cmul_f(a,b) ((a)*(b))
#define cdiv_f(a,b) ((a)/(b))
#define carg_f(a) cargf(a)
#define cexp_f(a) cexpf(a)
#define clog_f(a) clogf(a)
#define cipow_f(a,b) cipowf(a,b)
#define cpow_f(a,b) cpowf(a,b)
#define cisinteger_f(a) cisintegerf(a)
/*********************************************************/

/**************************double*************************/
typedef double complex Complex_;

#define cbuild_(re, im) ((re)+I*(im))
#define ceq_(a, b) ((a)==(b))
#define creal_(a) creal(a)
#define cimag_(a) cimag(a)
#define conj_(a) conj(a)
#define cnorm_(a) cnorm(a)
#define cabs_(a) cabs(a)
#define cadd_(a,b) ((a)+(b))
#define csub_(a,b) ((a)-(b))
#define csqr_(a) ((a)*(a))
#define cmul_(a,b) ((a)*(b))
#define cdiv_(a,b) ((a)/(b))
#define carg_(a) carg(a)
#define cexp_(a) cexp(a)
#define clog_(a) clog(a)
#define cipow_(a,b) cipow(a,b)
#define cpow_(a,b) cpow(a,b)
#define cisinteger_(a) cisinteger(a)
/*********************************************************/

/***********************long double***********************/
typedef long double complex Complex_l;

#define cbuild_l(re, im) ((re)+I*(im))
#define ceq_l(a, b) ((a)==(b))
#define creal_l(a) creall(a)
#define cimag_l(a) cimagl(a)
#define conj_l(a) conjl(a)
#define cnorm_l(a) cnorml(a)
#define cabs_l(a) cabsl(a)
#define cadd_l(a,b) ((a)+(b))
#define csub_l(a,b) ((a)-(b))
#define csqr_l(a) ((a)*(a))
#define cmul_l(a,b) ((a)*(b))
#define cdiv_l(a,b) ((a)/(b))
#define carg_l(a) cargl(a)
#define cexp_l(a) cexpl(a)
#define clog_l(a) clogl(a)
#define cipow_l(a,b) cipowl(a,b)
#define cpow_l(a,b) cpowl(a,b)
#define cisinteger_l(a) cisintegerl(a)
/*********************************************************/

#ifdef __cplusplus
}
#endif

#endif
