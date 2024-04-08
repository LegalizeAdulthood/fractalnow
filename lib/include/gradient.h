/*
 *  gradient.h -- part of FractalNow
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
  * \file gradient.h
  * \brief Header file related to gradients.
  * \author Marc Pegon
  */

#ifndef __GRADIENT_H__
#define __GRADIENT_H__

#include "color.h"
#include <stdint.h>
#include <stdio.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * \def DEFAULT_GRADIENT_TRANSITIONS
 * \brief Default number of transitions between two colors used for gradient.
 */
#define DEFAULT_GRADIENT_TRANSITIONS (uint_fast32_t)(UINT16_MAX+1)

/**
 * \def DEFAULT_GRADIENT_SIZE
 * \brief Default gradient size (total number of colors).
 */
#define DEFAULT_GRADIENT_SIZE (uint_fast32_t)(200000)

/**
 * \struct Gradient
 * \brief Simple gradient structure.
 *
 * This structure is to represent a gradient, i.e. continous
 * transitions between colors.
 */
/**
 * \typedef Gradient
 * \brief Convenient typedef for struct Gradient.
 */
typedef struct Gradient {
	uint_fast8_t bytesPerComponent;
 /*!< Colors bytes per component.*/
	uint_fast64_t size;
 /*!< Gradient size (total number of colors).*/
	Color *data;
 /*!< Gradient data.*/
	uint_fast32_t nbStops;
 /*!< Number of stops i.e. (pos,color) pairs used to build the gradient.*/
	double *positionStop;
 /*!< Stops positions. */
	Color *colorStop;
 /*!< Stops colors. */
} Gradient;

/**
 * \fn void GenerateGradient(Gradient *gradient, uint_fast32_t nbStops, double *positionStop, Color *colorStop, uint_fast32_t size)
 * \brief Generate a gradient.
 *
 * Number of stops must be > 2 (exit program with error otherwise).\n
 * Position stops must begin with 0, end with 1, and be strictly
 * increasing (exit program with error otherwise).\n
 * Gradient size must be stricly positive (exit program with
 * error otherwise).\n
 * All the colors in given array must have the same number of bytes
 * per component (undefined behaviour otherwise).
 *
 * \param gradient Pointer to the (already allocated) gradient structure to initialize.
 * \param nbStops Number of stops (different colors) for gradient.
 * \param positionStop Position stops.
 * \param colorStop Color stops.
 * \param size Gradient size.
 */
void GenerateGradient(Gradient *gradient, uint_fast32_t nbStops, double *positionStop, Color *colorStop,
				uint_fast32_t size);

/**
 * \fn void GenerateGradient2(Gradient *gradient, uint_fast32_t nbStops, Color *colorStop, uint_fast32_t nbTransitions)
 * \brief Generate a gradient.
 *
 * Generate a gradient from colors stops and the number of transitions
 * between two colors.\n
 * Number of stops must be > 2 (exit with error otherwise).\n
 * All the colors in given array must have the same number of bytes
 * per component (undefined behaviour otherwise).\n
 * Number of transitions must be strictly positive (exit program with
 * error otherwise).
 *
 * \param gradient Pointer to the (already allocated) gradient structure to initialize.
 * \param nbStops Number of stops (different colors) for gradient.
 * \param colorStop Color stops.
 * \param nbTransitions Number of transitions between two colors in the gradient.
 */
void GenerateGradient2(Gradient *gradient, uint_fast32_t nbStops, Color *colorStop, uint_fast32_t nbTransitions);

/**
 * \fn int isSupportedGradientFile(const char *fileName)
 * \brief Check whether a file is a supported gradient file.
 *
 * \param fileName File name.
 * \return 1 if file is a supported gradient file, 0 otherwise.
 */
int isSupportedGradientFile(const char *fileName);

/**
 * \fn int ReadGradientFileBody(Gradient *gradient, uint_fast8_t bytesPerComponent, const char *fileName, FILE *file, const char *format)
 * \brief Create gradient from gradient file body.
 *
 * The body of a gradient file is everything that comes after
 * the format version and the bytes per component.\n
 * fileName is used only for error messages.\n
 * This function should only be used internally by the library.
 *
 * \param gradient Pointer to the gradient structure to create.
 * \param bytesPerComponent Gradient file bytes per component (read from header).
 * \param fileName Gradient file name.
 * \param file Pointer to opened file, positioned at the beginning of the body.
 * \param format Gradient file format.
 * \return 0 in case of success, 1 in case of failure.
 */
int ReadGradientFileBody(Gradient *gradient, uint_fast8_t bytesPerComponent, const char *fileName,
			FILE *file, const char *format);

/**
 * \fn int ReadGradientFile(Gradient *gradient, const char *fileName)
 * \brief Read and parse a gradient file, and create the according gradient.
 *
 * \param gradient Pointer to the gradient structure to create.
 * \param fileName Gradient file name.
 * \return 0 in case of success, 1 in case of failure.
 */
int ReadGradientFile(Gradient *gradient, const char *fileName);

/**
 * \fn int WriteGradientFileBody(const Gradient *gradient, const char *fileName, FILE *file, const char *format)
 * \brief Write gradient file body.
 *
 * The body of a gradient file is everything that comes after
 * the format version and the bytes per component.\n
 * fileName is used only for error messages.\n
 * This function should only be used internally by the library.
 *
 * \param gradient Gradient to write into file.
 * \param fileName Gradient file name.
 * \param file Pointer to opened file, positioned at the beginning of the body.
 * \param format Gradient file format.
 * \return 0 in case of success, 1 in case of failure.
 */
int WriteGradientFileBody(const Gradient *gradient, const char *fileName, FILE *file,
				const char *format);

/**
 * \fn int WriteGradientFile(const Gradient *gradient, const char *fileName)
 * \brief Write gradient into file.
 *
 * \param gradient Gradient to write into file.
 * \param fileName Gradient file name.
 * \return 0 in case of success, 1 in case of failure.
 */
int WriteGradientFile(const Gradient *gradient, const char *fileName);

/**
 * \fn Gradient CopyGradient(const Gradient *gradient)
 * \brief Copy gradient.
 *
 * \param gradient Pointer to gradient to copy.
 * \return Copy of gradient.
 */
Gradient CopyGradient(const Gradient *gradient);

/**
 * \fn Gradient Gradient16(const Gradient *gradient)
 * \brief Convert gradient to 16 bits gradient.
 *
 * Simple copy if gradient colors depth is already 16.
 *
 * \param gradient Pointer to gradient to be converted.
 * \return Newly-allocated 16 bits gradient.
 */
Gradient Gradient16(const Gradient *gradient);

/**
 * \fn Gradient Gradient8(const Gradient *gradient)
 * \brief Convert gradient to 8 bits gradient.
 *
 * Simple copy if gradient colors depth is already 8.
 *
 * \param gradient Pointer to gradient to be converted.
 * \return Newly-allocated 8 bits gradient.
 */
Gradient Gradient8(const Gradient *gradient);

/**
 * \fn Color GetGradientColor(const Gradient *gradient, uint_fast64_t index)
 * \brief Get a color in the gradient.
 *
 * Get the color in gradient at position index mod gradient_size
 * 
 * \param gradient Pointer to the gradient to get the color from.
 * \param index (mod.) Index of the color to get.
 * \return Color in gradient at position (index mod gradient_size).
 */
Color GetGradientColor(const Gradient *gradient, uint_fast64_t index);

/**
 * \fn void FreeGradient(Gradient gradient)
 * \brief Free gradient.
 *
 * \param gradient Gradient to be freed.
 */
void FreeGradient(Gradient gradient);

#ifdef __cplusplus
}
#endif

#endif
