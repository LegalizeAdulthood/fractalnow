/*
 *  command_line.cpp -- part of fractal2D
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

#include "command_line.h"
#include "error.h"
#include "floating_point.h"
#include "help.h"
#include "thread.h"
#include <getopt.h>
#include <inttypes.h>
#include <QApplication>

int FileExists(const char *fileName)
{
	FILE *file;
	int res = 0;

	if ((file = fopen(fileName,"r")) != NULL) {
		res = 1;
		fclose(file);
	}
	return res;
}

CommandLineArguments::CommandLineArguments(int argc, char *argv[])
{
	fractal2D_traceLevel = T_QUIET;
	fractal2D_debug = 0;
	int help = 0;

	int64_t tmp;
	fractalFileName = NULL;
	renderingFileName = NULL;
	int minAntiAliasingSizeSpecified = 0;
	int maxAntiAliasingSizeSpecified = 0;
	int antiAliasingSizeIterationSpecified = 0;
	nbThreads = -1;

	width = 0;
	height = 0;
	int o;
	while ((o = getopt(argc, argv, "hvda:f:i:j:m:nM:r:x:y:")) != -1) {
		switch (o) {
		case 'h':
			help = 1;
			break;
		case 'v':
			fractal2D_traceLevel = T_VERBOSE;
			break;
		case 'd':
#ifndef DEBUG
				fractal2D_message(stdout, T_QUIET, "Debug unavailable: %s was not built in \
debug mode.\n", QApplication::applicationName().toStdString().c_str());
#else
			fractal2D_debug = 1;
#endif
			break;
		case 'f':
			fractalFileName = optarg;
			break;
		case 'r':
			renderingFileName = optarg;
			break;
		case 'j':
			if (sscanf(optarg, "%d", &nbThreads) < 1) {
				invalid_use_error("Command-line argument \'%s\' is not a number.\n    ", optarg);
			}

			if (nbThreads <= 0) {
				invalid_use_error("Number of threads must be positive.\n");
			}
			break;
		case 'm':
			if (sscanf(optarg, "%"SCNd64, &tmp) < 1) {
				invalid_use_error("Command-line argument \'%s\' is not a number.\n    ", optarg);
			}
			if (tmp < 2) {
				invalid_use_error("Minimum anti-aliasing size must be >= 2.\n");
			}
			minAntiAliasingSize = (uint_fast32_t)tmp;
			minAntiAliasingSizeSpecified = 1;
			break;
		case 'i':
			if (sscanf(optarg, "%"SCNd64, &tmp) < 1) {
				invalid_use_error("Command-line argument \'%s\' is not a number.\n    ", optarg);
			}
			if (tmp < 1) {
				invalid_use_error("Anti-aliasing size iteration must be >= 1.\n");
			}
			antiAliasingSizeIteration = (uint_fast32_t)tmp;
			antiAliasingSizeIterationSpecified = 1;
			break;
		case 'M':
			if (sscanf(optarg, "%"SCNd64, &tmp) < 1) {
				invalid_use_error("Command-line argument \'%s\' is not a number.\n    ", optarg);
			}
			if (tmp < 2) {
				invalid_use_error("Maximum anti-aliasing size must be >= 2.\n");
			}
			maxAntiAliasingSize = (uint_fast32_t)tmp;
			maxAntiAliasingSizeSpecified = 1;
			break;
		case 'x':
			if (sscanf(optarg, "%"SCNd64, &tmp) < 1) {
				invalid_use_error("Command-line argument \'%s\' is not a number.\n    ", optarg);
			}
			if (tmp < 2) {
				invalid_use_error("Output image width must be >= 2.\n");
			} else {
				width = (uint_fast32_t)tmp;
			}
			break;
		case 'y':
			if (sscanf(optarg, "%"SCNd64, &tmp) < 1) {
				invalid_use_error("Command-line argument \'%s\' is not a number.\n    ", optarg);
			}
			if (tmp < 2) {
				invalid_use_error("Output image height must be >= 2.\n");
			} else {
				height = (uint_fast32_t)tmp;
			}
			break;
		default:
			Help::Print();
			exit(EXIT_FAILURE);
			break;
		}
	}
	if (argv[optind] != NULL) {
		invalid_use_error("Remaining argument on command line : '%s'.\n", argv[optind]);
	}

	if (help) {
		Help::Print();
		exit(EXIT_SUCCESS);
	}

	if (!antiAliasingSizeIterationSpecified) {
		antiAliasingSizeIteration = DEFAULT_ANTIALIASING_SIZE_ITERATION;
	}
		
	if (!minAntiAliasingSizeSpecified) {
		minAntiAliasingSize = DEFAULT_MIN_ANTIALIASING_SIZE;
	}

	if (!maxAntiAliasingSizeSpecified) {
		maxAntiAliasingSize = DEFAULT_MAX_ANTIALIASING_SIZE;
	}
	if (minAntiAliasingSize > maxAntiAliasingSize) {
		invalid_use_error("Maximum anti-aliasing size must be greater than minimum anti-aliasing size.\n");
	}
	
	if (fractalFileName != NULL && !FileExists(fractalFileName)) {
		fractal2D_existence_error(fractalFileName);
	}

	if (renderingFileName != NULL && !FileExists(renderingFileName)) {
		fractal2D_existence_error(renderingFileName);
	}
}
