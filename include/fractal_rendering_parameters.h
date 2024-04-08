/*
 *  fractal_rendering_parameters.h -- part of fractal2D
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

#include "color.h"
#include "gradient.h"
#include "fractal_addend_function.h"
#include "fractal_coloring.h"
#include "fractal_counting_function.h"
#include "fractal_transfer_function.h"
#include <stdint.h>

/**
 * \struct s_RenderingParameters
 * \brief Structure containing fractal rendering parameters.
 */
typedef struct s_RenderingParameters {
	uint_fast8_t bytesPerComponent;
 /*!< Bytes per component for colors of rendering.*/
	Color spaceColor;
 /*!< The color for fractal space.*/
	CountingFunction countingFunction;
 /*!< Fractal iteration count function.*/
	ColoringMethod coloringMethod;
 /*!< Fractal coloring method.*/
	AddendFunction addendFunction;
 /*!< Fractal addend function (used only for CM_AVERAGE coloring method).*/
	InterpolationMethod interpolationMethod;
 /*!< Fractal interpolation method (used only for CM_AVERAGE coloring method.)*/
	TransferFunction transferFunction;
 /*!< Fractal transfer function (to make the values fit the gradient better).*/
	FLOAT multiplier;
 /*!< Value with which fractal values will be multiplied (to make the values fit the gradient better).*/
	FLOAT offset;
 /*!< Offset for mapping value to gradient.*/
	Gradient gradient;
 /*!< Gradient for mapping float values to colors.*/
} RenderingParameters;

/**
 * \fn void ReadRenderingFile(RenderingParameters *param, char *fileName)
 * \brief Read and parse fractal rendering parameters from file.
 *
 * \param param Pointer to the structure to store rendering parameters.
 * \param fileName Name of file containing rendering parameters.
 */
void ReadRenderingFile(RenderingParameters *param, char *fileName);

/**
 * \fn void FreeRenderingParameters(RenderingParameters *param)
 * \brief Free a RenderingParameters structure.
 *
 * \param param Pointer to the structure to free.
 */
void FreeRenderingParameters(RenderingParameters *param);

#endif
