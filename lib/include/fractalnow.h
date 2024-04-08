/*
 *  fractalnow.h -- part of FractalNow
 *
 *  Copyright (c) 2011 Marc Pegon <pe.marc@free.fr>
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
  * \file fractalnow.h
  * \brief Main FractalNow lib header file.
  * \author Marc Pegon
  */

#ifndef __FRACTALNOW_H__
#define __FRACTALNOW_H__

#include "color.h"
#include "complex_wrapper.h"
#include "error.h"
#include "file_io.h"
#include "filter.h"
#include "float_precision.h"
#include "fractal_addend_function.h"
#include "fractal_cache.h"
#include "fractal_coloring.h"
#include "fractal_compute_engine.h"
#include "fractal_config.h"
#include "fractal_formula.h"
#include "fractal.h"
#include "fractal_iteration_count.h"
#include "fractal_rendering_parameters.h"
#include "fractal_transfer_function.h"
#include "gradient.h"
#include "image.h"
#include "misc.h"
#include "ppm.h"
#include "uirectangle.h"
#include "task.h"
#include "thread.h"

#ifdef __cplusplus
extern "C" {
#endif

#ifndef VERSION_NUMBER
/**
 * \def VERSION_NUMBER
 * \brief FractalNow version number.
 */
#define VERSION_NUMBER ?.?.?
#endif

/**
 * \fn const char *FractalNow_VersionNumber()
 * \brief Get FractalNow lib version number.
 */
const char *FractalNow_VersionNumber();

void FractalNow_Init();

#ifdef __cplusplus
}
#endif

#endif

