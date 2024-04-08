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

#include "anti_aliasing.h"
#include "command_line.h"
#include "fractal.h"
#include "image.h"
#include "locale.h"
#include "ppm.h"
#include <getopt.h>
#include <inttypes.h>
#include <stdint.h>
#include <unistd.h>

int main(int argc, char *argv[]) {
	setlocale(LC_NUMERIC, "C");
	CommandLineArguments arg;
	ParseCommandLineArguments(&arg, argc, argv);

	Fractal fractal;
	ReadFractalFile(&fractal, arg.fractalFileName);
	RenderingParameters render;
	ReadRenderingFile(&render, arg.renderingFileName);

	uint_fast32_t width = arg.width;
	uint_fast32_t height = arg.height;
	FLOAT spanX = fractal.spanX;
	FLOAT spanY = fractal.spanY;
	if (width == 0) {
		width = roundF(spanX * height / spanY);
	} else if (height == 0) {
		height = roundF(spanY * width / spanX);
	}

	Image fractalImg, tmpImg;
	CreateImage(&fractalImg, width, height, render.bytesPerComponent);

	switch (arg.antiAliasingMethod) {
	case AAM_NONE:
		DrawFractalFast(&fractalImg, &fractal, &render, arg.quadInterpolationSize, arg.colorDissimilarityThreshold);
		break;
	case AAM_GAUSSIANBLUR:
		CreateImage(&tmpImg, width, height, render.bytesPerComponent);
		
		DrawFractalFast(&tmpImg, &fractal, &render, arg.quadInterpolationSize, arg.colorDissimilarityThreshold);
		ApplyGaussianBlur(&fractalImg, &tmpImg, arg.antiAliasingSize);

		FreeImage(&tmpImg);
		break;
	case AAM_OVERSAMPLING:
		CreateImage(&tmpImg, width*arg.antiAliasingSize, height*arg.antiAliasingSize, render.bytesPerComponent);

		DrawFractalFast(&tmpImg, &fractal, &render, arg.quadInterpolationSize, arg.colorDissimilarityThreshold);
		DownscaleImage(&fractalImg, &tmpImg);

		FreeImage(&tmpImg);
		break;
	case AAM_ADAPTIVE:
		DrawFractalFast(&fractalImg, &fractal, &render, arg.quadInterpolationSize, arg.colorDissimilarityThreshold);
		AntiAliaseFractal(&fractalImg, &fractal, &render, arg.antiAliasingSize, arg.adaptiveAAMThreshold);
		break;
	default:
		error("Unknown anti-aliasing method.\n");
		break;
	}

	ExportPPM(arg.dstFileName, &fractalImg);

	FreeRenderingParameters(&render);
	FreeImage(&fractalImg);

	info(T_NORMAL,"All done.\n");

	exit(EXIT_SUCCESS);
}

