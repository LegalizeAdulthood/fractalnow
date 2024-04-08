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

#include <stdlib.h>
#include <inttypes.h>
#include <math.h>
#include <complex.h>
#include <mpfr.h>
#include <mpc.h>

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
	FP_LDOUBLE,
 /*!< Extended precision (long double).*/
	FP_MP
 /* Multiple precision (mpfr_t and mpc_t).*/
} FloatPrecision;

#define FLOATTYPE_FP_SINGLE float
#define COMPLEX_FLOATTYPE_FP_SINGLE FLOATTYPE_FP_SINGLE complex
#define FLOATTYPE_FP_DOUBLE double
#define COMPLEX_FLOATTYPE_FP_DOUBLE FLOATTYPE_FP_DOUBLE complex
#define FLOATTYPE_FP_LDOUBLE long double
#define COMPLEX_FLOATTYPE_FP_LDOUBLE FLOATTYPE_FP_LDOUBLE complex
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
 /*!< double type value.*/
	FLOATTYPE(FP_LDOUBLE) val_FP_LDOUBLE;
 /*!< long double type value.*/
	FLOATTYPE(FP_MP) val_FP_MP;
 /*!< mpfr_t type value.*/
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
 * - "mp" for FP_MP
 *
 * \param floatPrecision Float precision destination.
 * \param str String specifying float precision.
 * \return 0 in case of success, 1 in case of failure.
 */
int GetFloatPrecision(FloatPrecision *floatPrecision, const char *str);

/************************FP_SINGLE************************/
#define init_FP_SINGLE(x) (void)NULL
#define clear_FP_SINGLE(x) (void)NULL
#define cinit_FP_SINGLE(x) (void)NULL
#define cclear_FP_SINGLE(x) (void)NULL

#define assign_FP_SINGLE(dst,src) dst=src
#define cassign_FP_SINGLE(dst,src) dst=src

#define fromMPFR_FP_SINGLE(dst,src) dst=mpfr_get_flt(src,MPFR_RNDN)
#define fromDouble_FP_SINGLE(dst,src) dst=(FLOATTYPE_FP_SINGLE)src
#define fromUi_FP_SINGLE(dst,src) dst=(FLOATTYPE_FP_SINGLE)src
#define fromSi_FP_SINGLE(dst,src) dst=(FLOATTYPE_FP_SINGLE)src
#define toDouble_FP_SINGLE(src) (long double)src

#define cfromMPC_FP_SINGLE(dst,src) dst=mpfr_get_flt(mpc_realref(src),MPFR_RNDN)+I*mpfr_get_flt(mpc_imagref(src),MPFR_RNDN)
#define cfromUi_FP_SINGLE(dst,src) dst=(COMPLEX_FLOATTYPE_FP_SINGLE)src
#define cfromSi_FP_SINGLE(dst,src) dst=(COMPLEX_FLOATTYPE_FP_SINGLE)src
#define cfromReIm_FP_SINGLE(dst,re,im) dst=re+I*im
#define cfromCDouble_FP_SINGLE(dst,src) dst=(COMPLEX_FLOATTYPE_FP_SINGLE)src
#define ctoCDouble_FP_SINGLE(src) (complex long double)src

#define cmp_FP_SINGLE(x,y) ((x<y)?-1:((x>y)?1:0))
#define cmp_ui_FP_SINGLE(x,y) ((x<y)?-1:((x>y)?1:0))
#define cmp_si_FP_SINGLE(x,y) ((x<y)?-1:((x>y)?1:0))
#define eq_ui_FP_SINGLE(x,y) (x==y)
#define eq_si_FP_SINGLE(x,y) (x==y)
#define ceq_si_FP_SINGLE(x,y) (x==y)

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

#define cadd_FP_SINGLE(res,x,y) res=((x)+(y))
#define csub_FP_SINGLE(res,x,y) res=((x)-(y))
#define cmul_FP_SINGLE(res,x,y) res=((x)*(y))
#define cdiv_FP_SINGLE(res,x,y) res=((x)/(y))

#define sqr_FP_SINGLE(res,x) res=((x)*(x))
#define csqr_FP_SINGLE(res,x) res=((x)*(x))
#define fabs_FP_SINGLE(res,x) res=fabsf(x)
#define cabs_FP_SINGLE(res,z) res=cabsf(z)
#define cnorm_FP_SINGLE(res,z) res=(crealf(z)*crealf(z)+cimagf(z)*cimagf(z))
#define creal_FP_SINGLE(res,z) res=crealf(z)
#define cimag_FP_SINGLE(res,z) res=cimagf(z)
#define carg_FP_SINGLE(res,z) res=cargf(z)
#define cpow_FP_SINGLE(res,z,y) res=cpowf(z,y)
#define cipow_FP_SINGLE(res,z,y) res=cipowf(z,y)
#define conj_FP_SINGLE(res,z) res=conjf(z)
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
#define fromDouble_FP_DOUBLE(dst,src) dst=(FLOATTYPE_FP_DOUBLE)src
#define fromUi_FP_DOUBLE(dst,src) dst=(FLOATTYPE_FP_DOUBLE)src
#define fromSi_FP_DOUBLE(dst,src) dst=(FLOATTYPE_FP_DOUBLE)src
#define toDouble_FP_DOUBLE(src) (long double)src

#define cfromMPC_FP_DOUBLE(dst,src) dst=mpfr_get_d(mpc_realref(src),MPFR_RNDN)+I*mpfr_get_d(mpc_imagref(src),MPFR_RNDN)
#define cfromUi_FP_DOUBLE(dst,src) dst=(COMPLEX_FLOATTYPE_FP_DOUBLE)src
#define cfromSi_FP_DOUBLE(dst,src) dst=(COMPLEX_FLOATTYPE_FP_DOUBLE)src
#define cfromReIm_FP_DOUBLE(dst,re,im) dst=re+I*im
#define cfromCDouble_FP_DOUBLE(dst,src) dst=(COMPLEX_FLOATTYPE_FP_DOUBLE)src
#define ctoCDouble_FP_DOUBLE(src) (complex long double)src

#define cmp_FP_DOUBLE(x,y) ((x<y)?-1:((x>y)?1:0))
#define cmp_ui_FP_DOUBLE(x,y) ((x<y)?-1:((x>y)?1:0))
#define cmp_si_FP_DOUBLE(x,y) ((x<y)?-1:((x>y)?1:0))
#define eq_ui_FP_DOUBLE(x,y) (x==y)
#define eq_si_FP_DOUBLE(x,y) (x==y)
#define ceq_si_FP_DOUBLE(x,y) (x==y)

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

#define cadd_FP_DOUBLE(res,x,y) res=((x)+(y))
#define csub_FP_DOUBLE(res,x,y) res=((x)-(y))
#define cmul_FP_DOUBLE(res,x,y) res=((x)*(y))
#define cdiv_FP_DOUBLE(res,x,y) res=((x)/(y))

#define sqr_FP_DOUBLE(res,x) res=((x)*(x))
#define csqr_FP_DOUBLE(res,x) res=((x)*(x))
#define fabs_FP_DOUBLE(res,x) res=fabs(x)
#define cabs_FP_DOUBLE(res,z) res=cabs(z)
#define cnorm_FP_DOUBLE(res,z) res=(creal(z)*creal(z)+cimag(z)*cimag(z))
#define creal_FP_DOUBLE(res,z) res=creal(z)
#define cimag_FP_DOUBLE(res,z) res=cimag(z)
#define carg_FP_DOUBLE(res,z) res=carg(z)
#define cpow_FP_DOUBLE(res,z,y) res=cpow(z,y)
#define cipow_FP_DOUBLE(res,z,y) res=cipow(z,y)
#define conj_FP_DOUBLE(res,z) res=conj(z)
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
#define fromDouble_FP_LDOUBLE(dst,src) dst=(FLOATTYPE_FP_LDOUBLE)src
#define fromUi_FP_LDOUBLE(dst,src) dst=(FLOATTYPE_FP_LDOUBLE)src
#define fromSi_FP_LDOUBLE(dst,src) dst=(FLOATTYPE_FP_LDOUBLE)src
#define toDouble_FP_LDOUBLE(src) (long double)src

#define cfromMPC_FP_LDOUBLE(dst,src) dst=mpfr_get_ld(mpc_realref(src),MPFR_RNDN)+I*mpfr_get_ld(mpc_imagref(src),MPFR_RNDN)
#define cfromUi_FP_LDOUBLE(dst,src) dst=(COMPLEX_FLOATTYPE_FP_LDOUBLE)src
#define cfromSi_FP_LDOUBLE(dst,src) dst=(COMPLEX_FLOATTYPE_FP_LDOUBLE)src
#define cfromReIm_FP_LDOUBLE(dst,re,im) dst=re+I*im
#define cfromCDouble_FP_LDOUBLE(dst,src) dst=(COMPLEX_FLOATTYPE_FP_LDOUBLE)src
#define ctoCDouble_FP_LDOUBLE(src) (complex long double)src

#define cmp_FP_LDOUBLE(x,y) ((x<y)?-1:((x>y)?1:0))
#define cmp_ui_FP_LDOUBLE(x,y) ((x<y)?-1:((x>y)?1:0))
#define cmp_si_FP_LDOUBLE(x,y) ((x<y)?-1:((x>y)?1:0))
#define ccmp_si_FP_LDOUBLE(x,y) ((x<y)?-1:((x>y)?1:0))
#define eq_ui_FP_LDOUBLE(x,y) (x==y)
#define eq_si_FP_LDOUBLE(x,y) (x==y)
#define ceq_si_FP_LDOUBLE(x,y) (x==y)

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

#define cadd_FP_LDOUBLE(res,x,y) res=((x)+(y))
#define csub_FP_LDOUBLE(res,x,y) res=((x)-(y))
#define cmul_FP_LDOUBLE(res,x,y) res=((x)*(y))
#define cdiv_FP_LDOUBLE(res,x,y) res=((x)/(y))

#define sqr_FP_LDOUBLE(res,x) res=((x)*(x))
#define csqr_FP_LDOUBLE(res,x) res=((x)*(x))
#define fabs_FP_LDOUBLE(res,x) res=fabsl(x)
#define cabs_FP_LDOUBLE(res,z) res=cabsl(z)
#define cnorm_FP_LDOUBLE(res,z) res=(creall(z)*creall(z)+cimagl(z)*cimagl(z))
#define creal_FP_LDOUBLE(res,z) res=creall(z)
#define cimag_FP_LDOUBLE(res,z) res=cimagl(z)
#define carg_FP_LDOUBLE(res,z) res=cargl(z)
#define cpow_FP_LDOUBLE(res,z,y) res=cpowl(z,y)
#define cipow_FP_LDOUBLE(res,z,y) res=cipowl(z,y)
#define conj_FP_LDOUBLE(res,z) res=conjl(z)
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
#define fromDouble_FP_MP(dst,src) mpfr_set_d(dst,src,MPFR_RNDN)
#define fromUi_FP_MP(dst,src) mpfr_set_ui(dst,src,MPFR_RNDN)
#define fromSi_FP_MP(dst,src) mpfr_set_si(dst,src,MPFR_RNDN)
#define toDouble_FP_MP(src) mpfr_get_ld(src,MPFR_RNDN)

#define cfromMPC_FP_MP(dst,src) mpc_set(dst,src,MPC_RNDNN)
#define cfromUi_FP_MP(dst,src) mpc_set_ui(dst,src,MPC_RNDNN)
#define cfromSi_FP_MP(dst,src) mpc_set_si(dst,src,MPC_RNDNN)
#define cfromReIm_FP_MP(dst,re,im) mpc_set_fr_fr(dst,re,im,MPC_RNDNN)
#define cfromCDouble_FP_MP(dst,src) mpc_set_d_d(dst,creal(src),cimag(src),MPC_RNDNN)
#define ctoCDouble_FP_MP(src) (mpfr_get_d(mpc_realref(src),MPFR_RNDN)+I*mpfr_get_d(mpc_imagref(src),MPFR_RNDN))

#define cmp_FP_MP(x,y) mpfr_cmp(x,y)
#define cmp_ui_FP_MP(x,y) mpfr_cmp_ui(x,y)
#define cmp_si_FP_MP(x,y) mpfr_cmp_si(x,y)
#define eq_ui_FP_MP(x,y) (mpfr_cmp_ui(x,y)==0)
#define eq_si_FP_MP(x,y) (mpfr_cmp_si(x,y)==0)
#define ceq_si_FP_MP(x,y) (mpc_cmp_si(x,y) == 0)

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
#define fromDoubleF(fprec,dst,src) fromDouble_##fprec(dst,src)
#define fromUiF(fprec,dst,src) fromUi_##fprec(dst,src)
#define fromSiF(fprec,dst,src) fromSi_##fprec(dst,src)
#define toDoubleF(fprec,src) toDouble_##fprec(src)

#define cfromMPCF(fprec,dst,src) cfromMPC_##fprec(dst,src)
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
#define DECL_MULTI_FLOAT(x) \
	AUX_DECL_MULTI_FLOAT(FP_SINGLE,x);\
	AUX_DECL_MULTI_FLOAT(FP_DOUBLE,x);\
	AUX_DECL_MULTI_FLOAT(FP_LDOUBLE,x);\
	AUX_DECL_MULTI_FLOAT(FP_MP,x)

#define AUX_DECL_MULTI_COMPLEX_FLOAT(fprec,x) DECL_COMPLEX_FLOAT(fprec,FLOAT_VAR(fprec,x))
#define DECL_MULTI_COMPLEX_FLOAT(x) \
	AUX_DECL_MULTI_COMPLEX_FLOAT(FP_SINGLE,x);\
	AUX_DECL_MULTI_COMPLEX_FLOAT(FP_DOUBLE,x);\
	AUX_DECL_MULTI_COMPLEX_FLOAT(FP_LDOUBLE,x);\
	AUX_DECL_MULTI_COMPLEX_FLOAT(FP_MP,x)

#define AUX_INIT_MULTI_FLOAT(fprec,x) initF(fprec,FLOAT_VAR(fprec,x))
#define INIT_MULTI_FLOAT(x) \
	AUX_INIT_MULTI_FLOAT(FP_SINGLE,x);\
	AUX_INIT_MULTI_FLOAT(FP_DOUBLE,x);\
	AUX_INIT_MULTI_FLOAT(FP_LDOUBLE,x);\
	AUX_INIT_MULTI_FLOAT(FP_MP,x)

#define AUX_INIT_MULTI_COMPLEX_FLOAT(fprec,x) cinitF(fprec,FLOAT_VAR(fprec,x))
#define INIT_MULTI_COMPLEX_FLOAT(x) \
	AUX_INIT_MULTI_COMPLEX_FLOAT(FP_SINGLE,x);\
	AUX_INIT_MULTI_COMPLEX_FLOAT(FP_DOUBLE,x);\
	AUX_INIT_MULTI_COMPLEX_FLOAT(FP_LDOUBLE,x);\
	AUX_INIT_MULTI_COMPLEX_FLOAT(FP_MP,x);

#define AUX_CLEAR_MULTI_FLOAT(fprec,x) clearF(fprec,FLOAT_VAR(fprec,x))
#define CLEAR_MULTI_FLOAT(x) \
	AUX_CLEAR_MULTI_FLOAT(FP_SINGLE,x);\
	AUX_CLEAR_MULTI_FLOAT(FP_DOUBLE,x);\
	AUX_CLEAR_MULTI_FLOAT(FP_LDOUBLE,x);\
	AUX_CLEAR_MULTI_FLOAT(FP_MP,x)

#define AUX_CLEAR_MULTI_COMPLEX_FLOAT(fprec,x) cclearF(fprec,FLOAT_VAR(fprec,x))
#define CLEAR_MULTI_COMPLEX_FLOAT(x) \
	AUX_CLEAR_MULTI_COMPLEX_FLOAT(FP_SINGLE,x);\
	AUX_CLEAR_MULTI_COMPLEX_FLOAT(FP_DOUBLE,x);\
	AUX_CLEAR_MULTI_COMPLEX_FLOAT(FP_LDOUBLE,x);\
	AUX_CLEAR_MULTI_COMPLEX_FLOAT(FP_MP,x);

#define AUX_ASSIGN_MULTI_FLOAT(fprec,dst,src) \
	fromMPFRF(fprec,FLOAT_VAR(fprec,dst),src)
#define ASSIGN_MULTI_FLOAT(dst,src) \
	AUX_ASSIGN_MULTI_FLOAT(FP_SINGLE,dst,src);\
	AUX_ASSIGN_MULTI_FLOAT(FP_DOUBLE,dst,src);\
	AUX_ASSIGN_MULTI_FLOAT(FP_LDOUBLE,dst,src);\
	AUX_ASSIGN_MULTI_FLOAT(FP_MP,dst,src)

#define AUX_ASSIGN_MULTI_COMPLEX_FLOAT(fprec,dst,src) \
	cfromMPCF(fprec,FLOAT_VAR(fprec,dst),src)
#define ASSIGN_MULTI_COMPLEX_FLOAT(dst,src) \
	AUX_ASSIGN_MULTI_COMPLEX_FLOAT(FP_SINGLE,dst,src);\
	AUX_ASSIGN_MULTI_COMPLEX_FLOAT(FP_DOUBLE,dst,src);\
	AUX_ASSIGN_MULTI_COMPLEX_FLOAT(FP_LDOUBLE,dst,src);\
	AUX_ASSIGN_MULTI_COMPLEX_FLOAT(FP_MP,dst,src)

#ifdef __cplusplus
}
#endif

#endif

