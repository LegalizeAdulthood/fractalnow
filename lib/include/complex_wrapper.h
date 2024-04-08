/*
 *  complex_wrapper.h -- part of FractalNow
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
  * \file complex_wrapper.h
  * \brief Header file for wrapper for complex numbers.
  *
  * Either builtin or standard C99.
  *
  * \author Marc Pegon
  */
 
#ifndef __COMPLEX_WRAPPER_H__
#define __COMPLEX_WRAPPER_H__

#ifdef _USE_BUILTIN_COMPLEX
#include "builtin_complex.h"
#else
#include "c99_complex_wrapper.h"
#endif

#endif
