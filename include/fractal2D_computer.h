/*
 *  fractal2D_computer.h -- part of fractal2D
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
  * \file fractal.h
  * \brief Main header file for fractal2D-computer program.
  * \author Marc Pegon
  */
 
#ifndef __FRACTAL2D_COMPUTER_H__
#define __FRACTAL2D_COMPUTER_H__

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
	char *floatTableFileName;
 /*!< Output file name to export fractal as a table of floats (before rendering).*/
	uint32_t width;
 /*!< Width of output float table/image.*/
	uint32_t height;
 /*!< Height of output float table/image.*/
	uint32_t quadInterpolationSize;
 /*!< Maximum size of quadrilaterals for interpolation.*/
	FLOAT quadDissimilarityThreshold;
 /*!< Dissimilarity threshold for interpolation.*/
	FLOAT upscalingFactor;
 /*!< Upscaling factor (to generate a bigger float table).*/
} CommandLineArguments;

/**
 * \fn void DisplayHelp()
 * \brief Display program help.
 */
void DisplayHelp();

/**
 * \fn void ParseCommandLineArguments(CommandLineArguments *dst, int argc, char *argv[])
 * \brief Parse arguments given on command line.
 *
 * \param dst Pointer to the (already allocated) structure in which to store the arguments.
 * \param argc Argc of the main function
 * \param argv Argv of the main function
 */
void ParseCommandLineArguments(CommandLineArguments *dst, int argc, char *argv[]);

#endif
