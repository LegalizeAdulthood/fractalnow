/*
 *  fractal_anti_aliasing.h -- part of fractal2D
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
  * \file fractal_anti_aliasing.h
  * \brief Header file related to anti-aliasing methods.
  * \author Marc Pegon
  */

#ifndef __FRACTAL_ANTI_ALIASING_H__
#define __FRACTAL_ANTI_ALIASING_H__

/**
 * \enum e_AntiAliasingMethod
 * \brief Possible anti-aliasing methods.
 */
typedef enum e_AntiAliasingMethod {
	AAM_NONE = 0, /*!< No anti-aliasing.*/
	AAM_GAUSSIANBLUR, /*!< Gaussian Blur.*/
	AAM_OVERSAMPLING, /*!< Over sampling (compute a bigger image and scale it down).*/
	AAM_ADAPTIVE /*!< Adaptive (compute more than one value for some pixels).*/
} AntiAliasingMethod;

/**
 * \fn AntiAliasingMethod GetAAM(const char *str)
 * \brief Get anti-aliasing method from string.
 *
 * Function is case insensitive.
 * Possible strings are :
 * - "none" for AAM_NONE
 * - "blur" for AAM_GAUSSIANBLUR
 * - "oversampling" for AAM_OVERSAMPLING
 * - "adaptive" for AAM_ADAPTIVE
 * Exit with error in case of failure.
 *
 * \param str String specifying anti-aliasing method.
 * \return Corresponding anti-aliasing method, or AAM_UNKNOWN in case of failure.
 */
AntiAliasingMethod GetAAM(const char *str);

#endif
