/*
 *  main.c -- part of fractal2D
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
#include "config.h"
#include "error.h"
#include "fractal.h"
#include "gradient.h"
#include "image.h"
#include "ppm.h"

#include <stdint.h>

int main(int argc, char *argv[]) {
	CommandLineArguments arg;
	ParseCommandLineArguments(&arg, argc, argv);

	Config config;
	ReadConfigFile(&config, arg.configFileName);

	uint32_t width = arg.width;
	uint32_t height = arg.height;
	if (width == 0) {
		width = config.spanX * height / config.spanY;
	} else if (height == 0) {
		height = config.spanY * width / config.spanX;
	}

	Gradient gradient;
	GenerateGradient(&gradient, config.color, config.nbColors, config.nbTransitions);

	Fractal fractal;
	InitFractal2(&fractal, config.centerX, config.centerY, config.spanX, config.spanY,
			config.escapeRadius, config.nbIterMax);

	Image fractalImg, tmpImg;
	CreateUnitializedImage(&fractalImg, width, height);

	switch (arg.antiAliasingMethod) {
	case AAM_NONE:
		DrawFractalFast(&fractalImg, &fractal, &gradient, config.spaceColor, config.multiplier,
				arg.quadInterpolationSize, arg.colorDissimilarityThreshold);
		break;
	case AAM_GAUSSIANBLUR:
		CreateUnitializedImage(&tmpImg, width, height);
		DrawFractalFast(&tmpImg, &fractal, &gradient, config.spaceColor, config.multiplier,
				arg.quadInterpolationSize, arg.colorDissimilarityThreshold);
		ApplyGaussianBlur(&fractalImg, &tmpImg, arg.gaussianBlurRadius);
		FreeImage(&tmpImg);
		break;
	case AAM_OVERSAMPLING:
		CreateUnitializedImage(&tmpImg, width*arg.overSamplingFactor, height*arg.overSamplingFactor);
		DrawFractalFast(&tmpImg, &fractal, &gradient, config.spaceColor, config.multiplier,
				arg.quadInterpolationSize, arg.colorDissimilarityThreshold);
		DownscaleImage(&fractalImg, &tmpImg);
		FreeImage(&tmpImg);
		break;
	default:
		error("Unknown anti-aliasing method.\n");
		break;
	}

	ExportPPM(arg.dstFileName, &fractalImg);

	FreeConfig(&config);
	FreeGradient(&gradient);
	FreeImage(&fractalImg);

	info(T_NORMAL,"All done.\n");

	exit(EXIT_SUCCESS);
}

