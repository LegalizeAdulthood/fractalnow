/*
 *  command_line.h -- part of fractal2D
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
  * \file command_line.h
  * \brief Header file related to command line parsing.
  * \author Marc Pegon
  */

#ifndef __COMMAND_LINE_H__
#define __COMMAND_LINE_H__

#ifdef __cplusplus
extern "C" {
#endif

#include "anti_aliasing.h"
#include "floating_point.h"
#include <stdint.h>

/**
 * \struct s_CommandLineArguments
 * \brief For storing command lines arguments.
 *
 * This structure is to store the arguments given on the command line.
 */
typedef struct s_CommandLineArguments {
	char *fractalFileName;
 /*!< Fractal file name.*/
	char *renderingFileName;
 /*!< Rendering file name.*/
	char *dstFileName;
 /*!< Output image file name.*/
	uint_fast32_t width;
 /*!< Width of output float table/image.*/
	uint_fast32_t height;
 /*!< Height of output float table/image.*/
	uint_fast32_t quadInterpolationSize;
 /*!< Maximum size of quadrilaterals for interpolation.*/
	int nbThreads;
 /*!< Number of threads. Negative if not specified. */
	FLOAT colorDissimilarityThreshold;
 /*!< Dissimilarity threshold for interpolation.*/
	AntiAliasingMethod antiAliasingMethod;
 /*!< Anti-aliasing method.*/
	FLOAT antiAliasingSize;
 /*!< Size for anti-aliasing (radius for blur, factor for oversampling and adaptive).*/
	FLOAT adaptiveAAMThreshold;
 /*!< Threshold used when anti-aliasing method is adaptive.*/
} CommandLineArguments;

/**
 * \fn void ParseCommandLineArguments(CommandLineArguments *dst, int argc, char *argv[])
 * \brief Parse arguments given on command line.
 *
 * \param dst Pointer to the (already allocated) structure in which to store the arguments.
 * \param argc Argc of the main function
 * \param argv Argv of the main function
 */
void ParseCommandLineArguments(CommandLineArguments *dst, int argc, char *argv[]);

#ifdef __cplusplus
}
#endif

#endif
