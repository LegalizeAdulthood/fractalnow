/*
 *  gradient.c -- part of FractalNow
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
 
#include "gradient.h"
#include "error.h"
#include "file_io.h"
#include "misc.h"
#include <string.h>

const char *gradientFormatStr[] = {
	(const char *)"g073"
};
const uint_fast32_t nbGradientFormats = sizeof(gradientFormatStr) / sizeof(const char *);

int  ReadGradientFileV073(Gradient *gradient, uint_fast8_t bytesPerComponent,
				const char *fileName, FILE *file);
typedef int (*ReadGradientFileFunction)(Gradient *gradient, uint_fast8_t bytesPerComponent,
					const char *fileName, FILE *file);

const ReadGradientFileFunction readGradientFileFunction[] = {
	ReadGradientFileV073
};

int  WriteGradientFileV073(const Gradient *gradient, const char *fileName, FILE *file);
typedef int (*WriteGradientFileFunction)(const Gradient *gradient, const char *fileName, FILE *file);
const WriteGradientFileFunction writeGradientFileFunction[] = {
	WriteGradientFileV073
};

void aux_GenerateGradient(Gradient *gradient, uint_fast64_t begin_ind_tab, uint_fast64_t end_ind_tab, 
				Color C1, Color C2)
{
	uint_fast64_t N = end_ind_tab - begin_ind_tab;
	Color *pixel = gradient->data+begin_ind_tab;
	if (N == 0) {
		*pixel = C2;
	} else {
		for (uint_fast64_t i=0;i<=N;i++) {
			pixel->bytesPerComponent = C1.bytesPerComponent;
			pixel->r = (C1.r*(N-i) + C2.r*i) / N;
			pixel->g = (C1.g*(N-i) + C2.g*i) / N;
			pixel->b = (C1.b*(N-i) + C2.b*i) / N;
			++pixel;
		}
	}
}

void GenerateGradient(Gradient *gradient, uint_fast32_t nbStops, FLOATT *positionStop,
				Color *colorStop, uint_fast32_t size)
{
	FractalNow_message(stdout, T_NORMAL,"Generating gradient...\n");
	
	if (nbStops < 2) {
		FractalNow_error("Gradient number of stops must be >= 2.\n");
	}
	if (size == 0) {
		FractalNow_error("Gradient size must be > 0.\n");
	}

	gradient->positionStop = (FLOATT *)safeMalloc("position stops copy", nbStops*sizeof(FLOATT));
	memcpy(gradient->positionStop, positionStop, nbStops*sizeof(FLOATT));
	gradient->colorStop = (Color *)safeMalloc("color stops copy", nbStops*sizeof(Color));
	memcpy(gradient->colorStop, colorStop, nbStops*sizeof(Color));
	gradient->nbStops = nbStops;
	gradient->bytesPerComponent = colorStop[0].bytesPerComponent;
	gradient->size = size;

	gradient->data = (Color *)safeMalloc("gradient data", size*sizeof(Color));
	for (uint_fast32_t i=0; i<nbStops-1; ++i) {
		if (positionStop[i] < 0 || positionStop[i] > 1) {
			FractalNow_error("Gradient position stops must be between 0 and 1.\n");
		} else if (i != 0 && positionStop[i] <= positionStop[i-1]) {
			FractalNow_error("Gradient position stops should be (stricly) increasing.\n");
		}
		aux_GenerateGradient(gradient, positionStop[i]*(size-1), positionStop[i+1]*(size-1),
					colorStop[i], colorStop[i+1]);
	}

	FractalNow_message(stdout, T_NORMAL,"Generating gradient : DONE.\n");
}

void GenerateGradient2(Gradient *gradient, uint_fast32_t nbStops, Color *colorStop, uint_fast32_t nbTransitions)
{
	if (nbTransitions == 0) {
		FractalNow_error("Gradient number of transitions must be > 0.\n");
	}

	FLOATT *positionStop = (FLOATT *)malloc(nbStops * sizeof(FLOATT));
	for (uint_fast32_t i = 0; i < nbStops; ++i) {
		positionStop[i] = i / (FLOATT)(nbStops-1);
	}

	uint_fast64_t size = (nbStops-1) * nbTransitions;
	GenerateGradient(gradient, nbStops, positionStop, colorStop, size);

	free(positionStop);
}

Gradient CopyGradient(const Gradient *gradient)
{
	Gradient res;
	res.bytesPerComponent = gradient->bytesPerComponent;
	res.size = gradient->size;
	res.data = (Color *)safeMalloc("gradient copy data", gradient->size*sizeof(Color));
	memcpy(res.data, gradient->data, gradient->size*sizeof(Color));
	res.nbStops = gradient->nbStops;
	res.positionStop = (FLOATT *)safeMalloc("gradient position stop copy", gradient->nbStops*sizeof(FLOATT));
	memcpy(res.positionStop, gradient->positionStop, gradient->nbStops*sizeof(FLOATT));
	res.colorStop = (Color *)safeMalloc("gradient color stop copy", gradient->nbStops*sizeof(Color));
	memcpy(res.colorStop, gradient->colorStop, gradient->nbStops*sizeof(Color));

	return res;
}

Gradient Gradient16(const Gradient *gradient)
{
	Gradient res;
	if (gradient->bytesPerComponent == 2) {
		res = CopyGradient(gradient);
	} else {
		Color *tmp = (Color *)safeMalloc("16 bits gradient color stops copy",
							gradient->nbStops*sizeof(Color));
		for (uint_fast32_t i = 0; i < gradient->nbStops; ++i) {
			tmp[i] = Color16(gradient->colorStop[i]);
		}

		GenerateGradient(&res, gradient->nbStops, gradient->positionStop, tmp,  gradient->size);
		free(tmp);
	}

	return res;
}

Gradient Gradient8(const Gradient *gradient)
{
	Gradient res;
	if (gradient->bytesPerComponent == 1) {
		res = CopyGradient(gradient);
	} else {
		Color *tmp = (Color *)safeMalloc("8 bits gradient color stops copy",
							gradient->nbStops*sizeof(Color));
		for (uint_fast32_t i = 0; i < gradient->nbStops; ++i) {
			tmp[i] = Color8(gradient->colorStop[i]);
		}

		GenerateGradient(&res, gradient->nbStops, gradient->positionStop, tmp,  gradient->size);
		free(tmp);
	}

	return res;
}

inline Color GetGradientColor(const Gradient *gradient, uint_fast64_t index)
{
	return gradient->data[index % gradient->size];
}

void FreeGradient(Gradient gradient)
{
	free(gradient.data);
	free(gradient.positionStop);
	free(gradient.colorStop);
}

int ReadGradientFileV073(Gradient *gradient, uint_fast8_t bytesPerComponent, const char *fileName, FILE *file)
{
	int res = 0;

	FLOATT positionStop[257];
	Color colorStop[257];
	uint_fast32_t nbStops=0;
	int readRes;
	while (1) {
		readRes = readFLOATT(file, &positionStop[nbStops]);
		if (readRes == EOF) {
			break;
		} else if (readRes != 1) {
			FractalNow_read_werror(fileName);
		}

		readRes = readColor(file, bytesPerComponent, &colorStop[nbStops]);
		if (readRes == EOF) {
			FractalNow_werror("Missing gradient color stop.\n");
		} else if (readRes != 1) {
			FractalNow_read_werror(fileName);
		}
		if (nbStops > 255) {
			break;
		}
		nbStops++;
	}
	if (nbStops < 2 || nbStops > 255) {
		FractalNow_werror("Gradient number of stops must be between 2 and 255.\n");
	}
	/* Check stops are well-formed (pos0 = 0 <= pos1 <= ... <= posN = 1) */
	if (positionStop[0] != 0) {
		FractalNow_werror("First gradient stop position should be 0.\n");
	}
	if (positionStop[nbStops-1] != 1) {
		FractalNow_werror("Last gradient stop position should be 1.\n");
	}
	for (uint_fast32_t i = 1; i < nbStops; ++i) {
		if (positionStop[i] <= positionStop[i-1]) {
			FractalNow_werror("Gradient stop positions should be (stricly) increasing.\n");
		}
	}

	GenerateGradient(gradient, nbStops, positionStop, colorStop, DEFAULT_GRADIENT_SIZE);

	end:
	return res;
}

ReadGradientFileFunction GetReadGradientFileFunction(const char *format)
{
	if (strlen(format) != 4) {
		return NULL;
	}

	ReadGradientFileFunction readGradientFile = NULL;
	char formatStr[5];
	strcpy(formatStr, format);
	toLowerCase(formatStr);

	uint_fast32_t i;
	for (i = 0; i < nbGradientFormats; ++i) {
		if (strcmp(formatStr, gradientFormatStr[i]) == 0) {
			readGradientFile = readGradientFileFunction[i];
			break;
		}
	}
	
	return readGradientFile;
}

int isSupportedGradientFile(const char *fileName)
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
		ReadGradientFileFunction readGradientFileFunction;
		readGradientFileFunction = GetReadGradientFileFunction(formatStr);
		res = (readGradientFileFunction == NULL);
	}

	end:
	if (file && fclose(file)) {
		FractalNow_close_errmsg(fileName);
		res = 1;
	}

	return !res;
}

int ReadGradientFileBody(Gradient *gradient, uint_fast8_t bytesPerComponent, const char *fileName,
			FILE *file, const char *format)
{
	FractalNow_message(stdout, T_VERBOSE, "Reading gradient file body...\n");
	int res = 0;
	ReadGradientFileFunction readGradientFile;
	readGradientFile = GetReadGradientFileFunction(format);
	if (readGradientFile == NULL) {
		FractalNow_werror("Unsupported gradient format '%s'.\n", format);
	}

	res |= readGradientFile(gradient, bytesPerComponent, fileName, file);

	end:
	FractalNow_message(stdout, T_VERBOSE, "Reading gradient file body : %s.\n", (res == 0) ? "DONE" : "FAILED");

	return res;
}

int ReadGradientFile(Gradient *gradient, const char *fileName)
{
	FractalNow_message(stdout, T_NORMAL, "Reading gradient file...\n");

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
	uint_fast8_t bytesPerComponent;
	uint32_t bytesPerComponent32;
	if (readUint32(file, &bytesPerComponent32) < 1) {
		FractalNow_read_werror(fileName);
	}
	if (bytesPerComponent32 == 1 || bytesPerComponent32 == 2) {
		bytesPerComponent = (uint_fast8_t)bytesPerComponent32;
	} else {
		FractalNow_werror("Invalid gradient file : bytes per components must be 1 or 2.\n");
	}
	res = ReadGradientFileBody(gradient, bytesPerComponent, fileName, file, formatStr);

	end:
	if (file && fclose(file)) {
		FractalNow_close_errmsg(fileName);
		res = 1;
	}

	FractalNow_message(stdout, T_NORMAL, "Reading gradient file : %s.\n", (res == 0) ? "DONE" : "FAILED");

	return res;
}

int WriteGradientFileV073(const Gradient *gradient, const char *fileName, FILE *file)
{
	int res = 0;

	const char *suffix = " ";
	for (uint_fast32_t i = 0; i < gradient->nbStops; ++i) {
		if (writeFLOATT(file, gradient->positionStop[i], " ") < 0) {
			FractalNow_write_werror(fileName);
		}
		if (i == gradient->nbStops-1) {
			suffix = "\n";
		}
		if (writeColor(file, gradient->colorStop[i], suffix) < 0) {
			FractalNow_write_werror(fileName);
		}
	}

	end:
	return res;
}

WriteGradientFileFunction GetWriteGradientFileFunction(const char *format)
{
	if (strlen(format) != 4) {
		return NULL;
	}

	WriteGradientFileFunction writeGradientFile = NULL;
	char formatStr[5];
	strcpy(formatStr, format);
	toLowerCase(formatStr);

	uint_fast32_t i;
	for (i = 0; i < nbGradientFormats; ++i) {
		if (strcmp(formatStr, gradientFormatStr[i]) == 0) {
			writeGradientFile = writeGradientFileFunction[i];
			break;
		}
	}
	
	return writeGradientFile;
}

int WriteGradientFileBody(const Gradient *gradient, const char *fileName, FILE *file,
				const char *format)
{
	FractalNow_message(stdout, T_VERBOSE, "Writing gradient file body...\n");
	int res = 0;
	WriteGradientFileFunction writeGradientFile;
	writeGradientFile = GetWriteGradientFileFunction(format);
	if (writeGradientFile == NULL) {
		FractalNow_werror("Unsupported gradient format '%s'.\n", format);
	}

	res |= writeGradientFile(gradient, fileName, file);

	end:
	FractalNow_message(stdout, T_VERBOSE, "Writing gradient file body : %s.\n", (res == 0) ? "DONE" : "FAILED");

	return res;
}

int WriteGradientFile(const Gradient *gradient, const char *fileName)
{
	FractalNow_message(stdout, T_NORMAL, "Writing gradient file...\n");

	int res = 0;
	FILE *file;

	file=fopen(fileName,"w");
	if (!file) {
		FractalNow_open_werror(fileName);
	}

	const char *format = gradientFormatStr[nbGradientFormats-1];
	if (writeString(file, format, "\n") < 0) {
		FractalNow_write_werror(fileName);
	}
	if (writeUint32(file, gradient->bytesPerComponent, "\n") < 0) {
		FractalNow_write_werror(fileName);
	}
	res |= WriteGradientFileBody(gradient, fileName, file, format);

	end:
	if (file && fclose(file)) {
		FractalNow_close_errmsg(fileName);
		res = 1;
	}

	FractalNow_message(stdout, T_VERBOSE, "Writing gradient file body : %s.\n", (res == 0) ? "DONE" : "FAILED");

	return res;
}

