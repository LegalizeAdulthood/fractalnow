/*
 *  main.h -- part of FractalNow
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
  * \file main.h
  * \brief Header file related to QFractalNow program.
  * \author Marc Pegon
  */

#ifndef __MAIN_H__
#define __MAIN_H__

#define DEFAULT_MIN_ANTIALIASING_SIZE (uint_fast32_t)(3)
#define DEFAULT_MAX_ANTIALIASING_SIZE (uint_fast32_t)(3)
#define DEFAULT_ANTIALIASING_SIZE_ITERATION (uint_fast32_t)(2)
#define DEFAULT_DECIMAL_NUMBER (int)(20)
#define MIN_SINGLE_STEP (double)(pow(10, -DBL_DIG))

#endif
