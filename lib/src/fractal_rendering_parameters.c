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
#include <stdio.h>
#include <string.h>

const char *renderingFileFormatStr[] = {
	(const char *)"r072",
	(const char *)"r073"
};

int  ReadRenderingFileV072(RenderingParameters *param, const char *fileName,
				FILE *file);
int  ReadRenderingFileV073(RenderingParameters *param, const char *fileName,
				FILE *file);

typedef int (*ReadRenderingFileFunction)(RenderingParameters *param,
					const char *fileName, FILE *file);

const ReadRenderingFileFunction readRenderingFileFunction[] = {
	ReadRenderingFileV072,
	ReadRenderingFileV073
};

const uint_fast32_t nbRenderingFileFormats = sizeof(renderingFileFormatStr) / sizeof(const char *);

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

int ReadRenderingFileV07x(RenderingParameters *param, const char *fileName, FILE *file,
				const char *gradientFormat)
{
	int res = 0;
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
	char str[256];

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

	if (ReadGradientFileBody(&gradient, bytesPerComponent, fileName, file, gradientFormat)) {
		fractal2D_werror("Invalid rendering file : failed to read gradient.\n");
	}

	InitRenderingParameters(param, bytesPerComponent, spaceColor, countingFunction, coloringMethod,
				addendFunction, stripeDensity, interpolationMethod, transferFunction,
				multiplier, offset, gradient);

	end:

	return res;
}

int ReadRenderingFileV072(RenderingParameters *param, const char *fileName, FILE *file)
{
	const char gradientFormat[] = "g072";
	return ReadRenderingFileV07x(param, fileName, file, gradientFormat);
}

int ReadRenderingFileV073(RenderingParameters *param, const char *fileName, FILE *file)
{
	const char gradientFormat[] = "g073";
	return ReadRenderingFileV07x(param, fileName, file, gradientFormat);
}

int ReadRenderingFileBody(RenderingParameters *param, const char *fileName,
			FILE *file, const char *format)
{
	fractal2D_message(stdout, T_VERBOSE, "Reading rendering file body...\n");

	int res = 0;
	if (strlen(format) != 4) {
		fractal2D_werror("Invalid rendering file format '%s'.\n", format);
	}
	char formatStr[5];
	strcpy(formatStr, format);
	toLowerCase(formatStr);

	ReadRenderingFileFunction readRenderingFile;
	uint_fast32_t i;
	for (i = 0; i < nbRenderingFileFormats; ++i) {
		if (strcmp(formatStr, renderingFileFormatStr[i]) == 0) {
			readRenderingFile = (ReadRenderingFileFunction)readRenderingFileFunction[i];
			break;
		}
	}
	if (i == nbRenderingFileFormats) {
		fractal2D_werror("Unsupported rendering file format '%s'.\n", format);
	}

	res |= readRenderingFile(param, fileName, file);

	end:
	fractal2D_message(stdout, T_VERBOSE, "Reading rendering file body : %s.\n", (res == 0) ? "DONE" : "FAILED");

	return res;
} 

int ReadRenderingFile(RenderingParameters *param, const char *fileName)
{
	fractal2D_message(stdout, T_NORMAL, "Reading rendering file...\n");

	int res = 0;
	FILE *file;

	file=fopen(fileName,"r");
	if (!file) {
		fractal2D_open_werror(fileName);
	}
	
	char formatStr[256];
	if (readString(file, formatStr) < 1) {
		fractal2D_read_werror(fileName);
	}
	res = ReadRenderingFileBody(param, fileName, file, formatStr);

	end:
	if (file && fclose(file)) {
		fractal2D_close_errmsg(fileName);
		res = 1;
	}

	fractal2D_message(stdout, T_NORMAL, "Reading rendering file : %s.\n", (res == 0) ? "DONE" : "FAILED");

	return res;
}

void FreeRenderingParameters(RenderingParameters param)
{
	FreeGradient(param.gradient);
}
