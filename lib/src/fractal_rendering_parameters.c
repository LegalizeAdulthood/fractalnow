/*
 *  fractal_rendering_parameters.c -- part of fractal2D
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

#include "fractal_rendering_parameters.h"
#include "error.h"
#include "file_parsing.h"
#include "misc.h"
#include <ctype.h>
#include <string.h>

inline void InitRenderingParameters(RenderingParameters *param, uint_fast8_t bytesPerComponent, Color spaceColor,
				CountingFunction countingFunction, ColoringMethod coloringMethod,
				AddendFunction addendFunction, uint_fast32_t stripeDensity,
				InterpolationMethod interpolationMethod, TransferFunction transferFunction,
				FLOAT multiplier, FLOAT offset, Gradient gradient)
{
	param->bytesPerComponent = bytesPerComponent;
	param->spaceColor = spaceColor;
	param->countingFunction = countingFunction;
	param->countingFunctionPtr = GetCountingFunctionPtr(countingFunction);
	param->coloringMethod = coloringMethod;
	param->addendFunction = addendFunction;
	param->stripeDensity = stripeDensity;
	param->interpolationMethod = interpolationMethod;
	param->transferFunction = transferFunction;
	param->transferFunctionPtr = GetTransferFunctionPtr(transferFunction);
	param->multiplier = multiplier;
	param->realMultiplier = multiplier*gradient.size;
	param->offset = offset;
	param->realOffset = offset*gradient.size;
	param->gradient = gradient;
}

RenderingParameters CopyRenderingParameters(const RenderingParameters *param)
{
	RenderingParameters res = *param;
	res.gradient = CopyGradient(&param->gradient);

	return res;
}

char *supportedRenderingFileFormat[] = {
	(char *)"r072"
};

int ReadRenderingFile(RenderingParameters *param, const char *fileName)
{
	fractal2D_message(stdout, T_NORMAL, "Reading rendering file...\n");

	int res = 0;
	FILE *file;

	file=fopen(fileName,"r");
	if (!file) {
		fractal2D_open_werror(fileName);
	}
	
	char str[256];
	if (readString(file, str) < 1) {
		fractal2D_read_werror(fileName);
	}
	if (strlen(str) != 4 || tolower(str[0]) != 'r') {
		fractal2D_werror("Invalid rendering file (could not read format).\n");
	}
	toLowerCase(str);
	if (strcmp(str, supportedRenderingFileFormat[0]) != 0) {
		fractal2D_werror("Unsupported rendering file format '%s'.\n", str);
	}

	uint_fast8_t bytesPerComponent;
	Color spaceColor;
	CountingFunction countingFunction;
	ColoringMethod coloringMethod;
	AddendFunction addendFunction;
	uint32_t stripeDensity;
	InterpolationMethod interpolationMethod;
	TransferFunction transferFunction;
	FLOAT multiplier;
	FLOAT offset;
	Gradient gradient;

	uint32_t bytesPerComponent32;
	if (readUint32(file, &bytesPerComponent32) < 1) {
		fractal2D_read_werror(fileName);
	}
	if (bytesPerComponent32 == 1 || bytesPerComponent32 == 2) {
		bytesPerComponent = (uint_fast8_t)bytesPerComponent32;
	} else {
		fractal2D_werror("Invalid rendering file : bytes per components must be 1 or 2.\n");
	}

	if (readColor(file, bytesPerComponent, &spaceColor) < 1) {
		fractal2D_read_werror(fileName);
	}

	if (readString(file, str) < 1) {
		fractal2D_read_werror(fileName);
	}
	if (GetCountingFunction(&countingFunction, str)) {
		fractal2D_werror("Invalid rendering file : could not get counting function.\n");
	}

	if (readString(file, str) < 1) {
		fractal2D_read_werror(fileName);
	}
	if (GetColoringMethod(&coloringMethod, str)) {
		fractal2D_werror("Invalid rendering file : could not get coloring method.\n");
	}

	/* Just to be sure these are initialized, because they are always needed (and thus read). */
	interpolationMethod = IM_NONE;
	addendFunction = AF_TRIANGLEINEQUALITY;
	stripeDensity = 1;

	if (coloringMethod == CM_AVERAGE) {
		if (readString(file, str) < 1) {
			fractal2D_read_werror(fileName);
		}
		if (GetAddendFunction(&addendFunction, str)) {
			fractal2D_werror("Invalid rendering file : could not get addend function.\n");
		}
		if (addendFunction == AF_STRIPE) {
			if (readUint32(file, &stripeDensity) < 1) {
				fractal2D_read_werror(fileName);
			}
		}
		if (readString(file, str) < 1) {
			fractal2D_read_werror(fileName);
		}
		if (GetInterpolationMethod(&interpolationMethod, str)) {
			fractal2D_werror("Invalid rendering file : could not get interpolation method.\n");
		}
	}

	if (readString(file, str) < 1) {
		fractal2D_read_werror(fileName);
	}
	if (GetTransferFunction(&transferFunction, str)) {
		fractal2D_werror("Invalid rendering file : could not get transfer function.\n");
	}

	if (readFLOAT(file, &multiplier) < 1) {
		fractal2D_read_werror(fileName);
	}
	if (readFLOAT(file, &offset) < 1) {
		fractal2D_read_werror(fileName);
	}

	Color color[256];
	uint_fast32_t nbColors=0;
	int nbread;
	while ((nbread = readColor(file, bytesPerComponent, &color[nbColors])) != EOF) {
		if (nbread < 1) {
			fractal2D_read_werror(fileName);
		}
		nbColors++;
	}

	if (nbColors < 2) {
		fractal2D_werror("Invalid rendering file : number of colors must be between 2 and 256.\n");
	}

	GenerateGradient(&gradient, color, nbColors, DEFAULT_GRADIENT_TRANSITIONS);

	InitRenderingParameters(param, bytesPerComponent, spaceColor, countingFunction, coloringMethod,
				addendFunction, stripeDensity, interpolationMethod, transferFunction,
				multiplier, offset, gradient);

	end:
	if (file) {
		res |= fclose(file);
	}

	fractal2D_message(stdout, T_NORMAL, "Reading rendering file : %s.\n", (res == 0) ? "DONE" : "FAILED");

	return res;
}

void FreeRenderingParameters(RenderingParameters param)
{
	FreeGradient(param.gradient);
}
