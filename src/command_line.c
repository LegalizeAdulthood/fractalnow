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
#include "help.h"
#include "fractal.h"
#include "thread.h"
#include <getopt.h>

void ParseCommandLineArguments(CommandLineArguments *dst, int argc, char *argv[])
{
	traceLevel = T_NORMAL;
	debug = 0;
	int help = 0;

	dst->quadInterpolationSize = DEFAULT_QUAD_INTERPOLATION_SIZE;
	dst->colorDissimilarityThreshold = DEFAULT_COLOR_DISSIMILARITY_THRESHOLD;

	long long int tmp;
	dst->configFileName = NULL;
	dst->dstFileName = NULL;
	dst->antiAliasingMethod = AAM_NONE;
	dst->overSamplingFactor = 0;
	dst->gaussianBlurRadius = 0;

	int widthSpecified = 0, heightSpecified = 0;
	dst->width = 0;
	dst->height = 0;
	int o;
	while ((o = getopt(argc, argv, "hqvdc:i:j:o:s:b:t:x:y:")) != -1) {
		switch (o) {
		case 'h':
			help = 1;
			break;
		case 'q':
			if (traceLevel == T_VERBOSE) {
				invalid_use_error("-q and -v are excluse.\n");
			}
			traceLevel = T_QUIET;
			break;
		case 'v':
			if (traceLevel == T_QUIET) {
				invalid_use_error("-q and -v are excluse.\n");
			}
			traceLevel = T_VERBOSE;
			break;
		case 'd':
			debug = 1;
			break;
		case 'c':
			dst->configFileName = optarg;
			break;
		case 'i':
			sscanf(optarg, "%lld", &tmp);
			if (tmp <= 0) {
				invalid_use_error("Quad interpolation size must be positive.\n");
			} else {
				dst->quadInterpolationSize = (uint32_t)tmp;
			}
			break;
		case 'j':
			sscanf(optarg, "%lld", &tmp);
			if (tmp <= 0) {
				invalid_use_error("Number of threads must be positive.\n");
			} else {
				nbThreads = (uint32_t)tmp;
			}
			break;
		case 'o':
			dst->dstFileName = optarg;
			break;
		case 'b':
			if (dst->antiAliasingMethod != AAM_NONE) {
				invalid_use_error("Should not combine anti-aliasing methods (blur and oversampling).\n");
			}
			sscanf(optarg, "%lf", &dst->gaussianBlurRadius);
			if (dst->gaussianBlurRadius <= 0) {
				invalid_use_error("Gaussian blur radius must be > 0.\n");
			} else {
				dst->antiAliasingMethod = AAM_GAUSSIANBLUR;
			}
			break;
		case 's':
			if (dst->antiAliasingMethod != AAM_NONE) {
				invalid_use_error("Should not combine anti-aliasing methods (blur and oversampling).\n");
			}
			sscanf(optarg, "%lf", &dst->overSamplingFactor);
			if (dst->overSamplingFactor <= 1.) {
				invalid_use_error("Oversampling factor must be > 1.\n");
			} else {
				dst->antiAliasingMethod = AAM_OVERSAMPLING;
			}
			break;
		case 't':
			sscanf(optarg, "%lf", &dst->colorDissimilarityThreshold);
			if (dst->colorDissimilarityThreshold < 0. || dst->colorDissimilarityThreshold > 1.) {
				invalid_use_error("Color dissimilarity threshold must be between 0 and 1.\n");
			} 
			break;
		case 'x':
			sscanf(optarg, "%lld", &tmp);
			if (tmp <= 0) {
				invalid_use_error("Output image width must be > 0.\n");
			} else {
				dst->width = (uint32_t)tmp;
				widthSpecified = 1;
			}
			break;
		case 'y':
			sscanf(optarg, "%lld", &tmp);
			if (tmp <= 0) {
				invalid_use_error("Output image height output must be > 0.\n");
			} else {
				dst->height = (uint32_t)tmp;
				heightSpecified = 1;
			}
			break;
		default:
			DisplayHelp();
			exit(EXIT_FAILURE);
			break;
		}
	}

	if (help) {
		DisplayHelp();
		exit(EXIT_SUCCESS);
	}

	if (dst->configFileName == NULL) {
		invalid_use_error("No config file specified.\n");
	}

	if (dst->dstFileName == NULL) {
		invalid_use_error("No output file specified.\n");
	}

	if (!widthSpecified && !heightSpecified) {
		invalid_use_error("At least output image width or height must be specified.\n");
	}
}
