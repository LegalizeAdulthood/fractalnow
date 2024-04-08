/*
 *  command_line.c -- part of fractal2D
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
 
#include "command_line.h"
#include "error.h"
#include "fractal.h"
#include "help.h"
#include "misc.h"
#include "thread.h"
#include <getopt.h>
#include <inttypes.h>
#include <string.h>

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

void ParseCommandLineArguments(CommandLineArguments *dst, int argc, char *argv[])
{
	fractal2D_traceLevel = T_NORMAL;
	fractal2D_debug = 0;
	int help = 0;

	dst->quadInterpolationSize = DEFAULT_QUAD_INTERPOLATION_SIZE;
	dst->colorDissimilarityThreshold = DEFAULT_COLOR_DISSIMILARITY_THRESHOLD;

	int64_t tmp;
	dst->fractalFileName = NULL;
	dst->renderingFileName = NULL;
	dst->dstFileName = NULL;
	dst->antiAliasingMethod = AAM_NONE;
	dst->antiAliasingSize = -1;
	dst->adaptiveAAMThreshold = -1;

	int widthSpecified = 0, heightSpecified = 0;
	dst->width = 0;
	dst->height = 0;
	int o;
	while ((o = getopt(argc, argv, "hqvda:c:i:j:o:p:r:s:t:x:y:")) != -1) {
		switch (o) {
		case 'h':
			help = 1;
			break;
		case 'q':
			if (fractal2D_traceLevel == T_VERBOSE) {
				invalid_use_error("-q and -v are mutually exclusive.\n");
			}
			fractal2D_traceLevel = T_QUIET;
			break;
		case 'v':
			if (fractal2D_traceLevel == T_QUIET) {
				invalid_use_error("-q and -v are mutually exclusive.\n");
			}
			fractal2D_traceLevel = T_VERBOSE;
			break;
		case 'd':
#ifndef DEBUG
				fractal2D_message(stdout, T_QUIET, "Debug unavailable: fractal2D was not built in \
debug mode.\n");
#else
			fractal2D_debug = 1;
#endif
			break;
		case 'a':
			dst->antiAliasingMethod = GetAAM(optarg);
			break;
		case 'c':
			dst->fractalFileName = optarg;
			break;
		case 'r':
			dst->renderingFileName = optarg;
			break;
		case 'i':
			if (sscanf(optarg, "%"SCNd64, &tmp) < 1) {
				invalid_use_error("Command-line argument \'%s\' is not a number.\n", optarg);
			}
			if (tmp <= 0) {
				invalid_use_error("Quad interpolation size must be positive.\n");
			} else {
				dst->quadInterpolationSize = (uint_fast32_t)tmp;
			}
			break;
		case 'j':
			if (sscanf(optarg, "%"SCNd64, &tmp) < 1) {
				invalid_use_error("Command-line argument \'%s\' is not a number.\n", optarg);
			}
			if (tmp <= 0) {
				invalid_use_error("Number of threads must be positive.\n");
			} else {
				nbThreads = (uint_fast32_t)tmp;
			}
			break;
		case 'o':
			dst->dstFileName = optarg;
			break;
		case 'p':
			if (sscanf(optarg, "%"SCNFLOAT, &dst->adaptiveAAMThreshold) < 1) {
				invalid_use_error("Command-line argument \'%s\' is not a floating-point number.\n", optarg);
			}
			if (dst->adaptiveAAMThreshold < 0.) {
				invalid_use_error("Adaptive anti-aliasing threshold must be >= 0.\n");
			} 
			break;
		case 's':
			if (sscanf(optarg, "%"SCNFLOAT, &dst->antiAliasingSize) < 1) {
				invalid_use_error("Command-line argument \'%s\' is not a floating-point number.\n", optarg);
			}
			break;
		case 't':
			if (sscanf(optarg, "%"SCNFLOAT, &dst->colorDissimilarityThreshold) < 1) {
				invalid_use_error("Command-line argument \'%s\' is not a floating-point number.\n", optarg);
			}
			if (dst->colorDissimilarityThreshold < 0.) {
				invalid_use_error("Quad dissimilarity threshold must be between >= 0.\n");
			} 
			break;
		case 'x':
			if (sscanf(optarg, "%"SCNd64, &tmp) < 1) {
				invalid_use_error("Command-line argument \'%s\' is not a number.\n", optarg);
			}
			if (tmp < 2) {
				invalid_use_error("Output image width must be >= 2.\n");
			} else {
				dst->width = (uint_fast32_t)tmp;
				widthSpecified = 1;
			}
			break;
		case 'y':
			if (sscanf(optarg, "%"SCNd64, &tmp) < 1) {
				invalid_use_error("Command-line argument \'%s\' is not a number.\n", optarg);
			}
			if (tmp < 2) {
				invalid_use_error("Output image height must be >= 2.\n");
			} else {
				dst->height = (uint_fast32_t)tmp;
				heightSpecified = 1;
			}
			break;
		default:
			DisplayHelp();
			exit(EXIT_FAILURE);
			break;
		}
	}
	if (argv[optind] != NULL) {
		invalid_use_error("Remaining argument on command line : '%s'.\n", argv[optind]);
	}

	if (help) {
		DisplayHelp();
		exit(EXIT_SUCCESS);
	}

	if (dst->fractalFileName == NULL) {
		invalid_use_error("No fractal file specified.\n");
	}

	if (dst->renderingFileName == NULL) {
		invalid_use_error("No rendering file specified.\n");
	}

	if (dst->dstFileName == NULL) {
		invalid_use_error("No output file specified.\n");
	}

	if (!widthSpecified && !heightSpecified) {
		invalid_use_error("At least output image width or height must be specified.\n");
	}

	switch (dst->antiAliasingMethod) {
	case AAM_NONE:
		if (dst->antiAliasingSize != -1) {
			invalid_use_error("No size parameter ('-s') should be specified when \
anti-aliasing is disabled.\n");
		}
		break;
	case AAM_GAUSSIANBLUR:
		if (dst->antiAliasingSize == -1) {
			invalid_use_error("No size parameter ('-s') specified for blur.\n");
		}
		if (dst->antiAliasingSize <= 0) {
			invalid_use_error("Size parameter ('-s') for gaussian blur must be > 0.\n");
		}
		if (dst->adaptiveAAMThreshold != -1) {
			invalid_use_error("No adaptive anti-aliasing threshold ('-p') should be \
specified when anti-aliasing method is blur.\n");
		}
		break;
	case AAM_OVERSAMPLING:
		if (dst->antiAliasingSize == -1) {
			invalid_use_error("No size parameter ('-s') specified for oversampling.\n");
		}
		if (dst->antiAliasingSize <= 1.) {
			invalid_use_error("Size parameter ('-s') for oversampling must be > 1.\n");
		}
		if (dst->adaptiveAAMThreshold != -1) {
			invalid_use_error("No adaptive anti-aliasing threshold ('-p') should be \
specified when anti-aliasing method is oversampling.\n");
		}
		break;
	case AAM_ADAPTIVE:
		if (dst->antiAliasingSize == -1) {
			invalid_use_error("No size parameter ('-s') specified for adaptive \
anti-aliasing.\n");
		}

		if (!isInteger(dst->antiAliasingSize)) {
			invalid_use_error("Size parameter ('-s') for adaptive anti-aliasing \
should be an integer.\n");
		}
		if (dst->antiAliasingSize <= 1.) {
			invalid_use_error("Size parameter ('-s') for adaptive anti-aliasing \
must be > 1.\n");
		}

		if (dst->adaptiveAAMThreshold == -1) {
			dst->adaptiveAAMThreshold = DEFAULT_ADAPTIVE_AAM_THRESHOLD;
		}

		break;
	default:
		invalid_use_error("Unknown anti-aliasing method.\n");
		break;
	}
	
	if (!FileExists(dst->fractalFileName)) {
		fractal2D_existence_error(dst->fractalFileName);
	}

	if (!FileExists(dst->renderingFileName)) {
		fractal2D_existence_error(dst->renderingFileName);
	}
}

