/*
 *  macro_build_floats.h -- part of FractalNow
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
  * \file macro_build_floats.h
  * \brief Header file related to macros for building functions operating on multiple float types.
  *
  * \author Marc Pegon
  */

#ifndef __MACRO_BUILD_FLOATS_H__
#define __MACRO_BUILD_FLOATS_H__

#ifdef __cplusplus
extern "C" {
#endif

#define BUILD_FLOAT(fprec) MACRO_BUILD_FLOAT(fprec)

#if defined(_ENABLE_MP_FLOATS) && defined(_ENABLE_LDOUBLE_FLOATS)
#define BUILD_FLOATS \
	BUILD_FLOAT(FP_SINGLE) \
	BUILD_FLOAT(FP_DOUBLE) \
	BUILD_FLOAT(FP_LDOUBLE) \
	BUILD_FLOAT(FP_MP)
#elif defined(_ENABLE_MP_FLOATS) && !defined(_ENABLE_LDOUBLE_FLOATS)
#define BUILD_FLOATS \
	BUILD_FLOAT(FP_SINGLE) \
	BUILD_FLOAT(FP_DOUBLE) \
	BUILD_FLOAT(FP_MP)
#elif !defined(_ENABLE_MP_FLOATS) && defined(_ENABLE_LDOUBLE_FLOATS)
#define BUILD_FLOATS \
	BUILD_FLOAT(FP_SINGLE) \
	BUILD_FLOAT(FP_LDOUBLE) \
	BUILD_FLOAT(FP_DOUBLE)
#else
#define BUILD_FLOATS \
	BUILD_FLOAT(FP_SINGLE) \
	BUILD_FLOAT(FP_DOUBLE)
#endif

#define MACRO_BUILD_FLOATS BUILD_FLOATS

#ifdef __cplusplus
}
#endif

#endif

