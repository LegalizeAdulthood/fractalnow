/*
 *  float_precision.h -- part of FractalNow
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
  * \file float_precision.h
  * \brief Header file related to floating point types for different precision.
  *
  * \author Marc Pegon
  */

#ifndef __FLOAT_PRECISION_H__
#define __FLOAT_PRECISION_H__

#include "complex_wrapper.h"
#include <stdlib.h>
#include <inttypes.h>
#include <math.h>

#ifdef _ENABLE_MP_FLOATS
#include <stdio.h>
#include <mpfr.h>
#include <mpc.h>
#endif

#ifdef __cplusplus
extern "C" {
#endif

/**
 * \enum e_FloatPrecision
 * \brief Supported float precisions (types).
 */
/**
 * \typedef FloatPrecision
 * \brief Convenient typedef for enum e_FloatPrecision.
 */
typedef enum e_FloatPrecision {
	FP_SINGLE = 0,
 /*!< Single precision (float).*/
	FP_DOUBLE,
 /*!< Double precision (double).*/
#ifdef _ENABLE_LDOUBLE_FLOATS
	FP_LDOUBLE,
#endif
 /*!< Extended precision (long double).*/
#ifdef _ENABLE_MP_FLOATS
	FP_MP
#endif
 /* Multiple precision (mpfr_t and mpc_t).*/
} FloatPrecision;

#define FLOATTYPE_FP_SINGLE float
#define COMPLEX_FLOATTYPE_FP_SINGLE Complex_f
#define FLOATTYPE_FP_DOUBLE double
#define COMPLEX_FLOATTYPE_FP_DOUBLE Complex_
#define FLOATTYPE_FP_LDOUBLE long double
#define COMPLEX_FLOATTYPE_FP_LDOUBLE Complex_l
#define FLOATTYPE_FP_MP mpfr_t
#define COMPLEX_FLOATTYPE_FP_MP mpc_t

#define FLOATTYPE(fprec) FLOATTYPE_##fprec
#define COMPLEX_FLOATTYPE(fprec) COMPLEX_FLOATTYPE_##fprec

/**
 * \union MultiFloat
 * \brief Union for different types of float.
 */
/**
 * \typedef MultiFloat
 * \brief Convenient typedef for union MultiFloat.
 */
typedef union MultiFloat {
	FLOATTYPE(FP_SINGLE) val_FP_SINGLE;
 /*!< float type value.*/
	FLOATTYPE(FP_DOUBLE) val_FP_DOUBLE;
#ifdef _ENABLE_LDOUBLE_FLOATS
 /*!< double type value.*/
	FLOATTYPE(FP_LDOUBLE) val_FP_LDOUBLE;
#endif
#ifdef _ENABLE_MP_FLOATS
 /*!< long double type value.*/
	FLOATTYPE(FP_MP) val_FP_MP;
 /*!< mpfr_t type value.*/
#endif
} MultiFloat;

/**
 * \var nbFloatPrecisions
 * \brief Number of diffenret float precisions.
 */
extern const uint_fast32_t nbFloatPrecisions;

/**
 * \var floatPrecisionStr
 * \brief Strings of float precisions.
 */
extern const char *floatPrecisionStr[];

/**
 * \var floatPrecisionDescStr
 * \brief More descriptive strings for float precisions.
 */
extern const char *floatPrecisionDescStr[];

/**
 * \fn int GetFloatPrecision(FloatPrecision *floatPrecision, const char *str)
 * \brief Get float precision from string.
 *
 * Function is case insensitive.
 * Possible strings are :
 * - "single" for FP_SINGLE
 * - "double" for FP_DOUBLE
 * - "ldouble" for FP_LDOUBLE
 * - "mp" for FP_MP if MP floats are enabled
 *
 * \param floatPrecision Float precision destination.
 * \param str String specifying float precision.
 * \return 0 in case of success, 1 in case of failure.
 */
int GetFloatPrecision(FloatPrecision *floatPrecision, const char *str);

#ifdef _ENABLE_MP_FLOATS
#define DEFAULT_MP_PRECISION (int64_t)(128)

extern int64_t fractalnow_mp_precision;

/**
 * \fn int64_t GetMinMPFloatPrecision()
 * \brief Get minimum precision of MP floats.
 *
 * \return Minimum precision of Multiple Precision floats.
 */
int64_t GetMinMPFloatPrecision();

/**
 * \fn int64_t GetMaxMPFloatPrecision()
 * \brief Get maximum precision of MP floats.
 *
 * \return Maximum precision of Multiple Precision floats.
 */
int64_t GetMaxMPFloatPrecision();

/**
 * \fn int64_t GetMPFloatPrecision()
 * \brief Get current precision of MP floats.
 *
 * \return Current precision of Multiple Precision floats.
 */
int64_t GetMPFloatPrecision();

/**
 * \fn void SetMPFloatPrecision(int64_t precision);
 * \brief Set precision of MP floats.
 *
 * Does nothing if precision is not between minimum and maximum precision.
 *
 * \param precision New Multiple Float precision.
 */
void SetMPFloatPrecision(int64_t precision);
#endif

/************************FP_SINGLE************************/
#define init_FP_SINGLE(x) (void)NULL
#define clear_FP_SINGLE(x) (void)NULL
#define cinit_FP_SINGLE(x) (void)NULL
#define cclear_FP_SINGLE(x) (void)NULL

#define assign_FP_SINGLE(dst,src) dst=src
#define cassign_FP_SINGLE(dst,src) dst=src

#define fromMPFR_FP_SINGLE(dst,src) dst=mpfr_get_flt(src,MPFR_RNDN)
#define fromLDouble_FP_SINGLE(dst,src) dst=(FLOATTYPE_FP_SINGLE)src
#define fromDouble_FP_SINGLE(dst,src) dst=(FLOATTYPE_FP_SINGLE)src
#define fromUi_FP_SINGLE(dst,src) dst=(FLOATTYPE_FP_SINGLE)src
#define fromSi_FP_SINGLE(dst,src) dst=(FLOATTYPE_FP_SINGLE)src
#define toDouble_FP_SINGLE(src) (long double)src

#define cfromMPC_FP_SINGLE(dst,src) dst=cbuild_f(mpfr_get_flt(mpc_realref(src),MPFR_RNDN),mpfr_get_flt(mpc_imagref(src),MPFR_RNDN))
#define cfromCLDouble_FP_SINGLE(dst,src) dst=cbuild_f((float)creal_l(src),(float)cimag_l(src))
#define cfromUi_FP_SINGLE(dst,src) dst=cbuild_f((float)src, 0)
#define cfromSi_FP_SINGLE(dst,src) dst=cbuild_f((float)src, 0)
#define cfromReIm_FP_SINGLE(dst,re,im) dst=cbuild_f(re,im)
#define cfromCDouble_FP_SINGLE(dst,src) dst=cbuild_f((float)creal_(src),(float)cimag_(src))
#define ctoCDouble_FP_SINGLE(src) cbuild_((double)creal_f(src), (double)cimag_f(src))

#define cmp_FP_SINGLE(x,y) ((x<y)?-1:((x>y)?1:0))
#define cmp_ui_FP_SINGLE(x,y) ((x<y)?-1:((x>y)?1:0))
#define cmp_si_FP_SINGLE(x,y) ((x<y)?-1:((x>y)?1:0))
#define eq_ui_FP_SINGLE(x,y) (x==y)
#define eq_si_FP_SINGLE(x,y) (x==y)
#define ceq_si_FP_SINGLE(x,y) ceq_f(x, cbuild_f(y, 0))
#define ceq_FP_SINGLE(x,y) ceq_f(x,y)
#define cisinteger_FP_SINGLE(x) cisinteger_f(x)

#define add_FP_SINGLE(res,x,y) res=((x)+(y))
#define sub_FP_SINGLE(res,x,y) res=((x)-(y))
#define mul_FP_SINGLE(res,x,y) res=((x)*(y))
#define div_FP_SINGLE(res,x,y) res=((x)/(y))

#define add_d_FP_SINGLE(res,x,y) res=((x)+(y))
#define sub_d_FP_SINGLE(res,x,y) res=((x)-(y))
#define mul_d_FP_SINGLE(res,x,y) res=((x)*(y))
#define div_d_FP_SINGLE(res,x,y) res=((x)/(y))

#define add_ui_FP_SINGLE(res,x,y) res=((x)+(y))
#define sub_ui_FP_SINGLE(res,x,y) res=((x)-(y))
#define mul_ui_FP_SINGLE(res,x,y) res=((x)*(y))
#define div_ui_FP_SINGLE(res,x,y) res=((x)/(y))

#define add_si_FP_SINGLE(res,x,y) res=((x)+(y))
#define sub_si_FP_SINGLE(res,x,y) res=((x)-(y))
#define mul_si_FP_SINGLE(res,x,y) res=((x)*(y))
#define div_si_FP_SINGLE(res,x,y) res=((x)/(y))

#define cadd_FP_SINGLE(res,x,y) res=cadd_f(x,y)
#define csub_FP_SINGLE(res,x,y) res=csub_f(x,y)
#define cmul_FP_SINGLE(res,x,y) res=cmul_f(x,y)
#define cdiv_FP_SINGLE(res,x,y) res=cdiv_f(x,y)

#define sqr_FP_SINGLE(res,x) res=((x)*(x))
#define csqr_FP_SINGLE(res,x) res=csqr_f(x)
#define fabs_FP_SINGLE(res,x) res=fabsf(x)
#define cabs_FP_SINGLE(res,z) res=cabs_f(z)
#define cnorm_FP_SINGLE(res,z) res=cnorm_f(z)
#define creal_FP_SINGLE(res,z) res=creal_f(z)
#define cimag_FP_SINGLE(res,z) res=cimag_f(z)
#define carg_FP_SINGLE(res,z) res=carg_f(z)
#define cpow_FP_SINGLE(res,z,y) res=cpow_f(z,y)
#define cipow_FP_SINGLE(res,z,y) res=cipow_f(z,y)
#define conj_FP_SINGLE(res,z) res=conj_f(z)
#define round_FP_SINGLE(res,x) res=roundf(x)
#define floor_FP_SINGLE(res,x) res=floorf(x)
#define pow_FP_SINGLE(res,x,y) res=powf(x,y)
#define fmax_FP_SINGLE(res,x,y) res=fmaxf(x,y)
#define modf_FP_SINGLE(ires,fres,x) fres=modff(x,&ires)
#define exp_FP_SINGLE(res,x) res=expf(x)
#define log_FP_SINGLE(res,x) res=logf(x)
#define sqrt_FP_SINGLE(res,x) res=sqrtf(x)
#define sin_FP_SINGLE(res,x) res=sinf(x)
#define cos_FP_SINGLE(res,x) res=cosf(x)
#define tan_FP_SINGLE(res,x) res=tanf(x)
/*********************************************************/

/************************FP_DOUBLE************************/
#define init_FP_DOUBLE(x) (void)NULL
#define clear_FP_DOUBLE(x) (void)NULL
#define cinit_FP_DOUBLE(x) (void)NULL
#define cclear_FP_DOUBLE(x) (void)NULL
#define assign_FP_DOUBLE(dst,src) dst=src
#define cassign_FP_DOUBLE(dst,src) dst=src

#define fromMPFR_FP_DOUBLE(dst,src) dst=mpfr_get_d(src,MPFR_RNDN)
#define fromLDouble_FP_DOUBLE(dst,src) dst=(FLOATTYPE_FP_DOUBLE)src
#define fromDouble_FP_DOUBLE(dst,src) dst=(FLOATTYPE_FP_DOUBLE)src
#define fromUi_FP_DOUBLE(dst,src) dst=(FLOATTYPE_FP_DOUBLE)src
#define fromSi_FP_DOUBLE(dst,src) dst=(FLOATTYPE_FP_DOUBLE)src
#define toDouble_FP_DOUBLE(src) (long double)src

#define cfromMPC_FP_DOUBLE(dst,src) dst=cbuild_(mpfr_get_d(mpc_realref(src),MPFR_RNDN),mpfr_get_d(mpc_imagref(src),MPFR_RNDN))
#define cfromCLDouble_FP_DOUBLE(dst,src) dst=cbuild_((double)creal_l(src),(double)cimag_l(src))
#define cfromUi_FP_DOUBLE(dst,src) dst=cbuild_((double)src, 0)
#define cfromSi_FP_DOUBLE(dst,src) dst=cbuild_((double)src, 0)
#define cfromReIm_FP_DOUBLE(dst,re,im) dst=cbuild_(re,im)
#define cfromCDouble_FP_DOUBLE(dst,src) dst=src
#define ctoCDouble_FP_DOUBLE(src) src

#define cmp_FP_DOUBLE(x,y) ((x<y)?-1:((x>y)?1:0))
#define cmp_ui_FP_DOUBLE(x,y) ((x<y)?-1:((x>y)?1:0))
#define cmp_si_FP_DOUBLE(x,y) ((x<y)?-1:((x>y)?1:0))
#define eq_ui_FP_DOUBLE(x,y) (x==y)
#define eq_si_FP_DOUBLE(x,y) (x==y)
#define ceq_si_FP_DOUBLE(x,y) ceq_(x, cbuild_(y, 0))
#define ceq_FP_DOUBLE(x,y) ceq_(x, y)
#define cisinteger_FP_DOUBLE(x) cisinteger_(x)

#define add_FP_DOUBLE(res,x,y) res=((x)+(y))
#define sub_FP_DOUBLE(res,x,y) res=((x)-(y))
#define mul_FP_DOUBLE(res,x,y) res=((x)*(y))
#define div_FP_DOUBLE(res,x,y) res=((x)/(y))

#define add_d_FP_DOUBLE(res,x,y) res=((x)+(y))
#define sub_d_FP_DOUBLE(res,x,y) res=((x)-(y))
#define mul_d_FP_DOUBLE(res,x,y) res=((x)*(y))
#define div_d_FP_DOUBLE(res,x,y) res=((x)/(y))

#define add_ui_FP_DOUBLE(res,x,y) res=((x)+(y))
#define sub_ui_FP_DOUBLE(res,x,y) res=((x)-(y))
#define mul_ui_FP_DOUBLE(res,x,y) res=((x)*(y))
#define div_ui_FP_DOUBLE(res,x,y) res=((x)/(y))

#define add_si_FP_DOUBLE(res,x,y) res=((x)+(y))
#define sub_si_FP_DOUBLE(res,x,y) res=((x)-(y))
#define mul_si_FP_DOUBLE(res,x,y) res=((x)*(y))
#define div_si_FP_DOUBLE(res,x,y) res=((x)/(y))

#define cadd_FP_DOUBLE(res,x,y) res=cadd_(x,y)
#define csub_FP_DOUBLE(res,x,y) res=csub_(x,y)
#define cmul_FP_DOUBLE(res,x,y) res=cmul_(x,y)
#define cdiv_FP_DOUBLE(res,x,y) res=cdiv_(x,y)

#define sqr_FP_DOUBLE(res,x) res=((x)*(x))
#define csqr_FP_DOUBLE(res,x) res=csqr_(x)
#define fabs_FP_DOUBLE(res,x) res=fabs(x)
#define cabs_FP_DOUBLE(res,z) res=cabs_(z)
#define cnorm_FP_DOUBLE(res,z) res=cnorm_(z)
#define creal_FP_DOUBLE(res,z) res=creal_(z)
#define cimag_FP_DOUBLE(res,z) res=cimag_(z)
#define carg_FP_DOUBLE(res,z) res=carg_(z)
#define cpow_FP_DOUBLE(res,z,y) res=cpow_(z,y)
#define cipow_FP_DOUBLE(res,z,y) res=cipow_(z,y)
#define conj_FP_DOUBLE(res,z) res=conj_(z)
#define round_FP_DOUBLE(res,x) res=round(x)
#define floor_FP_DOUBLE(res,x) res=floor(x)
#define pow_FP_DOUBLE(res,x,y) res=pow(x,y)
#define fmax_FP_DOUBLE(res,x,y) res=fmax(x,y)
#define modf_FP_DOUBLE(ires,fres,x) fres=modf(x,&ires)
#define exp_FP_DOUBLE(res,x) res=exp(x)
#define log_FP_DOUBLE(res,x) res=log(x)
#define sqrt_FP_DOUBLE(res,x) res=sqrt(x)
#define sin_FP_DOUBLE(res,x) res=sin(x)
#define cos_FP_DOUBLE(res,x) res=cos(x)
#define tan_FP_DOUBLE(res,x) res=tan(x)
/*********************************************************/

/************************FP_LDOUBLE***********************/
#define init_FP_LDOUBLE(x) (void)NULL
#define clear_FP_LDOUBLE(x) (void)NULL
#define cinit_FP_LDOUBLE(x) (void)NULL
#define cclear_FP_LDOUBLE(x) (void)NULL
#define assign_FP_LDOUBLE(dst,src) dst=src
#define cassign_FP_LDOUBLE(dst,src) dst=src

#define fromMPFR_FP_LDOUBLE(dst,src) dst=mpfr_get_ld(src,MPFR_RNDN)
#define fromLDouble_FP_LDOUBLE(dst,src) dst=(FLOATTYPE_FP_LDOUBLE)src
#define fromDouble_FP_LDOUBLE(dst,src) dst=(FLOATTYPE_FP_LDOUBLE)src
#define fromUi_FP_LDOUBLE(dst,src) dst=(FLOATTYPE_FP_LDOUBLE)src
#define fromSi_FP_LDOUBLE(dst,src) dst=(FLOATTYPE_FP_LDOUBLE)src
#define toDouble_FP_LDOUBLE(src) (long double)src

#define cfromMPC_FP_LDOUBLE(dst,src) dst=cbuild_l(mpfr_get_ld(mpc_realref(src),MPFR_RNDN),mpfr_get_ld(mpc_imagref(src),MPFR_RNDN))
#define cfromCLDouble_FP_LDOUBLE(dst,src) dst=src
#define cfromUi_FP_LDOUBLE(dst,src) dst=cbuild_l((long double)src, 0)
#define cfromSi_FP_LDOUBLE(dst,src) dst=cbuild_l((long double)src, 0)
#define cfromReIm_FP_LDOUBLE(dst,re,im) dst=cbuild_l(re, im)
#define cfromCDouble_FP_LDOUBLE(dst,src) dst=cbuild_l((double)creal_(src),(double)cimag_(src))
#define ctoCDouble_FP_LDOUBLE(src) cbuild_((double)creal_l(src),(double)cimag_l(src))

#define cmp_FP_LDOUBLE(x,y) ((x<y)?-1:((x>y)?1:0))
#define cmp_ui_FP_LDOUBLE(x,y) ((x<y)?-1:((x>y)?1:0))
#define cmp_si_FP_LDOUBLE(x,y) ((x<y)?-1:((x>y)?1:0))
#define ccmp_si_FP_LDOUBLE(x,y) ((x<y)?-1:((x>y)?1:0))
#define eq_ui_FP_LDOUBLE(x,y) (x==y)
#define eq_si_FP_LDOUBLE(x,y) (x==y)
#define ceq_si_FP_LDOUBLE(x,y) ceq_l(x, cbuild_l(y, 0))
#define ceq_FP_LDOUBLE(x,y) ceq_l(x,y)
#define cisinteger_FP_LDOUBLE(x) cisinteger_l(x)

#define add_FP_LDOUBLE(res,x,y) res=((x)+(y))
#define sub_FP_LDOUBLE(res,x,y) res=((x)-(y))
#define mul_FP_LDOUBLE(res,x,y) res=((x)*(y))
#define div_FP_LDOUBLE(res,x,y) res=((x)/(y))

#define add_d_FP_LDOUBLE(res,x,y) res=((x)+(y))
#define sub_d_FP_LDOUBLE(res,x,y) res=((x)-(y))
#define mul_d_FP_LDOUBLE(res,x,y) res=((x)*(y))
#define div_d_FP_LDOUBLE(res,x,y) res=((x)/(y))

#define add_ui_FP_LDOUBLE(res,x,y) res=((x)+(y))
#define sub_ui_FP_LDOUBLE(res,x,y) res=((x)-(y))
#define mul_ui_FP_LDOUBLE(res,x,y) res=((x)*(y))
#define div_ui_FP_LDOUBLE(res,x,y) res=((x)/(y))

#define add_si_FP_LDOUBLE(res,x,y) res=((x)+(y))
#define sub_si_FP_LDOUBLE(res,x,y) res=((x)-(y))
#define mul_si_FP_LDOUBLE(res,x,y) res=((x)*(y))
#define div_si_FP_LDOUBLE(res,x,y) res=((x)/(y))

#define cadd_FP_LDOUBLE(res,x,y) res=cadd_l(x,y)
#define csub_FP_LDOUBLE(res,x,y) res=csub_l(x,y)
#define cmul_FP_LDOUBLE(res,x,y) res=cmul_l(x,y)
#define cdiv_FP_LDOUBLE(res,x,y) res=cdiv_l(x,y)

#define sqr_FP_LDOUBLE(res,x) res=((x)*(x))
#define csqr_FP_LDOUBLE(res,x) res=csqr_l(x)
#define fabs_FP_LDOUBLE(res,x) res=fabsl(x)
#define cabs_FP_LDOUBLE(res,z) res=cabs_l(z)
#define cnorm_FP_LDOUBLE(res,z) res=cnorm_l(z)
#define creal_FP_LDOUBLE(res,z) res=creal_l(z)
#define cimag_FP_LDOUBLE(res,z) res=cimag_l(z)
#define carg_FP_LDOUBLE(res,z) res=carg_l(z)
#define cpow_FP_LDOUBLE(res,z,y) res=cpow_l(z,y)
#define cipow_FP_LDOUBLE(res,z,y) res=cipow_l(z,y)
#define conj_FP_LDOUBLE(res,z) res=conj_l(z)
#define round_FP_LDOUBLE(res,x) res=roundl(x)
#define floor_FP_LDOUBLE(res,x) res=floorl(x)
#define pow_FP_LDOUBLE(res,x,y) res=powl(x,y)
#define fmax_FP_LDOUBLE(res,x,y) res=fmaxl(x,y)
#define modf_FP_LDOUBLE(ires,fres,x) fres=modfl(x,&ires)
#define exp_FP_LDOUBLE(res,x) res=expl(x)
#define log_FP_LDOUBLE(res,x) res=logl(x)
#define sqrt_FP_LDOUBLE(res,x) res=sqrtl(x)
#define sin_FP_LDOUBLE(res,x) res=sinl(x)
#define cos_FP_LDOUBLE(res,x) res=cosl(x)
#define tan_FP_LDOUBLE(res,x) res=tanl(x)
/*********************************************************/

/**************************FP_MP**************************/
#define init_FP_MP(x) mpfr_init(x)
#define clear_FP_MP(x) mpfr_clear(x)
#define cinit_FP_MP(x) mpc_init2(x,mpfr_get_default_prec())
#define cclear_FP_MP(x) mpc_clear(x)
#define assign_FP_MP(dst,src) mpfr_set(dst,src,MPFR_RNDN)
#define cassign_FP_MP(dst,src) mpc_set(dst,src,MPC_RNDNN)

#define fromMPFR_FP_MP(dst,src) mpfr_set(dst,src,MPFR_RNDN)
#define fromLDouble_FP_MP(dst,src) mpfr_set_ld(dst,src,MPFR_RNDN)
#define fromDouble_FP_MP(dst,src) mpfr_set_d(dst,src,MPFR_RNDN)
#define fromUi_FP_MP(dst,src) mpfr_set_ui(dst,src,MPFR_RNDN)
#define fromSi_FP_MP(dst,src) mpfr_set_si(dst,src,MPFR_RNDN)
#define toDouble_FP_MP(src) mpfr_get_ld(src,MPFR_RNDN)

#define cfromMPC_FP_MP(dst,src) mpc_set(dst,src,MPC_RNDNN)
#define cfromCLDouble_FP_MP(dst,src) mpc_set_ld_ld(dst,creal_l(src),cimag_l(src),MPC_RNDNN)
#define cfromUi_FP_MP(dst,src) mpc_set_ui(dst,src,MPC_RNDNN)
#define cfromSi_FP_MP(dst,src) mpc_set_si(dst,src,MPC_RNDNN)
#define cfromReIm_FP_MP(dst,re,im) mpc_set_fr_fr(dst,re,im,MPC_RNDNN)
#define cfromCDouble_FP_MP(dst,src) mpc_set_d_d(dst,creal_(src),cimag_(src),MPC_RNDNN)
#define ctoCDouble_FP_MP(src) cbuild_(mpfr_get_d(mpc_realref(src),MPFR_RNDN),mpfr_get_d(mpc_imagref(src),MPFR_RNDN))

#define cmp_FP_MP(x,y) mpfr_cmp(x,y)
#define cmp_ui_FP_MP(x,y) mpfr_cmp_ui(x,y)
#define cmp_si_FP_MP(x,y) mpfr_cmp_si(x,y)
#define eq_ui_FP_MP(x,y) (mpfr_cmp_ui(x,y)==0)
#define eq_si_FP_MP(x,y) (mpfr_cmp_si(x,y)==0)
#define ceq_si_FP_MP(x,y) (mpc_cmp_si(x,y)==0)
#define ceq_FP_MP(x,y) (mpc_cmp(x,y)==0)
#define cisinteger_FP_MP(x) mpcIsInteger(x)

#define add_FP_MP(res,x,y) mpfr_add(res,x,y,MPFR_RNDN)
#define sub_FP_MP(res,x,y) mpfr_sub(res,x,y,MPFR_RNDN)
#define mul_FP_MP(res,x,y) mpfr_mul(res,x,y,MPFR_RNDN)
#define div_FP_MP(res,x,y) mpfr_div(res,x,y,MPFR_RNDN)

#define add_d_FP_MP(res,x,y) mpfr_add_d(res,x,y,MPFR_RNDN)
#define sub_d_FP_MP(res,x,y) mpfr_sub_d(res,x,y,MPFR_RNDN)
#define mul_d_FP_MP(res,x,y) mpfr_mul_d(res,x,y,MPFR_RNDN)
#define div_d_FP_MP(res,x,y) mpfr_div_d(res,x,y,MPFR_RNDN)

#define add_ui_FP_MP(res,x,y) mpfr_add_ui(res,x,y,MPFR_RNDN)
#define sub_ui_FP_MP(res,x,y) mpfr_sub_ui(res,x,y,MPFR_RNDN)
#define mul_ui_FP_MP(res,x,y) mpfr_mul_ui(res,x,y,MPFR_RNDN)
#define div_ui_FP_MP(res,x,y) mpfr_div_ui(res,x,y,MPFR_RNDN)

#define add_si_FP_MP(res,x,y) mpfr_add_si(res,x,y,MPFR_RNDN)
#define sub_si_FP_MP(res,x,y) mpfr_sub_si(res,x,y,MPFR_RNDN)
#define mul_si_FP_MP(res,x,y) mpfr_mul_si(res,x,y,MPFR_RNDN)
#define div_si_FP_MP(res,x,y) mpfr_div_si(res,x,y,MPFR_RNDN)

#define cadd_FP_MP(res,x,y) mpc_add(res,x,y,MPC_RNDNN)
#define csub_FP_MP(res,x,y) mpc_sub(res,x,y,MPC_RNDNN)
#define cmul_FP_MP(res,x,y) mpc_mul(res,x,y,MPC_RNDNN)
#define cdiv_FP_MP(res,x,y) mpc_div(res,x,y,MPC_RNDNN)

#define sqr_FP_MP(res,x) mpfr_sqr(res,x,MPFR_RNDN)
#define csqr_FP_MP(res,x) mpc_sqr(res,x,MPC_RNDNN)
#define fabs_FP_MP(res,x) mpfr_abs(res,x,MPFR_RNDN)
#define cabs_FP_MP(res,z) mpc_abs(res,z,MPFR_RNDN)
#define cnorm_FP_MP(res,z) mpc_norm(res,z,MPFR_RNDN)
#define creal_FP_MP(res,z) mpc_real(res,z,MPFR_RNDN)
#define cimag_FP_MP(res,z) mpc_imag(res,z,MPFR_RNDN)
#define carg_FP_MP(res,z) mpc_arg(res,z,MPFR_RNDN)
#define cpow_FP_MP(res,z,y) mpc_pow(res,z,y,MPC_RNDNN)
#define cipow_FP_MP(res,z,y) mpc_pow_si(res,z,y,MPC_RNDNN)
#define conj_FP_MP(res,z) mpc_conj(res,z,MPC_RNDNN)
#define round_FP_MP(res,x) mpfr_round(res,x)
#define floor_FP_MP(res,x) mpfr_floor(res,x)
#define pow_FP_MP(res,x,y) mpfr_pow(res,x,y,MPFR_RNDN)
#define fmax_FP_MP(res,x,y) mpfr_max(res,x,y,MPFR_RNDN)
#define modf_FP_MP(ires,fres,x) mpfr_modf(ires,fres,x,MPFR_RNDN)
#define exp_FP_MP(res,x) mpfr_exp(res,x,MPFR_RNDN)
#define log_FP_MP(res,x) mpfr_log(res,x,MPFR_RNDN)
#define sqrt_FP_MP(res,x) mpfr_sqrt(res,x,MPFR_RNDN)
#define sin_FP_MP(res,x) mpfr_sin(res,x,MPFR_RNDN)
#define cos_FP_MP(res,x) mpfr_cos(res,x,MPFR_RNDN)
#define tan_FP_MP(res,x) mpfr_tan(res,x,MPFR_RNDN)
/*********************************************************/


/**************************COMMON*************************/
#define initF(fprec,x) init_##fprec(x)
#define clearF(fprec,x) clear_##fprec(x)
#define cinitF(fprec,x) cinit_##fprec(x)
#define cclearF(fprec,x) cclear_##fprec(x)
#define assignF(fprec,dst,src) assign_##fprec(dst,src)
#define cassignF(fprec,dst,src) cassign_##fprec(dst,src)

#define fromMPFRF(fprec,dst,src) fromMPFR_##fprec(dst,src)
#define fromLDoubleF(fprec,dst,src) fromLDouble_##fprec(dst,src)
#define fromDoubleF(fprec,dst,src) fromDouble_##fprec(dst,src)
#define fromUiF(fprec,dst,src) fromUi_##fprec(dst,src)
#define fromSiF(fprec,dst,src) fromSi_##fprec(dst,src)
#define toDoubleF(fprec,src) toDouble_##fprec(src)

#define cfromMPCF(fprec,dst,src) cfromMPC_##fprec(dst,src)
#define cfromCLDoubleF(fprec,dst,src) cfromCLDouble_##fprec(dst,src)
#define cfromUiF(fprec,dst,src) cfromUi_##fprec(dst,src)
#define cfromSiF(fprec,dst,src) cfromSi_##fprec(dst,src)
#define cfromReImF(fprec,dst,re,im) cfromReIm_##fprec(dst,re,im)
#define cfromCDoubleF(fprec,dst,src) cfromCDouble_##fprec(dst,src)
#define ctoCDoubleF(fprec,src) ctoCDouble_##fprec(src)

#define cmpF(fprec,x,y) cmp_##fprec(x,y)
#define cmp_uiF(fprec,x,y) cmp_ui_##fprec(x,y)
#define cmp_siF(fprec,x,y) cmp_si_##fprec(x,y)
#define eq_uiF(fprec,x,y) eq_ui_##fprec(x,y)
#define eq_siF(fprec,x,y) eq_si_##fprec(x,y)
#define ceq_siF(fprec,x,y) ceq_si_##fprec(x,y)
#define ceqF(fprec,x,y) ceq_##fprec(x,y)
#define cisintegerF(fprec,x) cisinteger_##fprec(x)

#define addF(fprec,res,x,y) add_##fprec(res,x,y)
#define subF(fprec,res,x,y) sub_##fprec(res,x,y)
#define mulF(fprec,res,x,y) mul_##fprec(res,x,y)
#define divF(fprec,res,x,y) div_##fprec(res,x,y)

#define add_dF(fprec,res,x,y) add_d_##fprec(res,x,y)
#define sub_dF(fprec,res,x,y) sub_d_##fprec(res,x,y)
#define mul_dF(fprec,res,x,y) mul_d_##fprec(res,x,y)
#define div_dF(fprec,res,x,y) div_d_##fprec(res,x,y)

#define add_uiF(fprec,res,x,y) add_ui_##fprec(res,x,y)
#define sub_uiF(fprec,res,x,y) sub_ui_##fprec(res,x,y)
#define mul_uiF(fprec,res,x,y) mul_ui_##fprec(res,x,y)
#define div_uiF(fprec,res,x,y) div_ui_##fprec(res,x,y)

#define add_siF(fprec,res,x,y) add_si_##fprec(res,x,y)
#define sub_siF(fprec,res,x,y) sub_si_##fprec(res,x,y)
#define mul_siF(fprec,res,x,y) mul_si_##fprec(res,x,y)
#define div_siF(fprec,res,x,y) div_si_##fprec(res,x,y)

#define caddF(fprec,res,x,y) cadd_##fprec(res,x,y)
#define csubF(fprec,res,x,y) csub_##fprec(res,x,y)
#define cmulF(fprec,res,x,y) cmul_##fprec(res,x,y)
#define cdivF(fprec,res,x,y) cdiv_##fprec(res,x,y)

#define sqrF(fprec,res,x) sqr_##fprec(res,x)
#define csqrF(fprec,res,x) csqr_##fprec(res,x)
#define fabsF(fprec,res,x) fabs_##fprec(res,x)
#define cabsF(fprec,res,z) cabs_##fprec(res,z)
#define cnormF(fprec,res,z) cnorm_##fprec(res,z)
#define crealF(fprec,res,z) creal_##fprec(res,z)
#define cimagF(fprec,res,z) cimag_##fprec(res,z)
#define cargF(fprec,res,z) carg_##fprec(res,z)
#define cpowF(fprec,res,z,y) cpow_##fprec(res,z,y)
#define cipowF(fprec,res,z,y) cipow_##fprec(res,z,y)
#define conjF(fprec,res,z) conj_##fprec(res,z)
#define roundF(fprec,res,x) round_##fprec(res,x)
#define floorF(fprec,res,x) floor_##fprec(res,x)
#define powF(fprec,res,x,y) pow_##fprec(res,x,y)
#define fmaxF(fprec,res,x,y) fmax_##fprec(res,x,y)
#define modfF(fprec,ires,fres,x) modf_##fprec(ires,fres,x)
#define expF(fprec,res,x) exp_##fprec(res,x)
#define logF(fprec,res,x) log_##fprec(res,x)
#define sqrtF(fprec,res,x) sqrt_##fprec(res,x)
#define sinF(fprec,res,x) sin_##fprec(res,x)
#define cosF(fprec,res,x) cos_##fprec(res,x)
#define tanF(fprec,res,x) tan_##fprec(res,x)
/*********************************************************/

/***********************BIGGEST_FLOAT*********************/
#if defined(_ENABLE_MP_FLOATS)

#define BiggestFloat FLOATTYPE(FP_MP)
#define BiggestComplexFloat COMPLEX_FLOATTYPE(FP_MP)

#define initBiggestF(x) initF(FP_MP,x)
#define clearBiggestF(x) clearF(FP_MP,x)
#define cinitBiggestF(x) cinitF(FP_MP,x)
#define cclearBiggestF(x) cclearF(FP_MP,x)
#define assignBiggestF(dst,src) assignF(FP_MP,dst,src)
#define cassignBiggestF(dst,src) cassignF(FP_MP,dst,src)

#define fromMPFRBiggestF(dst,src) fromMPFRF(FP_MP,dst,src)
#define fromLDoubleBiggestF(dst,src) fromLDoubleF(FP_MP,dst,src)
#define fromDoubleBiggestF(dst,src) fromDoubleF(FP_MP,dst,src)
#define fromUiBiggestF(dst,src) fromUiF(FP_MP,dst,src)
#define fromSiBiggestF(dst,src) fromSiF(FP_MP,dst,src)
#define toDoubleBiggestF(src) toDoubleF(FP_MP,src)

#define cfromMPCBiggestF(dst,src) cfromMPCF(FP_MP,dst,src)
#define cfromCLDoubleBiggestF(dst,src) cfromCLDoubleF(FP_MP,dst,src)
#define cfromUiBiggestF(dst,src) cfromUiF(FP_MP,dst,src)
#define cfromSiBiggestF(dst,src) cfromSiF(FP_MP,dst,src)
#define cfromReImBiggestF(dst,re,im) cfromReImF(FP_MP,dst,re,im)
#define cfromCDoubleBiggestF(dst,src) cfromCDoubleF(FP_MP,dst,src)
#define ctoCDoubleBiggestF(src) ctoCDoubleF(FP_MP,src)

#define cmpBiggestF(x,y) cmpF(FP_MP,x,y)
#define cmp_uiBiggestF(x,y) cmp_uiF(FP_MP,x,y)
#define cmp_siBiggestF(x,y) cmp_siF(FP_MP,x,y)
#define eq_uiBiggestF(x,y) eq_uiF(FP_MP,x,y)
#define eq_siBiggestF(x,y) eq_siF(FP_MP,x,y)
#define ceq_siBiggestF(x,y) ceq_siF(FP_MP,x,y)
#define ceqBiggestF(x,y) ceqF(FP_MP,x,y)
#define cisintegerBiggestF(x) cisintegerF(FP_MP,x)

#define addBiggestF(res,x,y) addF(FP_MP,res,x,y)
#define subBiggestF(res,x,y) subF(FP_MP,res,x,y)
#define mulBiggestF(res,x,y) mulF(FP_MP,res,x,y)
#define divBiggestF(res,x,y) divF(FP_MP,res,x,y)

#define add_dBiggestF(res,x,y) add_dF(FP_MP,res,x,y)
#define sub_dBiggestF(res,x,y) sub_dF(FP_MP,res,x,y)
#define mul_dBiggestF(res,x,y) mul_dF(FP_MP,res,x,y)
#define div_dBiggestF(res,x,y) div_dF(FP_MP,res,x,y)

#define add_uiBiggestF(res,x,y) add_uiF(FP_MP,res,x,y)
#define sub_uiBiggestF(res,x,y) sub_uiF(FP_MP,res,x,y)
#define mul_uiBiggestF(res,x,y) mul_uiF(FP_MP,res,x,y)
#define div_uiBiggestF(res,x,y) div_uiF(FP_MP,res,x,y)

#define add_siBiggestF(res,x,y) add_siF(FP_MP,res,x,y)
#define sub_siBiggestF(res,x,y) sub_siF(FP_MP,res,x,y)
#define mul_siBiggestF(res,x,y) mul_siF(FP_MP,res,x,y)
#define div_siBiggestF(res,x,y) div_siF(FP_MP,res,x,y)

#define caddBiggestF(res,x,y) caddF(FP_MP,res,x,y)
#define csubBiggestF(res,x,y) csubF(FP_MP,res,x,y)
#define cmulBiggestF(res,x,y) cmulF(FP_MP,res,x,y)
#define cdivBiggestF(res,x,y) cdivF(FP_MP,res,x,y)

#define sqrBiggestF(res,x) sqrF(FP_MP,res,x)
#define csqrBiggestF(res,x) csqrF(FP_MP,res,x)
#define fabsBiggestF(res,x) fabsF(FP_MP,res,x)
#define cabsBiggestF(res,z) cabsF(FP_MP,res,z)
#define cnormBiggestF(res,z) cnormF(FP_MP,res,z)
#define crealBiggestF(res,z) crealF(FP_MP,res,z)
#define cimagBiggestF(res,z) cimagF(FP_MP,res,z)
#define cargBiggestF(res,z) cargF(FP_MP,res,z)
#define cpowBiggestF(res,z,y) cpowF(FP_MP,res,z,y)
#define cipowBiggestF(res,z,y) cipowF(FP_MP,res,z,y)
#define conjBiggestF(res,z) conjF(FP_MP,res,z)
#define roundBiggestF(res,x) roundF(FP_MP,res,x)
#define floorBiggestF(res,x) floorF(FP_MP,res,x)
#define powBiggestF(res,x,y) powF(FP_MP,res,x,y)
#define fmaxBiggestF(res,x,y) fmaxF(FP_MP,res,x,y)
#define modfBiggestF(ires,fres,x) modfF(FP_MP,ires,fres,x)
#define expBiggestF(res,x) expF(FP_MP,res,x)
#define logBiggestF(res,x) logF(FP_MP,res,x)
#define sqrtBiggestF(res,x) sqrtF(FP_MP,res,x)
#define sinBiggestF(res,x) sinF(FP_MP,res,x)
#define cosBiggestF(res,x) cosF(FP_MP,res,x)
#define tanBiggestF(res,x) tanF(FP_MP,res,x)

#define fromBiggestF(fprec,res,x) fromMPFRF(fprec,res,x)
#define cfromBiggestF(fprec,res,x) cfromMPCF(fprec,res,x)

#elif defined(_ENABLE_LDOUBLE_FLOATS) // !defined(_ENABLE_MP_FLOATS) && defined(_ENABLE_LDOUBLE_FLOATS)

#define BiggestFloat FLOATTYPE(FP_LDOUBLE)
#define BiggestComplexFloat COMPLEX_FLOATTYPE(FP_LDOUBLE)

#define initBiggestF(x) initF(FP_LDOUBLE,x)
#define clearBiggestF(x) clearF(FP_LDOUBLE,x)
#define cinitBiggestF(x) cinitF(FP_LDOUBLE,x)
#define cclearBiggestF(x) cclearF(FP_LDOUBLE,x)
#define assignBiggestF(dst,src) assignF(FP_LDOUBLE,dst,src)
#define cassignBiggestF(dst,src) cassignF(FP_LDOUBLE,dst,src)

#define fromMPFRBiggestF(dst,src) fromMPFRF(FP_LDOUBLE,dst,src)
#define fromLDoubleBiggestF(dst,src) fromLDoubleF(FP_LDOUBLE,dst,src)
#define fromDoubleBiggestF(dst,src) fromDoubleF(FP_LDOUBLE,dst,src)
#define fromUiBiggestF(dst,src) fromUiF(FP_LDOUBLE,dst,src)
#define fromSiBiggestF(dst,src) fromSiF(FP_LDOUBLE,dst,src)
#define toDoubleBiggestF(src) toDoubleF(FP_LDOUBLE,src)

#define cfromMPCBiggestF(dst,src) cfromMPCF(FP_LDOUBLE,dst,src)
#define cfromCLDoubleBiggestF(dst,src) cfromCLDoubleF(FP_LDOUBLE,dst,src)
#define cfromUiBiggestF(dst,src) cfromUiF(FP_LDOUBLE,dst,src)
#define cfromSiBiggestF(dst,src) cfromSiF(FP_LDOUBLE,dst,src)
#define cfromReImBiggestF(dst,re,im) cfromReImF(FP_LDOUBLE,dst,re,im)
#define cfromCDoubleBiggestF(dst,src) cfromCDoubleF(FP_LDOUBLE,dst,src)
#define ctoCDoubleBiggestF(src) ctoCDoubleF(FP_LDOUBLE,src)

#define cmpBiggestF(x,y) cmpF(FP_LDOUBLE,x,y)
#define cmp_uiBiggestF(x,y) cmp_uiF(FP_LDOUBLE,x,y)
#define cmp_siBiggestF(x,y) cmp_siF(FP_LDOUBLE,x,y)
#define eq_uiBiggestF(x,y) eq_uiF(FP_LDOUBLE,x,y)
#define eq_siBiggestF(x,y) eq_siF(FP_LDOUBLE,x,y)
#define ceq_siBiggestF(x,y) ceq_siF(FP_LDOUBLE,x,y)
#define ceqBiggestF(x,y) ceqF(FP_LDOUBLE,x,y)
#define cisintegerBiggestF(x) cisintegerF(FP_LDOUBLE,x)

#define addBiggestF(res,x,y) addF(FP_LDOUBLE,res,x,y)
#define subBiggestF(res,x,y) subF(FP_LDOUBLE,res,x,y)
#define mulBiggestF(res,x,y) mulF(FP_LDOUBLE,res,x,y)
#define divBiggestF(res,x,y) divF(FP_LDOUBLE,res,x,y)

#define add_dBiggestF(res,x,y) add_dF(FP_LDOUBLE,res,x,y)
#define sub_dBiggestF(res,x,y) sub_dF(FP_LDOUBLE,res,x,y)
#define mul_dBiggestF(res,x,y) mul_dF(FP_LDOUBLE,res,x,y)
#define div_dBiggestF(res,x,y) div_dF(FP_LDOUBLE,res,x,y)

#define add_uiBiggestF(res,x,y) add_uiF(FP_LDOUBLE,res,x,y)
#define sub_uiBiggestF(res,x,y) sub_uiF(FP_LDOUBLE,res,x,y)
#define mul_uiBiggestF(res,x,y) mul_uiF(FP_LDOUBLE,res,x,y)
#define div_uiBiggestF(res,x,y) div_uiF(FP_LDOUBLE,res,x,y)

#define add_siBiggestF(res,x,y) add_siF(FP_LDOUBLE,res,x,y)
#define sub_siBiggestF(res,x,y) sub_siF(FP_LDOUBLE,res,x,y)
#define mul_siBiggestF(res,x,y) mul_siF(FP_LDOUBLE,res,x,y)
#define div_siBiggestF(res,x,y) div_siF(FP_LDOUBLE,res,x,y)

#define caddBiggestF(res,x,y) caddF(FP_LDOUBLE,res,x,y)
#define csubBiggestF(res,x,y) csubF(FP_LDOUBLE,res,x,y)
#define cmulBiggestF(res,x,y) cmulF(FP_LDOUBLE,res,x,y)
#define cdivBiggestF(res,x,y) cdivF(FP_LDOUBLE,res,x,y)

#define sqrBiggestF(res,x) sqrF(FP_LDOUBLE,res,x)
#define csqrBiggestF(res,x) csqrF(FP_LDOUBLE,res,x)
#define fabsBiggestF(res,x) fabsF(FP_LDOUBLE,res,x)
#define cabsBiggestF(res,z) cabsF(FP_LDOUBLE,res,z)
#define cnormBiggestF(res,z) cnormF(FP_LDOUBLE,res,z)
#define crealBiggestF(res,z) crealF(FP_LDOUBLE,res,z)
#define cimagBiggestF(res,z) cimagF(FP_LDOUBLE,res,z)
#define cargBiggestF(res,z) cargF(FP_LDOUBLE,res,z)
#define cpowBiggestF(res,z,y) cpowF(FP_LDOUBLE,res,z,y)
#define cipowBiggestF(res,z,y) cipowF(FP_LDOUBLE,res,z,y)
#define conjBiggestF(res,z) conjF(FP_LDOUBLE,res,z)
#define roundBiggestF(res,x) roundF(FP_LDOUBLE,res,x)
#define floorBiggestF(res,x) floorF(FP_LDOUBLE,res,x)
#define powBiggestF(res,x,y) powF(FP_LDOUBLE,res,x,y)
#define fmaxBiggestF(res,x,y) fmaxF(FP_LDOUBLE,res,x,y)
#define modfBiggestF(ires,fres,x) modfF(FP_LDOUBLE,ires,fres,x)
#define expBiggestF(res,x) expF(FP_LDOUBLE,res,x)
#define logBiggestF(res,x) logF(FP_LDOUBLE,res,x)
#define sqrtBiggestF(res,x) sqrtF(FP_LDOUBLE,res,x)
#define sinBiggestF(res,x) sinF(FP_LDOUBLE,res,x)
#define cosBiggestF(res,x) cosF(FP_LDOUBLE,res,x)
#define tanBiggestF(res,x) tanF(FP_LDOUBLE,res,x)

#define fromBiggestF(fprec,res,x) fromLDoubleF(fprec,res,x)
#define cfromBiggestF(fprec,res,x) cfromCLDoubleF(fprec,res,x)

#else // !defined(_ENABLE_MP_FLOATS) && !defined(_ENABLE_LDOUBLE_FLOATS)

#define BiggestFloat FLOATTYPE(FP_DOUBLE)
#define BiggestComplexFloat COMPLEX_FLOATTYPE(FP_DOUBLE)

#define initBiggestF(x) initF(FP_DOUBLE,x)
#define clearBiggestF(x) clearF(FP_DOUBLE,x)
#define cinitBiggestF(x) cinitF(FP_DOUBLE,x)
#define cclearBiggestF(x) cclearF(FP_DOUBLE,x)
#define assignBiggestF(dst,src) assignF(FP_DOUBLE,dst,src)
#define cassignBiggestF(dst,src) cassignF(FP_DOUBLE,dst,src)

#define fromMPFRBiggestF(dst,src) fromMPFRF(FP_DOUBLE,dst,src)
#define fromLDoubleBiggestF(dst,src) fromLDoubleF(FP_DOUBLE,dst,src)
#define fromDoubleBiggestF(dst,src) fromDoubleF(FP_DOUBLE,dst,src)
#define fromUiBiggestF(dst,src) fromUiF(FP_DOUBLE,dst,src)
#define fromSiBiggestF(dst,src) fromSiF(FP_DOUBLE,dst,src)
#define toDoubleBiggestF(src) toDoubleF(FP_DOUBLE,src)

#define cfromMPCBiggestF(dst,src) cfromMPCF(FP_DOUBLE,dst,src)
#define cfromCLDoubleBiggestF(dst,src) cfromCLDoubleF(FP_DOUBLE,dst,src)
#define cfromUiBiggestF(dst,src) cfromUiF(FP_DOUBLE,dst,src)
#define cfromSiBiggestF(dst,src) cfromSiF(FP_DOUBLE,dst,src)
#define cfromReImBiggestF(dst,re,im) cfromReImF(FP_DOUBLE,dst,re,im)
#define cfromCDoubleBiggestF(dst,src) cfromCDoubleF(FP_DOUBLE,dst,src)
#define ctoCDoubleBiggestF(src) ctoCDoubleF(FP_DOUBLE,src)

#define cmpBiggestF(x,y) cmpF(FP_DOUBLE,x,y)
#define cmp_uiBiggestF(x,y) cmp_uiF(FP_DOUBLE,x,y)
#define cmp_siBiggestF(x,y) cmp_siF(FP_DOUBLE,x,y)
#define eq_uiBiggestF(x,y) eq_uiF(FP_DOUBLE,x,y)
#define eq_siBiggestF(x,y) eq_siF(FP_DOUBLE,x,y)
#define ceq_siBiggestF(x,y) ceq_siF(FP_DOUBLE,x,y)
#define ceqBiggestF(x,y) ceqF(FP_DOUBLE,x,y)
#define cisintegerBiggestF(x) cisintegerF(FP_DOUBLE,x)

#define addBiggestF(res,x,y) addF(FP_DOUBLE,res,x,y)
#define subBiggestF(res,x,y) subF(FP_DOUBLE,res,x,y)
#define mulBiggestF(res,x,y) mulF(FP_DOUBLE,res,x,y)
#define divBiggestF(res,x,y) divF(FP_DOUBLE,res,x,y)

#define add_dBiggestF(res,x,y) add_dF(FP_DOUBLE,res,x,y)
#define sub_dBiggestF(res,x,y) sub_dF(FP_DOUBLE,res,x,y)
#define mul_dBiggestF(res,x,y) mul_dF(FP_DOUBLE,res,x,y)
#define div_dBiggestF(res,x,y) div_dF(FP_DOUBLE,res,x,y)

#define add_uiBiggestF(res,x,y) add_uiF(FP_DOUBLE,res,x,y)
#define sub_uiBiggestF(res,x,y) sub_uiF(FP_DOUBLE,res,x,y)
#define mul_uiBiggestF(res,x,y) mul_uiF(FP_DOUBLE,res,x,y)
#define div_uiBiggestF(res,x,y) div_uiF(FP_DOUBLE,res,x,y)

#define add_siBiggestF(res,x,y) add_siF(FP_DOUBLE,res,x,y)
#define sub_siBiggestF(res,x,y) sub_siF(FP_DOUBLE,res,x,y)
#define mul_siBiggestF(res,x,y) mul_siF(FP_DOUBLE,res,x,y)
#define div_siBiggestF(res,x,y) div_siF(FP_DOUBLE,res,x,y)

#define caddBiggestF(res,x,y) caddF(FP_DOUBLE,res,x,y)
#define csubBiggestF(res,x,y) csubF(FP_DOUBLE,res,x,y)
#define cmulBiggestF(res,x,y) cmulF(FP_DOUBLE,res,x,y)
#define cdivBiggestF(res,x,y) cdivF(FP_DOUBLE,res,x,y)

#define sqrBiggestF(res,x) sqrF(FP_DOUBLE,res,x)
#define csqrBiggestF(res,x) csqrF(FP_DOUBLE,res,x)
#define fabsBiggestF(res,x) fabsF(FP_DOUBLE,res,x)
#define cabsBiggestF(res,z) cabsF(FP_DOUBLE,res,z)
#define cnormBiggestF(res,z) cnormF(FP_DOUBLE,res,z)
#define crealBiggestF(res,z) crealF(FP_DOUBLE,res,z)
#define cimagBiggestF(res,z) cimagF(FP_DOUBLE,res,z)
#define cargBiggestF(res,z) cargF(FP_DOUBLE,res,z)
#define cpowBiggestF(res,z,y) cpowF(FP_DOUBLE,res,z,y)
#define cipowBiggestF(res,z,y) cipowF(FP_DOUBLE,res,z,y)
#define conjBiggestF(res,z) conjF(FP_DOUBLE,res,z)
#define roundBiggestF(res,x) roundF(FP_DOUBLE,res,x)
#define floorBiggestF(res,x) floorF(FP_DOUBLE,res,x)
#define powBiggestF(res,x,y) powF(FP_DOUBLE,res,x,y)
#define fmaxBiggestF(res,x,y) fmaxF(FP_DOUBLE,res,x,y)
#define modfBiggestF(ires,fres,x) modfF(FP_DOUBLE,ires,fres,x)
#define expBiggestF(res,x) expF(FP_DOUBLE,res,x)
#define logBiggestF(res,x) logF(FP_DOUBLE,res,x)
#define sqrtBiggestF(res,x) sqrtF(FP_DOUBLE,res,x)
#define sinBiggestF(res,x) sinF(FP_DOUBLE,res,x)
#define cosBiggestF(res,x) cosF(FP_DOUBLE,res,x)
#define tanBiggestF(res,x) tanF(FP_DOUBLE,res,x)

#define fromBiggestF(fprec,res,x) fromDoubleF(fprec,res,x)
#define cfromBiggestF(fprec,res,x) cfromCDoubleF(fprec,res,x)
#endif
/*********************************************************/

#define DECL_FLOAT(fprec,x) FLOATTYPE(fprec) x
#define DECL_COMPLEX_FLOAT(fprec,x) COMPLEX_FLOATTYPE(fprec) x

#define DECL_INIT_FLOAT(fprec,x) \
	DECL_FLOAT(fprec,x);\
	initF(fprec,x)
#define DECL_INIT_COMPLEX_FLOAT(fprec,x) \
	DECL_COMPLEX_FLOAT(fprec,x);\
	cinitF(fprec,x)

#define FLOAT_VAR(fprec,x) x##_##fprec

#define AUX_DECL_MULTI_FLOAT(fprec,x) DECL_FLOAT(fprec,FLOAT_VAR(fprec,x))
#define AUX_DECL_MULTI_COMPLEX_FLOAT(fprec,x) DECL_COMPLEX_FLOAT(fprec,FLOAT_VAR(fprec,x))
#define AUX_INIT_MULTI_FLOAT(fprec,x) initF(fprec,FLOAT_VAR(fprec,x))
#define AUX_INIT_MULTI_COMPLEX_FLOAT(fprec,x) cinitF(fprec,FLOAT_VAR(fprec,x))
#define AUX_CLEAR_MULTI_FLOAT(fprec,x) clearF(fprec,FLOAT_VAR(fprec,x))
#define AUX_CLEAR_MULTI_COMPLEX_FLOAT(fprec,x) cclearF(fprec,FLOAT_VAR(fprec,x))

/***********************MULTI FLOATS**********************/
#if (defined(_ENABLE_MP_FLOATS) && defined(_ENABLE_LDOUBLE_FLOATS))

#define DECL_MULTI_FLOAT(x) \
	AUX_DECL_MULTI_FLOAT(FP_SINGLE,x);\
	AUX_DECL_MULTI_FLOAT(FP_DOUBLE,x);\
	AUX_DECL_MULTI_FLOAT(FP_LDOUBLE,x);\
	AUX_DECL_MULTI_FLOAT(FP_MP,x)

#define DECL_MULTI_COMPLEX_FLOAT(x) \
	AUX_DECL_MULTI_COMPLEX_FLOAT(FP_SINGLE,x);\
	AUX_DECL_MULTI_COMPLEX_FLOAT(FP_DOUBLE,x);\
	AUX_DECL_MULTI_COMPLEX_FLOAT(FP_LDOUBLE,x);\
	AUX_DECL_MULTI_COMPLEX_FLOAT(FP_MP,x)

#define INIT_MULTI_FLOAT(x) \
	AUX_INIT_MULTI_FLOAT(FP_SINGLE,x);\
	AUX_INIT_MULTI_FLOAT(FP_DOUBLE,x);\
	AUX_INIT_MULTI_FLOAT(FP_LDOUBLE,x);\
	AUX_INIT_MULTI_FLOAT(FP_MP,x)

#define INIT_MULTI_COMPLEX_FLOAT(x) \
	AUX_INIT_MULTI_COMPLEX_FLOAT(FP_SINGLE,x);\
	AUX_INIT_MULTI_COMPLEX_FLOAT(FP_DOUBLE,x);\
	AUX_INIT_MULTI_COMPLEX_FLOAT(FP_LDOUBLE,x);\
	AUX_INIT_MULTI_COMPLEX_FLOAT(FP_MP,x)

#define CLEAR_MULTI_FLOAT(x) \
	AUX_CLEAR_MULTI_FLOAT(FP_SINGLE,x);\
	AUX_CLEAR_MULTI_FLOAT(FP_DOUBLE,x);\
	AUX_CLEAR_MULTI_FLOAT(FP_LDOUBLE,x);\
	AUX_CLEAR_MULTI_FLOAT(FP_MP,x)

#define CLEAR_MULTI_COMPLEX_FLOAT(x) \
	AUX_CLEAR_MULTI_COMPLEX_FLOAT(FP_SINGLE,x);\
	AUX_CLEAR_MULTI_COMPLEX_FLOAT(FP_DOUBLE,x);\
	AUX_CLEAR_MULTI_COMPLEX_FLOAT(FP_LDOUBLE,x);\
	AUX_CLEAR_MULTI_COMPLEX_FLOAT(FP_MP,x)

#define AUX_ASSIGN_MULTI_FLOAT(fprec,dst,src) \
	fromBiggestF(fprec,FLOAT_VAR(fprec,dst),src)
#define ASSIGN_MULTI_FLOAT(dst,src) \
	AUX_ASSIGN_MULTI_FLOAT(FP_SINGLE,dst,src);\
	AUX_ASSIGN_MULTI_FLOAT(FP_DOUBLE,dst,src);\
	AUX_ASSIGN_MULTI_FLOAT(FP_LDOUBLE,dst,src);\
	AUX_ASSIGN_MULTI_FLOAT(FP_MP,dst,src)

#define AUX_ASSIGN_MULTI_COMPLEX_FLOAT(fprec,dst,src) \
	cfromBiggestF(fprec,FLOAT_VAR(fprec,dst),src)
#define ASSIGN_MULTI_COMPLEX_FLOAT(dst,src) \
	AUX_ASSIGN_MULTI_COMPLEX_FLOAT(FP_SINGLE,dst,src);\
	AUX_ASSIGN_MULTI_COMPLEX_FLOAT(FP_DOUBLE,dst,src);\
	AUX_ASSIGN_MULTI_COMPLEX_FLOAT(FP_LDOUBLE,dst,src);\
	AUX_ASSIGN_MULTI_COMPLEX_FLOAT(FP_MP,dst,src)

#elif (defined(_ENABLE_MP_FLOATS) && !defined(_ENABLE_LDOUBLE_FLOATS))

#define DECL_MULTI_FLOAT(x) \
	AUX_DECL_MULTI_FLOAT(FP_SINGLE,x);\
	AUX_DECL_MULTI_FLOAT(FP_DOUBLE,x);\
	AUX_DECL_MULTI_FLOAT(FP_MP,x)

#define DECL_MULTI_COMPLEX_FLOAT(x) \
	AUX_DECL_MULTI_COMPLEX_FLOAT(FP_SINGLE,x);\
	AUX_DECL_MULTI_COMPLEX_FLOAT(FP_DOUBLE,x);\
	AUX_DECL_MULTI_COMPLEX_FLOAT(FP_MP,x)

#define INIT_MULTI_FLOAT(x) \
	AUX_INIT_MULTI_FLOAT(FP_SINGLE,x);\
	AUX_INIT_MULTI_FLOAT(FP_DOUBLE,x);\
	AUX_INIT_MULTI_FLOAT(FP_MP,x)

#define INIT_MULTI_COMPLEX_FLOAT(x) \
	AUX_INIT_MULTI_COMPLEX_FLOAT(FP_SINGLE,x);\
	AUX_INIT_MULTI_COMPLEX_FLOAT(FP_DOUBLE,x);\
	AUX_INIT_MULTI_COMPLEX_FLOAT(FP_MP,x);

#define CLEAR_MULTI_FLOAT(x) \
	AUX_CLEAR_MULTI_FLOAT(FP_SINGLE,x);\
	AUX_CLEAR_MULTI_FLOAT(FP_DOUBLE,x);\
	AUX_CLEAR_MULTI_FLOAT(FP_MP,x)

#define CLEAR_MULTI_COMPLEX_FLOAT(x) \
	AUX_CLEAR_MULTI_COMPLEX_FLOAT(FP_SINGLE,x);\
	AUX_CLEAR_MULTI_COMPLEX_FLOAT(FP_DOUBLE,x);\
	AUX_CLEAR_MULTI_COMPLEX_FLOAT(FP_MP,x)

#define AUX_ASSIGN_MULTI_FLOAT(fprec,dst,src) \
	fromBiggestF(fprec,FLOAT_VAR(fprec,dst),src)
#define ASSIGN_MULTI_FLOAT(dst,src) \
	AUX_ASSIGN_MULTI_FLOAT(FP_SINGLE,dst,src);\
	AUX_ASSIGN_MULTI_FLOAT(FP_DOUBLE,dst,src);\
	AUX_ASSIGN_MULTI_FLOAT(FP_MP,dst,src)

#define AUX_ASSIGN_MULTI_COMPLEX_FLOAT(fprec,dst,src) \
	cfromBiggestF(fprec,FLOAT_VAR(fprec,dst),src)
#define ASSIGN_MULTI_COMPLEX_FLOAT(dst,src) \
	AUX_ASSIGN_MULTI_COMPLEX_FLOAT(FP_SINGLE,dst,src);\
	AUX_ASSIGN_MULTI_COMPLEX_FLOAT(FP_DOUBLE,dst,src);\
	AUX_ASSIGN_MULTI_COMPLEX_FLOAT(FP_MP,dst,src)

#elif (!defined(_ENABLE_MP_FLOATS) && defined(_ENABLE_LDOUBLE_FLOATS))

#define DECL_MULTI_FLOAT(x) \
	AUX_DECL_MULTI_FLOAT(FP_SINGLE,x);\
	AUX_DECL_MULTI_FLOAT(FP_DOUBLE,x);\
	AUX_DECL_MULTI_FLOAT(FP_LDOUBLE,x)

#define DECL_MULTI_COMPLEX_FLOAT(x) \
	AUX_DECL_MULTI_COMPLEX_FLOAT(FP_SINGLE,x);\
	AUX_DECL_MULTI_COMPLEX_FLOAT(FP_DOUBLE,x);\
	AUX_DECL_MULTI_COMPLEX_FLOAT(FP_LDOUBLE,x)

#define INIT_MULTI_FLOAT(x) \
	AUX_INIT_MULTI_FLOAT(FP_SINGLE,x);\
	AUX_INIT_MULTI_FLOAT(FP_DOUBLE,x);\
	AUX_INIT_MULTI_FLOAT(FP_LDOUBLE,x)

#define INIT_MULTI_COMPLEX_FLOAT(x) \
	AUX_INIT_MULTI_COMPLEX_FLOAT(FP_SINGLE,x);\
	AUX_INIT_MULTI_COMPLEX_FLOAT(FP_DOUBLE,x);\
	AUX_INIT_MULTI_COMPLEX_FLOAT(FP_LDOUBLE,x);

#define CLEAR_MULTI_FLOAT(x) \
	AUX_CLEAR_MULTI_FLOAT(FP_SINGLE,x);\
	AUX_CLEAR_MULTI_FLOAT(FP_DOUBLE,x);\
	AUX_CLEAR_MULTI_FLOAT(FP_LDOUBLE,x)

#define CLEAR_MULTI_COMPLEX_FLOAT(x) \
	AUX_CLEAR_MULTI_COMPLEX_FLOAT(FP_SINGLE,x);\
	AUX_CLEAR_MULTI_COMPLEX_FLOAT(FP_DOUBLE,x);\
	AUX_CLEAR_MULTI_COMPLEX_FLOAT(FP_LDOUBLE,x)

#define AUX_ASSIGN_MULTI_FLOAT(fprec,dst,src) \
	fromBiggestF(fprec,FLOAT_VAR(fprec,dst),src)
#define ASSIGN_MULTI_FLOAT(dst,src) \
	AUX_ASSIGN_MULTI_FLOAT(FP_SINGLE,dst,src);\
	AUX_ASSIGN_MULTI_FLOAT(FP_DOUBLE,dst,src);\
	AUX_ASSIGN_MULTI_FLOAT(FP_LDOUBLE,dst,src)

#define AUX_ASSIGN_MULTI_COMPLEX_FLOAT(fprec,dst,src) \
	cfromBiggestF(fprec,FLOAT_VAR(fprec,dst),src)
#define ASSIGN_MULTI_COMPLEX_FLOAT(dst,src) \
	AUX_ASSIGN_MULTI_COMPLEX_FLOAT(FP_SINGLE,dst,src);\
	AUX_ASSIGN_MULTI_COMPLEX_FLOAT(FP_DOUBLE,dst,src);\
	AUX_ASSIGN_MULTI_COMPLEX_FLOAT(FP_LDOUBLE,dst,src)

#else // !defined(_ENABLE_MP_FLOATS) && !defined(_ENABLE_LDOUBLE_FLOATS)

#define DECL_MULTI_FLOAT(x) \
	AUX_DECL_MULTI_FLOAT(FP_SINGLE,x);\
	AUX_DECL_MULTI_FLOAT(FP_DOUBLE,x)

#define DECL_MULTI_COMPLEX_FLOAT(x) \
	AUX_DECL_MULTI_COMPLEX_FLOAT(FP_SINGLE,x);\
	AUX_DECL_MULTI_COMPLEX_FLOAT(FP_DOUBLE,x)

#define INIT_MULTI_FLOAT(x) \
	AUX_INIT_MULTI_FLOAT(FP_SINGLE,x);\
	AUX_INIT_MULTI_FLOAT(FP_DOUBLE,x)

#define INIT_MULTI_COMPLEX_FLOAT(x) \
	AUX_INIT_MULTI_COMPLEX_FLOAT(FP_SINGLE,x);\
	AUX_INIT_MULTI_COMPLEX_FLOAT(FP_DOUBLE,x)

#define CLEAR_MULTI_FLOAT(x) \
	AUX_CLEAR_MULTI_FLOAT(FP_SINGLE,x);\
	AUX_CLEAR_MULTI_FLOAT(FP_DOUBLE,x)

#define CLEAR_MULTI_COMPLEX_FLOAT(x) \
	AUX_CLEAR_MULTI_COMPLEX_FLOAT(FP_SINGLE,x);\
	AUX_CLEAR_MULTI_COMPLEX_FLOAT(FP_DOUBLE,x)

#define AUX_ASSIGN_MULTI_FLOAT(fprec,dst,src) \
	fromBiggestF(fprec,FLOAT_VAR(fprec,dst),src)
#define ASSIGN_MULTI_FLOAT(dst,src) \
	AUX_ASSIGN_MULTI_FLOAT(FP_SINGLE,dst,src);\
	AUX_ASSIGN_MULTI_FLOAT(FP_DOUBLE,dst,src)

#define AUX_ASSIGN_MULTI_COMPLEX_FLOAT(fprec,dst,src) \
	cfromBiggestF(fprec,FLOAT_VAR(fprec,dst),src)
#define ASSIGN_MULTI_COMPLEX_FLOAT(dst,src) \
	AUX_ASSIGN_MULTI_COMPLEX_FLOAT(FP_SINGLE,dst,src);\
	AUX_ASSIGN_MULTI_COMPLEX_FLOAT(FP_DOUBLE,dst,src)

#endif
/*********************************************************/

#ifdef __cplusplus
}
#endif

#endif

