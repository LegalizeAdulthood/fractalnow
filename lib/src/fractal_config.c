/*
 *  fractal_config.c -- part of FractalNow
 *
 *  Copyright (c) 2012 Marc Pegon <pe.marc@free.fr>
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

#include "fractal_config.h"
#include "file_io.h"
#include "fractal.h"
#include "fractal_rendering_parameters.h"
#include "misc.h"
#include <string.h>

const char *fractalConfigFormatStr[] = {
	(const char *)"c075"
};
const uint_fast32_t nbFractalConfigFormats = sizeof(fractalConfigFormatStr) /
						sizeof(const char *);

int ReadFractalConfigFileV075(FractalConfig *fractalConfig, const char *fileName, FILE *file);
typedef int (*ReadFractalConfigFileFunction)(FractalConfig *fractalConfig, const char *fileName,
						FILE *file);
const ReadFractalConfigFileFunction readFractalConfigFileFunction[] = {
	ReadFractalConfigFileV075
};

int WriteFractalConfigFileV075(const FractalConfig *fractalConfig, const char *fileName, FILE *file);
typedef int (*WriteFractalConfigFileFunction)(const FractalConfig *fractalConfig,
						const char *fileName, FILE *file);
const WriteFractalConfigFileFunction writeFractalConfigFileFunction[] = {
	WriteFractalConfigFileV075
};

inline void InitFractalConfig(FractalConfig *config, Fractal fractal, RenderingParameters render)
{
	config->fractal = fractal;
	config->render = render;
}

FractalConfig CopyFractalConfig(const FractalConfig *config)
{
	FractalConfig res;
	InitFractalConfig(&res, CopyFractal(&config->fractal),
				CopyRenderingParameters(&config->render));
	return res;
}

void ResetFractal(FractalConfig *config, Fractal fractal)
{
	FreeFractal(config->fractal);
	config->fractal = fractal;
}

void ResetRenderingParameters(FractalConfig *config, RenderingParameters param)
{
	FreeRenderingParameters(config->render);
	config->render = param;
}

int ReadFractalConfigFileV075(FractalConfig *fractalConfig, const char *fileName, FILE *file)
{
	int res = 0;
	const char fractalFormat[] = "f075";
	const char renderingFormat[] = "r075";
	res |= ReadFractalFileBody(&fractalConfig->fractal, fileName, file, fractalFormat);
	if (res) {
		FractalNow_werror("Failed to read fractal from fractal configuration file.\n");
	}
	res |= ReadRenderingFileBody(&fractalConfig->render, fileName, file, renderingFormat);
	if (res) {
		FractalNow_werror("Failed to read rendering parameters from fractal \
configuration file.\n");
	}

	end:
	return res;
}

ReadFractalConfigFileFunction GetReadFractalConfigFileFunction(const char *format)
{
	if (strlen(format) != 4) {
		return NULL;
	}

	ReadFractalConfigFileFunction readFractalConfigFile = NULL;
	char formatStr[5];
	strcpy(formatStr, format);
	toLowerCase(formatStr);

	uint_fast32_t i;
	for (i = 0; i < nbFractalConfigFormats; ++i) {
		if (strcmp(formatStr, fractalConfigFormatStr[i]) == 0) {
			readFractalConfigFile = readFractalConfigFileFunction[i];
			break;
		}
	}
	
	return readFractalConfigFile;
}

int isSupportedFractalConfigFile(const char *fileName)
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
		ReadFractalConfigFileFunction readFractalConfigFileFunction;
		readFractalConfigFileFunction = GetReadFractalConfigFileFunction(formatStr);
		res = (readFractalConfigFileFunction == NULL);
	}

	end:
	if (file && fclose(file)) {
		FractalNow_close_errmsg(fileName);
		res = 1;
	}

	return !res;
}

int ReadFractalConfigFileBody(FractalConfig *fractalConfig, const char *fileName,
				FILE *file, const char *format)
{
	FractalNow_message(stdout, T_VERBOSE, "Reading fractal config file body...\n");
	int res = 0;
	ReadFractalConfigFileFunction readFractalConfigFile;
	readFractalConfigFile = GetReadFractalConfigFileFunction(format);
	if (readFractalConfigFile == NULL) {
		FractalNow_werror("Unsupported fractal config format '%s'.\n", format);
	}

	res |= readFractalConfigFile(fractalConfig, fileName, file);

	end:
	FractalNow_message(stdout, T_VERBOSE, "Reading fractal config file body : %s.\n",
				(res == 0) ? "DONE" : "FAILED");

	return res;
}

int ReadFractalConfigFile(FractalConfig *fractalConfig, const char *fileName)
{
	FractalNow_message(stdout, T_NORMAL, "Reading fractal configuration file...\n");

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
	res = ReadFractalConfigFileBody(fractalConfig, fileName, file, formatStr);

	end:
	if (file && fclose(file)) {
		FractalNow_close_errmsg(fileName);
		res = 1;
	}

	FractalNow_message(stdout, T_NORMAL, "Reading fractal configuration file : %s.\n",
				(res == 0) ? "DONE" : "FAILED");

	return res;
}

int WriteFractalConfigFileV075(const FractalConfig *fractalConfig, const char *fileName, FILE *file)
{
	int res = 0;
	const char fractalFormat[] = "f075";
	const char renderingFormat[] = "r075";
	res |= WriteFractalFileBody(&fractalConfig->fractal, fileName, file, fractalFormat);
	if (res) {
		FractalNow_werror("Failed to write fractal file.\n");
	}
	res |= WriteRenderingFileBody(&fractalConfig->render, fileName, file, renderingFormat);
	if (res) {
		FractalNow_werror("Failed to read rendering file.\n");
	}

	end:
	return res;
}

WriteFractalConfigFileFunction GetWriteFractalConfigFileFunction(const char *format)
{
	if (strlen(format) != 4) {
		return NULL;
	}

	WriteFractalConfigFileFunction writeFractalConfigFile = NULL;
	char formatStr[5];
	strcpy(formatStr, format);
	toLowerCase(formatStr);

	uint_fast32_t i;
	for (i = 0; i < nbFractalConfigFormats; ++i) {
		if (strcmp(formatStr, fractalConfigFormatStr[i]) == 0) {
			writeFractalConfigFile = writeFractalConfigFileFunction[i];
			break;
		}
	}
	
	return writeFractalConfigFile;
}

int WriteFractalConfigFileBody(const FractalConfig *fractalConfig, const char *fileName,
				FILE *file, const char *format)
{
	FractalNow_message(stdout, T_VERBOSE, "Writing fractal config file body...\n");
	int res = 0;
	WriteFractalConfigFileFunction writeFractalConfigFile;
	writeFractalConfigFile = GetWriteFractalConfigFileFunction(format);
	if (writeFractalConfigFile == NULL) {
		FractalNow_werror("Unsupported fractal config format '%s'.\n", format);
	}

	res |= writeFractalConfigFile(fractalConfig, fileName, file);

	end:
	FractalNow_message(stdout, T_VERBOSE, "Writing fractal config file body : %s.\n",
				(res == 0) ? "DONE" : "FAILED");

	return res;
}

int WriteFractalConfigFile(const FractalConfig *fractalConfig, const char *fileName)
{
	FractalNow_message(stdout, T_NORMAL, "Writing fractal configuration file...\n");

	int res = 0;
	FILE *file;

	file=fopen(fileName,"w");
	if (!file) {
		FractalNow_open_werror(fileName);
	}
	
	const char *format = fractalConfigFormatStr[nbFractalConfigFormats-1];
	if (writeString(file, format, "\n") < 0) {
		FractalNow_write_werror(fileName);
	}
	res = WriteFractalConfigFileBody(fractalConfig, fileName, file, format);

	end:
	if (file && fclose(file)) {
		FractalNow_close_errmsg(fileName);
		res = 1;
	}

	FractalNow_message(stdout, T_NORMAL, "Writing fractal configuration file : %s.\n",
				(res == 0) ? "DONE" : "FAILED");

	return res;
}

void FreeFractalConfig(FractalConfig fractalConfig)
{
	FreeRenderingParameters(fractalConfig.render);
}
