/*
 *  fractal2D.c -- part of fractal2D
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

#include "fractal2D.h"
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
	printf("\nDraw some fractal subset :\n\
Usage : fractal2D [-h] \n  \
or \n\
fractal2D [-q|-v] [-d] [-j <NbThreads>] -c <FractalFile> -r \
<RenderingFile> -o <OutPut> [-f <FloatTable>] [-x <Width>|-y \
<Height>] [-s <OSFactor>|-b <BlurRadius>] [-i <QuadSize>] \
[-t <Threshold>]\n\n  \
-h : Prints this help.\n  \
-q : Quiet mode.\n  \
-v : Verbose mode.\n  \
-d : Debug mode.\n  \
-j <NbThreads> : Specify the number of threads to compute image \
(%u by default).\n  \
-c <FractalFile> : Specify the fractal config file, used for \
computing fractal. See fractal file syntax below.\n  \
-r <RenderingFile> : Specify the rendering file, used for \
rendering fractal. See rendering file syntax below.\n  \
-o <Output> : Specify the output image file (raw PPM).\n  \
-f <FloatTable> : Specify an output file to export fractal as \
a table of floats (before rendering, and in particular before \
any anti-aliasing method is applied). Note that in the case when \
anti-aliasing method is oversampling, the table of floats will be \
that of the high resolution fractal (size multiplied by \
OSFactor).\n  \
-x <Width> : Specify output image width. \n  \
-y <Height> : Specify output image height. \n  \
-s <OSFactor> : Specify factor for oversampling (excellent \
but costly antialiasing method). For a very high quality image, \
5 is a good value.\n  \
-b <BlurRadius> : Specify radius for gaussian blur (fast \
antialiasing method). Values in [2.5, 4] are generally \
sufficient.\n  \
-i <QuadSize> : Maximum size of quadrilateral for linear  \
interpolation (%u by default, which is good for no visible \
loss of quality). A value of 1 means no interpolation(all \
pixels are computed).\n\
Note that when oversampling factor is specified, this is \
used to compute the high resolution image, before downscaling.\n  \
-t <Threshold> : Dissimilarity threshold for quad \
interpolation ("PRINTF_FLOAT_TYPE_SPECIFIER" by default, which is \
good for no visible loss of quality). A quadrilateral that shows \
too dissimilar values at its corners will be computed (as opposed \
to interpolated). Value must be greater than 0. Note that when \
oversampling factor is specified, this is used to compute \
the high resolution image, before downscaling.\n\n  \
Fractal file syntax :\n<FRACTAL_TYPE> | [CX | CY] | \
<CENTER_X> | <CENTER_Y> | <SPAN_X> | <SPAN_Y> | \
<ESCAPE_RADIUS> | <NB_ITER_MAX> | <COLOR_SCALING> | \
<SPACE_COLOR(hexa)> | <NB_TRANSITIONS> | <Color1(hexa)> | .. \
| <ColorN(hexa)>\n\
where FRACTAL_TYPE is either MANDELBROT or JULIA, CX and CY \
are optional floats needed only for Julia (c parameter), and \
'|' designates separators (tabs, blank spaces).\n  \
Rendering file syntax :\n<BYTES_PER_COMPONENT> | \
<SPACE_COLOR(hexa)> | <COLOR_SCALING> | <NB_TRANSITIONS> | \
<Color1(hexa)> | .. | <ColorN(hexa)>\n\
where BYTES_PER_COMPONENT is either 1 (RGB8) or 2 (RGB16).\n",
	(unsigned)DEFAULT_NB_THREADS,
	(unsigned)DEFAULT_QUAD_INTERPOLATION_SIZE,
	FLOAT_DIG, DEFAULT_QUAD_DISSIMILARITY_THRESHOLD);
}

void ParseCommandLineArguments(CommandLineArguments *dst, int argc, char *argv[])
{
	traceLevel = T_NORMAL;
	debug = 0;
	int help = 0;

	dst->quadInterpolationSize = DEFAULT_QUAD_INTERPOLATION_SIZE;
	dst->quadDissimilarityThreshold = DEFAULT_QUAD_DISSIMILARITY_THRESHOLD;

	long long int tmp;
	dst->fractalFileName = NULL;
	dst->renderingFileName = NULL;
	dst->dstFileName = NULL;
	dst->floatTableFileName = NULL;
	dst->antiAliasingMethod = AAM_NONE;
	dst->overSamplingFactor = 0;
	dst->gaussianBlurRadius = 0;

	int widthSpecified = 0, heightSpecified = 0;
	dst->width = 0;
	dst->height = 0;
	int o;
	while ((o = getopt(argc, argv, "hqvdc:r:i:j:o:f:s:b:t:x:y:")) != -1) {
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
			dst->fractalFileName = optarg;
			break;
		case 'r':
			dst->renderingFileName = optarg;
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
			sscanf(optarg, SCANF_FLOAT_TYPE_SPECIFIER, &dst->overSamplingFactor);
			if (dst->overSamplingFactor <= 1.) {
				invalid_use_error("Oversampling factor must be > 1.\n");
			} else {
				dst->antiAliasingMethod = AAM_OVERSAMPLING;
			}
			break;
		case 't':
			sscanf(optarg, SCANF_FLOAT_TYPE_SPECIFIER, &dst->quadDissimilarityThreshold);
			if (dst->quadDissimilarityThreshold < 0.) {
				invalid_use_error("Quad dissimilarity threshold must be between >= 0.\n");
			} 
			break;
		case 'x':
			sscanf(optarg, "%lld", &tmp);
			if (tmp < 2) {
				invalid_use_error("Output image width must be >= 2.\n");
			} else {
				dst->width = (uint32_t)tmp;
				widthSpecified = 1;
			}
			break;
		case 'y':
			sscanf(optarg, "%lld", &tmp);
			if (tmp < 2) {
				invalid_use_error("Output image height must be >= 2.\n");
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
	if (argv[optind] != NULL) {
		invalid_use_error("Remaining argument on command line : %s.\n", argv[optind]);
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

	if (access(dst->fractalFileName, F_OK) != 0) {
		existence_error(dst->fractalFileName);
	}

	if (access(dst->renderingFileName, F_OK) != 0) {
		existence_error(dst->renderingFileName);
	}
}

int main(int argc, char *argv[]) {
	CommandLineArguments arg;
	ParseCommandLineArguments(&arg, argc, argv);

	Fractal fractal;
	ReadFractalFile(&fractal, arg.fractalFileName);
	RenderingParameters render;
	ReadRenderingFile(&render, arg.renderingFileName);

	uint32_t width = arg.width;
	uint32_t height = arg.height;
	FLOAT spanX = fractal.x2 - fractal.x1;
	FLOAT spanY = fractal.y2 - fractal.y1;
	if (width == 0) {
		width = roundF(spanX * height / spanY);
	} else if (height == 0) {
		height = roundF(spanY * width / spanX);
	}

	Image fractalImg, tmpImg;
	CreateUnitializedImage(&fractalImg, width, height, render.bytesPerComponent);
	FloatTable tmpTable;

	switch (arg.antiAliasingMethod) {
	case AAM_NONE:
		CreateUnitializedFloatTable(&tmpTable, width, height);

		ComputeFractalFast(&tmpTable, &fractal, arg.quadInterpolationSize, arg.quadDissimilarityThreshold);
		RenderFractal(&fractalImg, &tmpTable, &render);
		break;
	case AAM_GAUSSIANBLUR:
		CreateUnitializedImage(&tmpImg, width, height, render.bytesPerComponent);
		CreateUnitializedFloatTable(&tmpTable, width, height);
		
		ComputeFractalFast(&tmpTable, &fractal, arg.quadInterpolationSize, arg.quadDissimilarityThreshold);
		RenderFractal(&tmpImg, &tmpTable, &render);
		ApplyGaussianBlur(&fractalImg, &tmpImg, arg.gaussianBlurRadius);

		FreeImage(&tmpImg);
		break;
	case AAM_OVERSAMPLING:
		CreateUnitializedImage(&tmpImg, width*arg.overSamplingFactor, height*arg.overSamplingFactor,
					render.bytesPerComponent);
		CreateUnitializedFloatTable(&tmpTable, width*arg.overSamplingFactor, height*arg.overSamplingFactor);

		ComputeFractalFast(&tmpTable, &fractal, arg.quadInterpolationSize, arg.quadDissimilarityThreshold);
		RenderFractal(&tmpImg, &tmpTable, &render);
		DownscaleImage(&fractalImg, &tmpImg);

		FreeImage(&tmpImg);
		break;
	default:
		error("Unknown anti-aliasing method.\n");
		break;
	}

	if (arg.floatTableFileName) {
		ExportFloatTable(arg.floatTableFileName, &tmpTable);
	}

	ExportPPM(arg.dstFileName, &fractalImg);

	FreeRenderingParameters(&render);
	FreeImage(&fractalImg);
	FreeFloatTable(&tmpTable);

	info(T_NORMAL,"All done.\n");

	exit(EXIT_SUCCESS);
}

