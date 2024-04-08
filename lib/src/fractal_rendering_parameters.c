/*
 *  fractal_rendering_parameters.c -- part of FractalNow
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
#include "file_io.h"
#include "misc.h"
#include <ctype.h>
#include <stdio.h>
#include <string.h>

const char *renderingFileFormatStr[] = {
	(const char *)"r075"
};
const uint_fast32_t nbRenderingFileFormats = sizeof(renderingFileFormatStr) / sizeof(const char *);

int ReadRenderingFileV075(RenderingParameters *param, const char *fileName,
				FILE *file);
typedef int (*ReadRenderingFileFunction)(RenderingParameters *param,
					const char *fileName, FILE *file);
const ReadRenderingFileFunction readRenderingFileFunction[] = {
	ReadRenderingFileV075
};

int WriteRenderingFileV075(const RenderingParameters *param, const char *fileName,
				FILE *file);
typedef int (*WriteRenderingFileFunction)(const RenderingParameters *param,
						const char *fileName, FILE *file);
const WriteRenderingFileFunction writeRenderingFileFunction[] = {
	WriteRenderingFileV075
};

inline void InitRenderingParameters(RenderingParameters *param, uint_fast8_t bytesPerComponent, Color spaceColor,
				CountingFunction countingFunction, ColoringMethod coloringMethod,
				AddendFunction addendFunction, uint_fast32_t stripeDensity,
				InterpolationMethod interpolationMethod, TransferFunction transferFunction,
				FLOATT multiplier, FLOATT offset, Gradient gradient)
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

void ResetGradient(RenderingParameters *param, Gradient gradient)
{
	FreeGradient(param->gradient);
	param->gradient = gradient;
	param->bytesPerComponent = gradient.bytesPerComponent;
	param->realMultiplier = param->multiplier*gradient.size;
	param->realOffset = param->offset*gradient.size;
}

int ReadRenderingFileV075(RenderingParameters *param, const char *fileName, FILE *file)
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
	FLOATT multiplier;
	FLOATT offset;
	Gradient gradient;
	char str[256];

	uint32_t bytesPerComponent32;
	if (readUint32(file, &bytesPerComponent32) < 1) {
		FractalNow_read_werror(fileName);
	}
	if (bytesPerComponent32 == 1 || bytesPerComponent32 == 2) {
		bytesPerComponent = (uint_fast8_t)bytesPerComponent32;
	} else {
		FractalNow_werror("Invalid rendering file : bytes per components must be 1 or 2.\n");
	}

	if (readColor(file, bytesPerComponent, &spaceColor) < 1) {
		FractalNow_read_werror(fileName);
	}

	if (readString(file, str) < 1) {
		FractalNow_read_werror(fileName);
	}
	if (GetColoringMethod(&coloringMethod, str)) {
		FractalNow_werror("Invalid rendering file : could not get coloring method.\n");
	}

	/* Just to be sure these are initialized, because they are always needed (and thus read). */
	countingFunction = CF_SMOOTH;
	interpolationMethod = IM_NONE;
	addendFunction = AF_TRIANGLEINEQUALITY;
	stripeDensity = 1;

	switch (coloringMethod) {
	case CM_ITERATIONCOUNT:
		if (readString(file, str) < 1) {
			FractalNow_read_werror(fileName);
		}
		if (GetCountingFunction(&countingFunction, str)) {
			FractalNow_werror("Invalid rendering file : could not get counting function.\n");
		}
		break;
	case CM_AVERAGECOLORING:
		if (readString(file, str) < 1) {
			FractalNow_read_werror(fileName);
		}
		if (GetAddendFunction(&addendFunction, str)) {
			FractalNow_werror("Invalid rendering file : could not get addend function.\n");
		}
		if (addendFunction == AF_STRIPE) {
			if (readUint32(file, &stripeDensity) < 1) {
				FractalNow_read_werror(fileName);
			}
		}
		if (readString(file, str) < 1) {
			FractalNow_read_werror(fileName);
		}
		if (GetInterpolationMethod(&interpolationMethod, str)) {
			FractalNow_werror("Invalid rendering file : could not get interpolation method.\n");
		}
		break;
	default:
		FractalNow_werror("Unknown coloring method.\n");
		break;
	}

	if (readString(file, str) < 1) {
		FractalNow_read_werror(fileName);
	}
	if (GetTransferFunction(&transferFunction, str)) {
		FractalNow_werror("Invalid rendering file : could not get transfer function.\n");
	}

	if (readFLOATT(file, &multiplier) < 1) {
		FractalNow_read_werror(fileName);
	}
	if (readFLOATT(file, &offset) < 1) {
		FractalNow_read_werror(fileName);
	}

	if (ReadGradientFileBody(&gradient, bytesPerComponent, fileName, file, "g073")) {
		FractalNow_werror("Invalid rendering file : failed to read gradient.\n");
	}

	InitRenderingParameters(param, bytesPerComponent, spaceColor, countingFunction, coloringMethod,
				addendFunction, stripeDensity, interpolationMethod, transferFunction,
				multiplier, offset, gradient);

	end:

	return res;
}

ReadRenderingFileFunction GetReadRenderingFileFunction(const char *format)
{
	if (strlen(format) != 4) {
		return NULL;
	}

	ReadRenderingFileFunction readRenderingFile = NULL;
	char formatStr[5];
	strcpy(formatStr, format);
	toLowerCase(formatStr);

	uint_fast32_t i;
	for (i = 0; i < nbRenderingFileFormats; ++i) {
		if (strcmp(formatStr, renderingFileFormatStr[i]) == 0) {
			readRenderingFile = readRenderingFileFunction[i];
			break;
		}
	}
	
	return readRenderingFile;
}

int isSupportedRenderingFile(const char *fileName)
{
	int res = 0;
	FILE *file;

	file=fopen(fileName,"r");
	if (!file) {
		FractalNow_open_werror(fileName);
	}
	
	char formatStr[256];
	if (readString(file, formatStr) < 1) {
		res = 1;
	} else {
		ReadRenderingFileFunction readRenderingFileFunction;
		readRenderingFileFunction = GetReadRenderingFileFunction(formatStr);
		res = (readRenderingFileFunction == NULL);
	}

	end:
	if (file && fclose(file)) {
		FractalNow_close_errmsg(fileName);
		res = 1;
	}

	return !res;
}

int ReadRenderingFileBody(RenderingParameters *param, const char *fileName,
			FILE *file, const char *format)
{
	FractalNow_message(stdout, T_VERBOSE, "Reading rendering file body...\n");

	int res = 0;
	ReadRenderingFileFunction readRenderingFile;
	readRenderingFile = GetReadRenderingFileFunction(format);
	if (readRenderingFile == NULL) {
		FractalNow_werror("Unsupported rendering file format '%s'.\n", format);
	}

	res |= readRenderingFile(param, fileName, file);

	end:
	FractalNow_message(stdout, T_VERBOSE, "Reading rendering file body : %s.\n", (res == 0) ? "DONE" : "FAILED");

	return res;
} 

int ReadRenderingFile(RenderingParameters *param, const char *fileName)
{
	FractalNow_message(stdout, T_NORMAL, "Reading rendering file...\n");

	int res = 0;
	FILE *file;

	file=fopen(fileName,"r");
	if (!file) {
		FractalNow_open_werror(fileName);
	}
	
	char formatStr[256];
	if (readString(file, formatStr) < 1) {
		FractalNow_read_werror(fileName);
	}
	res = ReadRenderingFileBody(param, fileName, file, formatStr);

	end:
	if (file && fclose(file)) {
		FractalNow_close_errmsg(fileName);
		res = 1;
	}

	FractalNow_message(stdout, T_NORMAL, "Reading rendering file : %s.\n", (res == 0) ? "DONE" : "FAILED");

	return res;
}

int WriteRenderingFileV075(const RenderingParameters *param, const char *fileName, FILE *file)
{
	int res = 0;

	if (writeUint32(file, param->bytesPerComponent, "\n") < 0) {
		FractalNow_write_werror(fileName);
	}
	if (writeColor(file, param->spaceColor, "\n") < 0) {
		FractalNow_write_werror(fileName);
	}
	if (writeString(file, coloringMethodStr[(int)param->coloringMethod], "\n") < 0) {
		FractalNow_write_werror(fileName);
	}
	switch (param->coloringMethod) {
	case CM_ITERATIONCOUNT:
		if (writeString(file, countingFunctionStr[(int)param->countingFunction], "\n") < 0) {
			FractalNow_write_werror(fileName);
		}
		break;
	case CM_AVERAGECOLORING:
		if (writeString(file, addendFunctionStr[(int)param->addendFunction], " ") < 0) {
			FractalNow_write_werror(fileName);
		}
		if (param->addendFunction == AF_STRIPE) {
			if (writeUint32(file, param->stripeDensity, " ") < 0) {
				FractalNow_write_werror(fileName);
			}
		}
		if (writeString(file, interpolationMethodStr[(int)param->interpolationMethod], "\n") < 0) {
			FractalNow_write_werror(fileName);
		}
		break;
	default:
		FractalNow_werror("Unknown coloring method.\n");
		break;
	}
	if (writeString(file, transferFunctionStr[(int)param->transferFunction], "\n") < 0) {
		FractalNow_write_werror(fileName);
	}
	if (writeFLOATT(file, param->multiplier, " ") < 0) {
		FractalNow_write_werror(fileName);
	}
	if (writeFLOATT(file, param->offset, "\n") < 0) {
		FractalNow_write_werror(fileName);
	}

	if (WriteGradientFileBody(&param->gradient, fileName, file, "g073")) {
		FractalNow_werror("Failed to write gradient.\n");
	}

	end:

	return res;
}

WriteRenderingFileFunction GetWriteRenderingFileFunction(const char *format)
{
	if (strlen(format) != 4) {
		return NULL;
	}

	WriteRenderingFileFunction writeRenderingFile = NULL;
	char formatStr[5];
	strcpy(formatStr, format);
	toLowerCase(formatStr);

	uint_fast32_t i;
	for (i = 0; i < nbRenderingFileFormats; ++i) {
		if (strcmp(formatStr, renderingFileFormatStr[i]) == 0) {
			writeRenderingFile = writeRenderingFileFunction[i];
			break;
		}
	}
	
	return writeRenderingFile;
}

int WriteRenderingFileBody(const RenderingParameters *param, const char *fileName,
				FILE *file, const char *format)
{
	FractalNow_message(stdout, T_VERBOSE, "Writing rendering file body...\n");

	int res = 0;
	WriteRenderingFileFunction writeRenderingFile;
	writeRenderingFile = GetWriteRenderingFileFunction(format);
	if (writeRenderingFile == NULL) {
		FractalNow_werror("Unsupported rendering file format '%s'.\n", format);
	}

	res |= writeRenderingFile(param, fileName, file);

	end:
	FractalNow_message(stdout, T_VERBOSE, "Writing rendering file body : %s.\n", (res == 0) ? "DONE" : "FAILED");

	return res;
} 

int WriteRenderingFile(const RenderingParameters *param, const char *fileName)
{
	FractalNow_message(stdout, T_NORMAL, "Writing rendering file...\n");

	int res = 0;
	FILE *file;

	file=fopen(fileName,"w");
	if (!file) {
		FractalNow_open_werror(fileName);
	}
	
	const char *format = renderingFileFormatStr[nbRenderingFileFormats-1];
	if (writeString(file, format, "\n") < 0) {
		FractalNow_write_werror(fileName);
	}
	res = WriteRenderingFileBody(param, fileName, file, format);

	end:
	if (file && fclose(file)) {
		FractalNow_close_errmsg(fileName);
		res = 1;
	}

	FractalNow_message(stdout, T_NORMAL, "Writing rendering file : %s.\n", (res == 0) ? "DONE" : "FAILED");

	return res;
}

void FreeRenderingParameters(RenderingParameters param)
{
	FreeGradient(param.gradient);
}
