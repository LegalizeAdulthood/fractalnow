/*
 *  command_line.c -- part of FractalNow
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
#include "help.h"
#include "fractalnow.h"
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
	FractalNow_traceLevel = T_NORMAL;
	FractalNow_debug = 0;
	int help = 0;

	dst->quadInterpolationSize = DEFAULT_QUAD_INTERPOLATION_SIZE;
	dst->colorDissimilarityThreshold = DEFAULT_COLOR_DISSIMILARITY_THRESHOLD;

	int64_t tmp;
	dst->fractalConfigFileName = NULL;
	dst->fractalFileName = NULL;
	dst->renderingFileName = NULL;
	dst->gradientFileName = NULL;
	dst->dstFileName = NULL;
	dst->antiAliasingMethod = AAM_NONE;
	dst->antiAliasingSize = -1;
	dst->adaptiveAAMThreshold = -1;
	dst->nbThreads = -1;
	dst->floatPrecision = FP_DOUBLE;
	dst->MPFloatPrecision = fractalnow_mpfr_precision;

	int widthSpecified = 0, heightSpecified = 0;
	dst->width = 0;
	dst->height = 0;
	int o;
	while ((o = getopt(argc, argv, "hqvda:c:f:g:i:j:l:L:o:p:r:s:t:x:y:")) != -1) {
		switch (o) {
		case 'h':
			help = 1;
			break;
		case 'q':
			if (FractalNow_traceLevel == T_VERBOSE) {
				invalid_use_error("-q and -v are mutually exclusive.\n");
			}
			FractalNow_traceLevel = T_QUIET;
			break;
		case 'v':
			if (FractalNow_traceLevel == T_QUIET) {
				invalid_use_error("-q and -v are mutually exclusive.\n");
			}
			FractalNow_traceLevel = T_VERBOSE;
			break;
		case 'd':
#ifndef DEBUG
				FractalNow_message(stdout, T_QUIET, "Debug unavailable: FractalNow was not built in \
debug mode.\n");
#else
			FractalNow_debug = 1;
#endif
			break;
		case 'a':
			dst->antiAliasingMethod = GetAAM(optarg);
			break;
		case 'c':
			dst->fractalConfigFileName = optarg;
			break;
		case 'f':
			dst->fractalFileName = optarg;
			break;
		case 'r':
			dst->renderingFileName = optarg;
			break;
		case 'g':
			dst->gradientFileName = optarg;
			break;
		case 'l':
			if (GetFloatPrecision(&dst->floatPrecision, optarg)) {
				invalid_use_error("\n");
			}
			break;
		case 'L':
			if (sscanf(optarg, "%"SCNd64, &tmp) < 1) {
				invalid_use_error("Command-line argument \'%s\' is not a number.\n", optarg);
			}
			if (tmp < MPFR_PREC_MIN || tmp > MPFR_PREC_MAX) {
				invalid_use_error("MP floats precision must be between %ld and %ld.\n",
					(long int)MPFR_PREC_MIN, (long int)MPFR_PREC_MAX);
			} else {
				dst->MPFloatPrecision = (mpfr_prec_t)tmp;
			}
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
			if (sscanf(optarg, "%d", &dst->nbThreads) < 1) {
				invalid_use_error("Command-line argument \'%s\' is not a number.\n", optarg);
			}
			if (dst->nbThreads <= 0) {
				invalid_use_error("Number of threads must be positive.\n");
			}
			break;
		case 'o':
			dst->dstFileName = optarg;
			break;
		case 'p':
			if (sscanf(optarg, "%lf", &dst->adaptiveAAMThreshold) < 1) {
				invalid_use_error("Command-line argument \'%s\' is not a floating-point number.\n", optarg);
			}
			if (dst->adaptiveAAMThreshold < 0.) {
				invalid_use_error("Adaptive anti-aliasing threshold must be >= 0.\n");
			} 
			break;
		case 's':
			if (sscanf(optarg, "%lf", &dst->antiAliasingSize) < 1) {
				invalid_use_error("Command-line argument \'%s\' is not a floating-point number.\n", optarg);
			}
			break;
		case 't':
			if (sscanf(optarg, "%lf", &dst->colorDissimilarityThreshold) < 1) {
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

	if (dst->fractalConfigFileName == NULL) {
		if (dst->fractalFileName == NULL) {
			invalid_use_error("No configuration or fractal file specified.\n");
		}

		if (dst->renderingFileName == NULL) {
			invalid_use_error("No configuration or rendering file specified.\n");
		}
	}

	if (dst->dstFileName == NULL) {
		invalid_use_error("No output file specified.\n");
	}

	if (!widthSpecified && !heightSpecified) {
		invalid_use_error("At least width or height must be specified.\n");
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

		if (!complexIsInteger(dst->antiAliasingSize)) {
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
	
	if (dst->fractalConfigFileName != NULL && !FileExists(dst->fractalConfigFileName)) {
		FractalNow_existence_error(dst->fractalConfigFileName);
	}

	if (dst->fractalFileName != NULL && !FileExists(dst->fractalFileName)) {
		FractalNow_existence_error(dst->fractalFileName);
	}

	if (dst->renderingFileName != NULL && !FileExists(dst->renderingFileName)) {
		FractalNow_existence_error(dst->renderingFileName);
	}

	if (dst->gradientFileName != NULL && !FileExists(dst->gradientFileName)) {
		FractalNow_existence_error(dst->gradientFileName);
	}
}

