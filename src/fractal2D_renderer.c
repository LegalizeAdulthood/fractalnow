/*
 *  fractal2D_renderer.c -- part of fractal2D
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

#include "fractal2D_renderer.h"
#include "error.h"
#include "float_table.h"
#include "fractal.h"
#include "gradient.h"
#include "image.h"
#include "ppm.h"
#include <getopt.h>
#include <stdint.h>
#include <unistd.h>

void DisplayHelp()
{
	printf("\nRender some fractal float table (generated by \
fractal2D-renderer:\n\
Usage : fractal2D-renderer [-h] \n  \
or \n\
fractal2D-renderer [-q|-v] [-d] [-j <NbThreads>] -f <FloatTable> \
-r <RenderingFile> -o <OutPut> [-s <DownscalingFactor>|-b \
<BlurRadius>]\n\n  \
-h : Prints this help.\n  \
-q : Quiet mode.\n  \
-v : Verbose mode.\n  \
-d : Debug mode.\n  \
-j <NbThreads> : Specify the number of threads to compute image \
(%u by default).\n  \
-f <FloatTable> : Specify the input fractal float table to \
render.\n  \
-r <RenderingFile> : Specify the rendering file, used for \
rendering fractal. See rendering file syntax below.\n  \
-o <Output> : Specify the output image file (raw PPM). \n  \
-s <DownscalingFactor> : Specify downscaling factor. Note that \
in the case when a downscaling factor is specified, the output \
image size will be divided by this it (upscaling when computing \
fractal and downscaling when rendering can be used to generate \
an anti-aliased image (oversampling method)).\n  \
-b <BlurRadius> : Specify radius for gaussian blur (fast \
antialiasing method). Values in [2.5, 4] are generally \
sufficient.\n\n  \
Rendering file syntax :\n<BYTES_PER_COMPONENT> | \
<SPACE_COLOR(hexa)> | <COLOR_SCALING> | <NB_TRANSITIONS> | \
<Color1(hexa)> | .. | <ColorN(hexa)>\n\
where BYTES_PER_COMPONENT is either 1 (RGB8) or 2 (RGB16) and \
'|' designates separators (tabs, blank spaces).\n",
	(unsigned)DEFAULT_NB_THREADS);
}

void ParseCommandLineArguments(CommandLineArguments *dst, int argc, char *argv[])
{
	traceLevel = T_NORMAL;
	debug = 0;
	int help = 0;

	long long int tmp;
	dst->floatTableFileName = NULL;
	dst->renderingFileName = NULL;
	dst->dstFileName = NULL;
	dst->antiAliasingMethod = AAM_NONE;
	dst->downscalingFactor = 1.;
	dst->gaussianBlurRadius = 0;

	int o;
	while ((o = getopt(argc, argv, "hqvdr:j:o:f:s:b:")) != -1) {
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
		case 'r':
			dst->renderingFileName = optarg;
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
		case 'f':
			dst->floatTableFileName = optarg;
			break;
		case 'b':
			if (dst->antiAliasingMethod != AAM_NONE) {
				invalid_use_error("Should not combine anti-aliasing methods (blur and oversampling).\n");
			}
			sscanf(optarg, SCANF_FLOAT_TYPE_SPECIFIER, &dst->gaussianBlurRadius);
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
			sscanf(optarg, SCANF_FLOAT_TYPE_SPECIFIER, &dst->downscalingFactor);
			if (dst->downscalingFactor <= 1.) {
				invalid_use_error("Downscaling factor must be > 1.\n");
			} else {
				dst->antiAliasingMethod = AAM_OVERSAMPLING;
			}
			break;
		default:
			DisplayHelp();
			exit(EXIT_FAILURE);
			break;
		}
	}
	if (argv[optind] != NULL) {
		invalid_use_error("Remaining argument on command line : %s.\n", argv[optind]);
	}

	if (help) {
		DisplayHelp();
		exit(EXIT_SUCCESS);
	}

	if (dst->floatTableFileName == NULL) {
		invalid_use_error("No fractal float table file specified.\n");
	}

	if (dst->renderingFileName == NULL) {
		invalid_use_error("No rendering file specified.\n");
	}

	if (dst->dstFileName == NULL) {
		invalid_use_error("No output file specified.\n");
	}

	if (access(dst->floatTableFileName, F_OK) != 0) {
		existence_error(dst->floatTableFileName);
	}

	if (access(dst->renderingFileName, F_OK) != 0) {
		existence_error(dst->renderingFileName);
	}
}

int main(int argc, char *argv[]) {
	CommandLineArguments arg;
	ParseCommandLineArguments(&arg, argc, argv);

	FloatTable fractalTable;
	ImportFloatTable(&fractalTable, arg.floatTableFileName);
	RenderingParameters render;
	ReadRenderingFile(&render, arg.renderingFileName);

	Image fractalImg, tmpImg;
	CreateUnitializedImage(&fractalImg, fractalTable.width / arg.downscalingFactor,
				fractalTable.height / arg.downscalingFactor, 
				render.bytesPerComponent);

	switch (arg.antiAliasingMethod) {
	case AAM_NONE:
		RenderFractal(&fractalImg, &fractalTable, &render);
		break;
	case AAM_GAUSSIANBLUR:
		CreateUnitializedImage(&tmpImg, fractalTable.width, fractalTable.height, render.bytesPerComponent);
		
		RenderFractal(&tmpImg, &fractalTable, &render);
		ApplyGaussianBlur(&fractalImg, &tmpImg, arg.gaussianBlurRadius);

		FreeImage(&tmpImg);
		break;
	case AAM_OVERSAMPLING:
		CreateUnitializedImage(&tmpImg, fractalTable.width, fractalTable.height, render.bytesPerComponent);

		RenderFractal(&tmpImg, &fractalTable, &render);
		DownscaleImage(&fractalImg, &tmpImg);

		FreeImage(&tmpImg);
		break;
	default:
		error("Unknown anti-aliasing method.\n");
		break;
	}

	ExportPPM(arg.dstFileName, &fractalImg);

	FreeRenderingParameters(&render);
	FreeImage(&fractalImg);
	FreeFloatTable(&fractalTable);

	info(T_NORMAL,"All done.\n");

	exit(EXIT_SUCCESS);
}

