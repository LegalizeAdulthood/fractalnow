/*
 *  command_line.h -- part of fractal2D
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
  * \file command_line.h
  * \brief Header file related to command line parsing.
  * \author Marc Pegon
  */

#ifndef __COMMAND_LINE_H__
#define __COMMAND_LINE_H__

#include "main.h"
#include <stdint.h>

 //! For storing command lines arguments.
 /* This structure is to store the arguments given on the command line.
 */
class CommandLineArguments {
	public:
	 //!brief Parse arguments given on command line.
	 /*
	 * \param dst Pointer to the (already allocated) structure in which to store the arguments.
	 * \param argc Argc of the main function
	 * \param argv Argv of the main function
	 */
	CommandLineArguments(int argc, char *argv[]);

	//! Fractal file name.
	char *fractalFileName;
	//! Rendering file name.
	char *renderingFileName;
	//! Width of output float table/image.
	uint_fast32_t width;
	//! Height of output float table/image.
	uint_fast32_t height;
	//! Minimum size for anti-aliasing.
	uint_fast32_t minAntiAliasingSize;
	//! Maximum size for anti-aliasing.
	uint_fast32_t maxAntiAliasingSize;
	//! Anti-aliasing size iteration.
	uint_fast32_t antiAliasingSizeIteration;
	//! Force number of threads used for drawing fractals.
	int nbThreads;
};

#endif
