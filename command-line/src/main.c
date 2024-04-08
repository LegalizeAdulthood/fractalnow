/*
 *  main.c -- part of FractalNow
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

#include "anti_aliasing.h"
#include "command_line.h"
#include "fractal.h"
#include "fractal_config.h"
#include "image.h"
#include "locale.h"
#include "ppm.h"
#include <getopt.h>
#include <inttypes.h>
#include <stdint.h>
#include <stdlib.h>

int main(int argc, char *argv[]) {
	setlocale(LC_NUMERIC, "C");
	CommandLineArguments arg;
	ParseCommandLineArguments(&arg, argc, argv);

	FractalConfig fractalConfig;
	if (arg.fractalConfigFileName != NULL) {
		if (ReadFractalConfigFile(&fractalConfig, arg.fractalConfigFileName)) {
			FractalNow_error("Failed to read config file.\n");
		}
	}
	if (arg.fractalFileName != NULL) {
		Fractal fractal;
		if (ReadFractalFile(&fractal, arg.fractalFileName)) {
			FractalNow_error("Failed to read fractal file.\n");
		}
		if (arg.fractalConfigFileName == NULL) {
			fractalConfig.fractal = fractal;
		} else {
			ResetFractal(&fractalConfig, fractal);
		}
	}
	if (arg.renderingFileName != NULL) {
		RenderingParameters render;
		if (ReadRenderingFile(&render, arg.renderingFileName)) {
			FractalNow_error("Failed to read rendering file.\n");
		}
		if (arg.fractalConfigFileName == NULL) {
			fractalConfig.render = render;
		} else {
			ResetRenderingParameters(&fractalConfig, render);
		}
	}
	if (arg.gradientFileName != NULL) {
		Gradient gradient;
		if (ReadGradientFile(&gradient, arg.gradientFileName)) {
			FractalNow_error("Failed to read gradient file.\n");
		}
		ResetGradient(&fractalConfig.render, gradient);
	}

	Fractal fractal = fractalConfig.fractal;
	RenderingParameters render = fractalConfig.render;
	uint_fast32_t width = arg.width;
	uint_fast32_t height = arg.height;
	FLOATT spanX = fractal.spanX;
	FLOATT spanY = fractal.spanY;
	if (width == 0) {
		width = roundF(spanX * height / spanY);
	} else if (height == 0) {
		height = roundF(spanY * width / spanX);
	}

	Threads *threads = CreateThreads((arg.nbThreads <= 0) ? DEFAULT_NB_THREADS :
						(uint_fast32_t)arg.nbThreads);

	Image fractalImg, tmpImg;
	CreateImage(&fractalImg, width, height, render.bytesPerComponent);

	switch (arg.antiAliasingMethod) {
	case AAM_NONE:
		DrawFractal(&fractalImg, &fractal, &render, arg.quadInterpolationSize,
				arg.colorDissimilarityThreshold, NULL, threads);
		break;
	case AAM_GAUSSIANBLUR:
		CreateImage(&tmpImg, width, height, render.bytesPerComponent);
		
		DrawFractal(&tmpImg, &fractal, &render, arg.quadInterpolationSize,
				arg.colorDissimilarityThreshold, NULL, threads);
		ApplyGaussianBlur(&fractalImg, &tmpImg, arg.antiAliasingSize, threads);

		FreeImage(tmpImg);
		break;
	case AAM_OVERSAMPLING:
		CreateImage(&tmpImg, width*arg.antiAliasingSize, height*arg.antiAliasingSize,
				render.bytesPerComponent);

		DrawFractal(&tmpImg, &fractal, &render, arg.quadInterpolationSize,
				arg.colorDissimilarityThreshold, NULL, threads);
		DownscaleImage(&fractalImg, &tmpImg, threads);

		FreeImage(tmpImg);
		break;
	case AAM_ADAPTIVE:
		DrawFractal(&fractalImg, &fractal, &render, arg.quadInterpolationSize,
				arg.colorDissimilarityThreshold, NULL, threads);
		AntiAliaseFractal(&fractalImg, &fractal, &render, arg.antiAliasingSize,
					arg.adaptiveAAMThreshold, NULL, threads);
		break;
	default:
		FractalNow_error("Unknown anti-aliasing method.\n");
		break;
	}

	if (ExportPPM(arg.dstFileName, &fractalImg)) {
		FractalNow_error("Failed to export image as PPM.\n");
	}

	FreeFractalConfig(fractalConfig);
	FreeImage(fractalImg);
	DestroyThreads(threads);

	FractalNow_message(stdout, T_NORMAL, "All done.\n");

	exit(EXIT_SUCCESS);
}

