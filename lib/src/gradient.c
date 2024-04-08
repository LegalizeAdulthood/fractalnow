/*
 *  gradient.c -- part of fractal2D
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
#include "file_parsing.h"
#include "misc.h"
#include <string.h>

const char *gradientFormatStr[] = {
	(const char *)"g072",
	(const char *)"g073"
};

int  ReadGradientFileV072(Gradient *gradient, uint_fast8_t bytesPerComponent,
				const char *fileName, FILE *file);
int  ReadGradientFileV073(Gradient *gradient, uint_fast8_t bytesPerComponent,
				const char *fileName, FILE *file);
typedef int (*ReadGradientFileFunction)(Gradient *gradient, uint_fast8_t bytesPerComponent,
					const char *fileName, FILE *file);

const ReadGradientFileFunction readGradientFileFunction[] = {
	ReadGradientFileV072,
	ReadGradientFileV073
};
const uint_fast32_t nbGradientFormats = sizeof(gradientFormatStr) / sizeof(const char *);

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

void GenerateGradient(Gradient *gradient, uint_fast32_t nbStops, FLOAT *positionStop,
				Color *colorStop, uint_fast32_t size)
{
	fractal2D_message(stdout, T_NORMAL,"Generating gradient...\n");
	
	if (nbStops < 2) {
		fractal2D_error("Gradient number of stops must be >= 2.\n");
	}
	if (size == 0) {
		fractal2D_error("Gradient size must be > 0.\n");
	}

	gradient->positionStop = (FLOAT *)safeMalloc("position stops copy", nbStops*sizeof(FLOAT));
	memcpy(gradient->positionStop, positionStop, nbStops*sizeof(FLOAT));
	gradient->colorStop = (Color *)safeMalloc("color stops copy", nbStops*sizeof(Color));
	memcpy(gradient->colorStop, colorStop, nbStops*sizeof(Color));
	gradient->nbStops = nbStops;
	gradient->size = size;

	gradient->data = (Color *)safeMalloc("gradient data", size*sizeof(Color));
	for (uint_fast32_t i=0; i<nbStops-1; ++i) {
		if (positionStop[i] < 0 || positionStop[i] > 1) {
			fractal2D_error("Gradient position stops must be between 0 and 1.\n");
		} else if (i != 0 && positionStop[i] <= positionStop[i-1]) {
			fractal2D_error("Gradient position stops should be (stricly) increasing.\n");
		}
		aux_GenerateGradient(gradient, positionStop[i]*(size-1), positionStop[i+1]*(size-1),
					colorStop[i], colorStop[i+1]);
	}

	fractal2D_message(stdout, T_NORMAL,"Generating gradient : DONE.\n");
}

void GenerateGradient2(Gradient *gradient, uint_fast32_t nbStops, Color *colorStop, uint_fast32_t nbTransitions)
{
	if (nbTransitions == 0) {
		fractal2D_error("Gradient number of transitions must be > 0.\n");
	}

	FLOAT *positionStop = (FLOAT *)malloc(nbStops * sizeof(FLOAT));
	for (uint_fast32_t i = 0; i < nbStops; ++i) {
		positionStop[i] = i / (FLOAT)(nbStops-1);
	}

	uint_fast64_t size = (nbStops-1) * nbTransitions;
	GenerateGradient(gradient, nbStops, positionStop, colorStop, size);

	free(positionStop);
}

Gradient CopyGradient(const Gradient *gradient)
{
	Gradient res;
	res.size = gradient->size;
	res.data = (Color *)safeMalloc("gradient copy data", gradient->size*sizeof(Color));
	memcpy(res.data, gradient->data, gradient->size*sizeof(Color));
	res.nbStops = gradient->nbStops;
	res.positionStop = (FLOAT *)safeMalloc("gradient position stop copy", gradient->nbStops*sizeof(FLOAT));
	memcpy(res.positionStop, gradient->positionStop, gradient->nbStops*sizeof(FLOAT));
	res.colorStop = (Color *)safeMalloc("gradient color stop copy", gradient->nbStops*sizeof(Color));
	memcpy(res.colorStop, gradient->colorStop, gradient->nbStops*sizeof(Color));

	return res;
}

Gradient Gradient16(const Gradient *gradient)
{
	Gradient res;
	if (gradient->colorStop[0].bytesPerComponent == 2) {
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
	if (gradient->colorStop[0].bytesPerComponent == 1) {
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

int ReadGradientFileV072(Gradient *gradient, uint_fast8_t bytesPerComponent, const char *fileName, FILE *file)
{
	int res = 0;

	Color color[257];
	uint_fast32_t nbColors=0;
	int readRes;
	while (1) {
		readRes = readColor(file, bytesPerComponent, &color[nbColors]);
		if (readRes == EOF) {
			break;
		} else if (readRes < 1) {
			fractal2D_read_werror(fileName);
		}
		if (nbColors > 255) {
			break;
		}
		nbColors++;
	}

	if (nbColors < 2 || nbColors > 255) {
		fractal2D_werror("Gradient number of colors must be between 2 and 255.\n");
	}

	GenerateGradient2(gradient, nbColors, color, DEFAULT_GRADIENT_TRANSITIONS);

	end:
	return res;
}

int ReadGradientFileV073(Gradient *gradient, uint_fast8_t bytesPerComponent, const char *fileName, FILE *file)
{
	int res = 0;

	FLOAT positionStop[257];
	Color colorStop[257];
	uint_fast32_t nbStops=0;
	int readRes;
	while (1) {
		readRes = readFLOAT(file, &positionStop[nbStops]);
		if (readRes == EOF) {
			break;
		} else if (readRes != 1) {
			fractal2D_read_werror(fileName);
		}

		readRes = readColor(file, bytesPerComponent, &colorStop[nbStops]);
		if (readRes == EOF) {
			fractal2D_werror("Missing gradient color stop.\n");
		} else if (readRes != 1) {
			fractal2D_read_werror(fileName);
		}
		if (nbStops > 255) {
			break;
		}
		nbStops++;
	}
	if (nbStops < 2 || nbStops > 255) {
		fractal2D_werror("Gradient number of stops must be between 2 and 255.\n");
	}
	/* Check stops are well-formed (pos0 = 0 <= pos1 <= ... <= posN = 1) */
	if (positionStop[0] != 0) {
		fractal2D_werror("First gradient stop position should be 0.\n");
	}
	if (positionStop[nbStops-1] != 1) {
		fractal2D_werror("Last gradient stop position should be 1.\n");
	}
	for (uint_fast32_t i = 1; i < nbStops; ++i) {
		if (positionStop[i] <= positionStop[i-1]) {
			fractal2D_werror("Gradient stop positions should be (stricly) increasing.\n");
		}
	}

	GenerateGradient(gradient, nbStops, positionStop, colorStop, DEFAULT_GRADIENT_SIZE);

	end:
	return res;
}

int ReadGradientFileBody(Gradient *gradient, uint_fast8_t bytesPerComponent, const char *fileName,
			FILE *file, const char *format)
{
	fractal2D_message(stdout, T_VERBOSE, "Reading gradient file body...\n");
	int res = 0;
	if (strlen(format) != 4) {
		fractal2D_werror("Invalid gradient format '%s'.\n", format);
	}
	char formatStr[5];
	strcpy(formatStr, format);
	toLowerCase(formatStr);

	ReadGradientFileFunction readGradientFile;
	uint_fast32_t i;
	for (i = 0; i < nbGradientFormats; ++i) {
		if (strcmp(formatStr, gradientFormatStr[i]) == 0) {
			readGradientFile = (ReadGradientFileFunction)readGradientFileFunction[i];
			break;
		}
	}
	if (i == nbGradientFormats) {
		fractal2D_werror("Unsupported gradient format '%s'.\n", format);
	}

	res |= readGradientFile(gradient, bytesPerComponent, fileName, file);

	end:
	fractal2D_message(stdout, T_VERBOSE, "Reading gradient file body : %s.\n", (res == 0) ? "DONE" : "FAILED");

	return res;
}

int ReadGradientFile(Gradient *gradient, const char *fileName)
{
	fractal2D_message(stdout, T_NORMAL, "Reading gradient file...\n");

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
	uint_fast8_t bytesPerComponent;
	uint32_t bytesPerComponent32;
	if (readUint32(file, &bytesPerComponent32) < 1) {
		fractal2D_read_werror(fileName);
	}
	if (bytesPerComponent32 == 1 || bytesPerComponent32 == 2) {
		bytesPerComponent = (uint_fast8_t)bytesPerComponent32;
	} else {
		fractal2D_werror("Invalid gradient file : bytes per components must be 1 or 2.\n");
	}
	res = ReadGradientFileBody(gradient, bytesPerComponent, fileName, file, formatStr);

	end:
	if (file && fclose(file)) {
		fractal2D_close_errmsg(fileName);
		res = 1;
	}

	fractal2D_message(stdout, T_NORMAL, "Reading gradient file : %s.\n", (res == 0) ? "DONE" : "FAILED");

	return res;
}

