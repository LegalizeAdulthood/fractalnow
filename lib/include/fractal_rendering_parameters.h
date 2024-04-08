/*
 *  fractal_rendering_parameters.h -- part of FractalNow
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
  * \file fractal_rendering_parameters.h
  * \brief Header file related to rendering parameters.
  * \author Marc Pegon
  */

#ifndef __FRACTAL_RENDERING_PARAMETERS_H__
#define __FRACTAL_RENDERING_PARAMETERS_H__

#ifdef __cplusplus
extern "C" {
#endif

#include "color.h"
#include "gradient.h"
#include "fractal_addend_function.h"
#include "fractal_coloring.h"
#include "fractal_counting_function.h"
#include "fractal_transfer_function.h"
#include <stdint.h>

/**
 * \struct RenderingParameters
 * \brief Structure for handling fractal rendering parameters.
 */
/**
 * \typedef RenderingParameters
 * \brief Convenient typedef for struct RenderingParameters.
 */
typedef struct RenderingParameters {
	uint_fast8_t bytesPerComponent;
 /*!< Bytes per component matching gradient colors'.*/
	Color spaceColor;
 /*!< Fractal space color.*/
	CountingFunction countingFunction;
 /*!< Fractal counting function enum value.*/
	CountingFunctionPtr countingFunctionPtr;
 /*!< Fractal counting function ptr.*/
	ColoringMethod coloringMethod;
 /*!< Fractal coloring method.*/
	AddendFunction addendFunction;
 /*!< Fractal addend function (used only for average coloring method).*/
	uint_fast32_t stripeDensity;
 /*!< Stripe density (used only for AF_STRIPE addend function).*/
	InterpolationMethod interpolationMethod;
 /*!< Fractal interpolation method (used only for average coloring method).*/
	TransferFunction transferFunction;
 /*!< Fractal transfer function enum value (to make the values fit the gradient better).*/
	TransferFunctionPtr transferFunctionPtr;
 /*!< Transfer function ptr.*/
	FLOATT multiplier;
 /*!< Value with which fractal values will be multiplied (to make the values fit the gradient better).*/
	FLOATT offset;
 /*!< Offset for mapping value to gradient.*/
	Gradient gradient;
 /*!< Gradient for mapping float values to colors.*/

 /* For internal use.*/
	FLOATT realMultiplier;
 /*!< Real multiplier (scaled according to gradient size).*/
	FLOATT realOffset;
 /*!< Real offset (scaled according to gradient size).*/
} RenderingParameters;

/**
 * \fn void InitRenderingParameters(RenderingParameters *param, uint_fast8_t bytesPerComponent, Color spaceColor, CountingFunction countingFunction, ColoringMethod coloringMethod, AddendFunction addendFunction, uint_fast32_t stripeDensity, InterpolationMethod interpolationMethod, TransferFunction transferFunction, FLOATT multiplier, FLOATT offset, Gradient gradient)
 * \brief Initialize rendering parameters.
 *
 * Bytes per component *must* agree with space color and gradient colors :
 * this is *not* checked by the function.\n
 * Gradient will be owned by rendering parameters, and free'd when
 * rendering parameters are free'd.
 *
 * \param param Pointer to structure to initialize.
 * \param bytesPerComponent Bytes per component for colors of rendering.
 * \param spaceColor The color for fractal space.
 * \param countingFunction  Fractal counting function.
 * \param coloringMethod Fractal coloring method.
 * \param addendFunction Fractal addend function (used only for average coloring method).
 * \param stripeDensity Stripe density (used only for AF_STRIPE addend function).
 * \param interpolationMethod Fractal interpolation method (used only for average coloring method).
 * \param transferFunction Fractal transfer function (to make the values fit the gradient better).
 * \param multiplier Value with which fractal values will be multiplied (to make the values fit the gradient better).
 * \param offset Offset for mapping value to gradient.
 * \param gradient Gradient for mapping float values to colors.
 */
void InitRenderingParameters(RenderingParameters *param, uint_fast8_t bytesPerComponent, Color spaceColor,
				CountingFunction countingFunction, ColoringMethod coloringMethod,
				AddendFunction addendFunction, uint_fast32_t stripeDensity,
				InterpolationMethod interpolationMethod, TransferFunction transferFunction,
				FLOATT multiplier, FLOATT offset, Gradient gradient);

/**
 * \fn RenderingParameters CopyRenderingParameters(const RenderingParameters *render)
 * \brief Copy rendering parameters.
 *
 * \param render Pointer to rendering parameters to copy.
 * \return Copy of rendering parameters.
 */
RenderingParameters CopyRenderingParameters(const RenderingParameters *param);

/**
 * \fn void ResetGradient(RenderingParameters *param, Gradient gradient)
 * \brief Reset gradient.
 *
 * Gradient will be owned by rendering parameters, and will be free'd
 * when rendering parameters are free'd.
 *
 * \param param Rendering parameters to be changed.
 * \param gradient New Gradient.
 */
void ResetGradient(RenderingParameters *param, Gradient gradient);

/**
 * \fn int isSupportedRenderingFile(const char *fileName)
 * \brief Check whether a file is a supported rendering file.
 *
 * \param fileName File name.
 * \return 1 if file is a supported rendering file, 0 otherwise.
 */
int isSupportedRenderingFile(const char *fileName);

/**
 * \fn int ReadRenderingFileBody(RenderingParameters *param, const char *fileName, FILE *file, const char *format)
 * \brief Read fractal rendering parameters from rendering file body.
 *
 * The body of a rendering file is everything that comes after
 * the format version.\n
 * fileName is used only for error messages.\n
 * This function should only be used internally by the library.
 *
 * \param param Pointer to the rendering parameters structure to create.
 * \param fileName Rendering file name.
 * \param file Pointer to opened file, positioned at the beginning of the body.
 * \param format Rendering file format.
 * \return 0 in case of success, 1 in case of failure.
 */
int ReadRenderingFileBody(RenderingParameters *param, const char *fileName,
			FILE *file, const char *format);

/**
 * \fn int ReadRenderingFile(RenderingParameters *param, const char *fileName)
 * \brief Read and parse fractal rendering parameters file.
 *
 * \param param Pointer to the structure to store rendering parameters.
 * \param fileName Rendering file name.
 * \return 0 in case of success, 1 in case of failure.
 */
int ReadRenderingFile(RenderingParameters *param, const char *fileName);

/**
 * \fn int WriteRenderingFileBody(const RenderingParameters *param, const char *fileName, FILE *file, const char *format)
 * \brief Write rendering parameters into file.
 *
 * The body of a rendering file is everything that comes after
 * the format version.\n
 * fileName is used only for error messages.\n
 * This function should only be used internally by the library.
 *
 * \param param Rendering parameters to write.
 * \param fileName Rendering file name.
 * \param file Pointer to opened file, positioned at the beginning of the body.
 * \param format Rendering file format.
 * \return 0 in case of success, 1 in case of failure.
 */
int WriteRenderingFileBody(const RenderingParameters *param, const char *fileName,
				FILE *file, const char *format);

/**
 * \fn int WriteRenderingFile(const RenderingParameters *param, const char *fileName)
 * \brief Write rendering parameters into file.
 *
 * \param param Rendering parameters to write.
 * \param fileName Rendering file name.
 * \return 0 in case of success, 1 in case of failure.
 */
int WriteRenderingFile(const RenderingParameters *param, const char *fileName);

/**
 * \fn void FreeRenderingParameters(RenderingParameters param)
 * \brief Free a RenderingParameters structure.
 *
 * \param param RenderingParameters structure to be freed.
 */
void FreeRenderingParameters(RenderingParameters param);

#ifdef __cplusplus
}
#endif

#endif
