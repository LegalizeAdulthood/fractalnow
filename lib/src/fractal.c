/*
 *  fractal.c -- part of FractalNow
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
 
#include "fractal.h"
#include "error.h"
#include "file_io.h"
#include "filter.h"
#include "fractal_compute_engine.h"
#include "misc.h"
#include "uirectangle.h"
#include "thread.h"
#include <ctype.h>
#include <inttypes.h>
#include <pthread.h>
#include <string.h>

#define HandleRequests(max_counter) \
if (counter == max_counter) {\
	HandlePauseRequest(threadArgHeader);\
	cancelRequested =\
		CancelTaskRequested(threadArgHeader);\
	counter = 0;\
} else {\
	++counter;\
}

typedef struct s_DrawFractalArguments {
	uint_fast32_t threadId;
	FractalCache *cache;
	Image *image;
	Image *copyImage;
	const Fractal *fractal;
	const RenderingParameters *render;
	uint_fast32_t nbRectangles;
	UIRectangle *rectangles;
	uint_fast32_t size;
	double threshold;
	FloatPrecision floatPrecision;
} DrawFractalArguments;

void FreeDrawFractalArguments(void *arg)
{
	DrawFractalArguments *c_arg = (DrawFractalArguments *)arg;
	if (c_arg->threadId == 0) {
		free(c_arg->rectangles);
		if (c_arg->copyImage != NULL) {
			FreeImage(*c_arg->copyImage);
			free(c_arg->copyImage);
		}
	}
}

const char *fractalFormatStr[] = {
	(const char *)"f075"
};
const uint_fast32_t nbFractalFormats = sizeof(fractalFormatStr) / sizeof(const char *);

int  ReadFractalFileV075(Fractal *fractal, const char *fileName, FILE *file);
typedef int (*ReadFractalFileFunction)(Fractal *fractal, const char *fileName,
					FILE *file);
const ReadFractalFileFunction readFractalFileFunction[] = {
	ReadFractalFileV075
};

int  WriteFractalFileV075(const Fractal *fractal, const char *fileName, FILE *file);
typedef int (*WriteFractalFileFunction)(const Fractal *fractal, const char *fileName,
					FILE *file);
const WriteFractalFileFunction writeFractalFileFunction[] = {
	WriteFractalFileV075
};

void InitFractal(Fractal *fractal, FractalFormula fractalFormula, const mpc_t p,
			const mpc_t c, const mpfr_t centerX, const mpfr_t centerY,
			const mpfr_t spanX, const mpfr_t spanY,
			double escapeRadius, uint_fast32_t maxIter)
{
	cinitF(FP_MP, fractal->p);
	cinitF(FP_MP, fractal->c);
	initF(FP_MP, fractal->centerX);
	initF(FP_MP, fractal->centerY);
	initF(FP_MP, fractal->spanX);
	initF(FP_MP, fractal->spanY);
	initF(FP_MP, fractal->x1);
	initF(FP_MP, fractal->y1);

	fractal->fractalFormula = fractalFormula;

	if (fractalFormula == FRAC_MANDELBROT || fractalFormula == FRAC_JULIA) {
		cfromUiF(FP_MP, fractal->p, 2);
	} else {
		cassignF(FP_MP, fractal->p, p);
	}
	cassignF(FP_MP, fractal->c, c);
	fractal->escapeRadius = escapeRadius;
	fractal->maxIter = maxIter;

	assignF(FP_MP, fractal->centerX, centerX);
	assignF(FP_MP, fractal->centerY, centerY);
	assignF(FP_MP, fractal->spanX, spanX);
	assignF(FP_MP, fractal->spanY, spanY);

	mpfr_t tmp;
	initF(FP_MP, tmp);

	div_uiF(FP_MP, tmp, spanX, 2);
	subF(FP_MP, fractal->x1, centerX, tmp);

	div_uiF(FP_MP, tmp, spanY, 2);
	subF(FP_MP, fractal->y1, centerY, tmp);

	clearF(FP_MP, tmp);
}

void InitFractal2(Fractal *fractal, FractalFormula fractalFormula, long double complex p,
			long double complex c, long double centerX, long double centerY,
			long double spanX, long double spanY, double escapeRadius,
			uint_fast32_t maxIter)
{
	cinitF(FP_MP, fractal->p);
	cinitF(FP_MP, fractal->c);
	initF(FP_MP, fractal->centerX);
	initF(FP_MP, fractal->centerY);
	initF(FP_MP, fractal->spanX);
	initF(FP_MP, fractal->spanY);
	initF(FP_MP, fractal->x1);
	initF(FP_MP, fractal->y1);

	fractal->fractalFormula = fractalFormula;

	if (fractalFormula == FRAC_MANDELBROT || fractalFormula == FRAC_JULIA) {
		p = 2;
	}

	cfromCDoubleF(FP_MP, fractal->p, p);
	cfromCDoubleF(FP_MP, fractal->c, c);
	fractal->escapeRadius = escapeRadius;
	fractal->maxIter = maxIter;

	fromDoubleF(FP_MP, fractal->centerX, centerX);
	fromDoubleF(FP_MP, fractal->centerY, centerY);
	fromDoubleF(FP_MP, fractal->spanX, spanX);
	fromDoubleF(FP_MP, fractal->spanY, spanY);
	fromDoubleF(FP_MP, fractal->x1, centerX - spanX/2);
	fromDoubleF(FP_MP, fractal->y1, centerY - spanY/2);
}

Fractal CopyFractal(const Fractal *fractal)
{
	Fractal res;

	InitFractal(&res, fractal->fractalFormula, fractal->p, fractal->c,
			fractal->centerX, fractal->centerY,
			fractal->spanX, fractal->spanY,
			fractal->escapeRadius, fractal->maxIter);

	return res;
}

int ReadFractalFileV075(Fractal *fractal, const char *fileName, FILE *file)
{
	int res = 0;
	char tmp[6][256];
	mpc_t p;
	cinitF(FP_MP, p);
	mpc_t c;
	cinitF(FP_MP, c);
	mpfr_t centerX, centerY;
	initF(FP_MP, centerX);
	initF(FP_MP, centerY);
	mpfr_t spanX, spanY;
	initF(FP_MP, spanX);
	initF(FP_MP, spanY);

	FractalFormula fractalFormula;
	if (readString(file, tmp[0]) < 1) {
		FractalNow_read_werror(fileName);
	}
	if (GetFractalFormula(&fractalFormula, tmp[0])) {
		FractalNow_werror("Invalid fractal file : could not get fractal formula.\n");
	}
	switch (fractalFormula) {
	case FRAC_MANDELBROT:
	case FRAC_JULIA:
		fromUiF(FP_MP, mpc_realref(p), 2);
		fromUiF(FP_MP, mpc_imagref(p), 0);
		break;
	case FRAC_MULTIBROT:
	case FRAC_MULTIJULIA:
	case FRAC_BURNINGSHIP:
	case FRAC_JULIABURNINGSHIP:
	case FRAC_MANDELBAR:
	case FRAC_JULIABAR:
	case FRAC_RUDY:
		if (readMPFR(file, mpc_realref(p)) < 1) {
			FractalNow_read_werror(fileName);
		}
		if (readMPFR(file, mpc_imagref(p)) < 1) {
			FractalNow_read_werror(fileName);
		}
		break;
	default:
		FractalNow_werror("Unknown fractal formula \'%s\'.\n", tmp[0]);
		break;
	}

	if (cmp_uiF(FP_MP, mpc_realref(p), 0) < 0 || cmp_uiF(FP_MP, mpc_realref(p), 100) > 0) {
		FractalNow_werror("Invalid fractal file : Re(p) must be between 0 and 100.\n");
	}
	if (cmp_uiF(FP_MP, mpc_imagref(p), 0) < 0 || cmp_uiF(FP_MP, mpc_imagref(p), 100) > 0) {
		FractalNow_werror("Invalid fractal file : Im(p) must be between 0 and 100.\n");
	}
	cfromCDoubleF(FP_MP, c, 0.5+I*0.5);

	switch (fractalFormula) {
	case FRAC_MANDELBROT:
	case FRAC_BURNINGSHIP:
	case FRAC_MANDELBAR:
	case FRAC_MULTIBROT:
		break;
	case FRAC_JULIA:
	case FRAC_MULTIJULIA:
	case FRAC_JULIABURNINGSHIP:
	case FRAC_JULIABAR:
	case FRAC_RUDY:
		if (readMPFR(file, mpc_realref(c)) < 1) {
			FractalNow_read_werror(fileName);
		}
		if (readMPFR(file, mpc_imagref(c)) < 1) {
			FractalNow_read_werror(fileName);
		}
		break;
	default:
		FractalNow_werror("Unknown fractal formula \'%s\'.\n", tmp[0]);
		break;
	}

	if (readMPFR(file, centerX) < 1) {
		FractalNow_read_werror(fileName);
	}
	if (readMPFR(file, centerY) < 1) {
		FractalNow_read_werror(fileName);
	}
	if (readMPFR(file, spanX) < 1) {
		FractalNow_read_werror(fileName);
	}
	if (cmp_uiF(FP_MP, spanX, 0) <= 0) {
		FractalNow_werror("Invalid fractal file : spanX must be > 0.\n");
	}
	if (readMPFR(file, spanY) < 1) {
		FractalNow_read_werror(fileName);
	}
	if (cmp_uiF(FP_MP, spanY, 0) <= 0) {
		FractalNow_werror("Invalid fractal file : spanY must be > 0.\n");
	}
	double escapeRadius;
	if (readDouble(file, &escapeRadius) < 1) {
		FractalNow_read_werror(fileName);
	}
	if (escapeRadius <= 1.) {
		FractalNow_werror("Invalid config file : escape radius must be > 1.\n");
	}
	uint32_t nbIterMax;
	if (readUint32(file, &nbIterMax) < 1) {
		FractalNow_read_werror(fileName);
	}

	InitFractal(fractal, fractalFormula, p, c, centerX, centerY, spanX, spanY,
			escapeRadius, nbIterMax);
	end:

	cclearF(FP_MP, p);
	cclearF(FP_MP, c);
	clearF(FP_MP, centerX);
	clearF(FP_MP, centerY);
	clearF(FP_MP, spanX);
	clearF(FP_MP, spanY);

	return res;
}

ReadFractalFileFunction GetReadFractalFileFunction(const char *format)
{
	if (strlen(format) != 4) {
		return NULL;
	}

	ReadFractalFileFunction readFractalFile = NULL;
	char formatStr[5];
	strcpy(formatStr, format);
	toLowerCase(formatStr);

	uint_fast32_t i;
	for (i = 0; i < nbFractalFormats; ++i) {
		if (strcmp(formatStr, fractalFormatStr[i]) == 0) {
			readFractalFile = readFractalFileFunction[i];
			break;
		}
	}
	
	return readFractalFile;
}

int isSupportedFractalFile(const char *fileName)
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
		ReadFractalFileFunction readFractalFileFunction;
		readFractalFileFunction = GetReadFractalFileFunction(formatStr);
		res = (readFractalFileFunction == NULL);
	}

	end:
	if (file && fclose(file)) {
		FractalNow_close_errmsg(fileName);
		res = 1;
	}

	return !res;
}

int ReadFractalFileBody(Fractal *fractal, const char *fileName, FILE *file, const char *format)
{
	FractalNow_message(stdout, T_VERBOSE, "Reading fractal file body...\n");

	int res = 0;
	ReadFractalFileFunction readFractalFile = GetReadFractalFileFunction(format);
	if (readFractalFile == NULL) {
		FractalNow_werror("Unsupported fractal format '%s'.\n", format);
	}
	res |= readFractalFile(fractal, fileName, file);

	end:
	FractalNow_message(stdout, T_VERBOSE, "Reading fractal file body : %s.\n",
				(res == 0) ? "DONE" : "FAILED");

	return res;
}

int ReadFractalFile(Fractal *fractal, const char *fileName)
{
	FractalNow_message(stdout, T_NORMAL, "Reading fractal file...\n");

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
	res = ReadFractalFileBody(fractal, fileName, file, formatStr);

	end:
	if (file && fclose(file)) {
		FractalNow_close_errmsg(fileName);
		res = 1;
	}

	FractalNow_message(stdout, T_NORMAL, "Reading fractal file : %s.\n",
				(res == 0) ? "DONE" : "FAILED");

	return res;
}

WriteFractalFileFunction GetWriteFractalFileFunction(const char *format)
{
	if (strlen(format) != 4) {
		return NULL;
	}

	WriteFractalFileFunction writeFractalFile = NULL;
	char formatStr[5];
	strcpy(formatStr, format);
	toLowerCase(formatStr);

	uint_fast32_t i;
	for (i = 0; i < nbFractalFormats; ++i) {
		if (strcmp(formatStr, fractalFormatStr[i]) == 0) {
			writeFractalFile = writeFractalFileFunction[i];
			break;
		}
	}
	
	return writeFractalFile;
}

int WriteFractalFileV075(const Fractal *fractal, const char *fileName, FILE *file)
{
	int res = 0;

	const char *fractalFormula = fractalFormulaStr[(int)fractal->fractalFormula];
	if (writeString(file, fractalFormula, "\n") < 0) {
		FractalNow_write_werror(fileName);
	}
	switch (fractal->fractalFormula) {
	case FRAC_MANDELBROT:
	case FRAC_JULIA:
		break;
	case FRAC_MULTIBROT:
	case FRAC_MULTIJULIA:
	case FRAC_BURNINGSHIP:
	case FRAC_JULIABURNINGSHIP:
	case FRAC_MANDELBAR:
	case FRAC_JULIABAR:
	case FRAC_RUDY:
		if (writeMPFR(file, mpc_realref(fractal->p), " ") < 0) {
			FractalNow_write_werror(fileName);
		}
		if (writeMPFR(file, mpc_imagref(fractal->p), "\n") < 0) {
			FractalNow_write_werror(fileName);
		}
		break;
	default:
		FractalNow_werror("Unknown fractal formula value '%d'.\n",
			fractal->fractalFormula);
		break;
	}

	switch (fractal->fractalFormula) {
	case FRAC_MANDELBROT:
	case FRAC_MULTIBROT:
	case FRAC_BURNINGSHIP:
	case FRAC_MANDELBAR:
		break;
	case FRAC_JULIA:
	case FRAC_MULTIJULIA:
	case FRAC_JULIABURNINGSHIP:
	case FRAC_JULIABAR:
	case FRAC_RUDY:
		if (writeMPFR(file, mpc_realref(fractal->c), " ") < 0) {
			FractalNow_write_werror(fileName);
		}
		if (writeMPFR(file, mpc_imagref(fractal->c), "\n") < 0) {
			FractalNow_write_werror(fileName);
		}
		break;
	default:
		FractalNow_werror("Unknown fractal formula value '%d'.\n",
			fractal->fractalFormula);
		break;
	}

	if (writeMPFR(file, fractal->centerX, " ") < 0) {
		FractalNow_write_werror(fileName);
	}
	if (writeMPFR(file, fractal->centerY, " ") < 0) {
		FractalNow_write_werror(fileName);
	}
	if (writeMPFR(file, fractal->spanX, " ") < 0) {
		FractalNow_write_werror(fileName);
	}
	if (writeMPFR(file, fractal->spanY, "\n") < 0) {
		FractalNow_write_werror(fileName);
	}
	if (writeDouble(file, fractal->escapeRadius, " ") < 0) {
		FractalNow_write_werror(fileName);
	}
	if (writeUint32(file, fractal->maxIter, "\n") < 0) {
		FractalNow_write_werror(fileName);
	}

	end:

	return res;
}

int WriteFractalFileBody(const Fractal *fractal, const char *fileName, FILE *file, const char *format)
{
	FractalNow_message(stdout, T_VERBOSE, "Writing fractal file body...\n");

	int res = 0;
	WriteFractalFileFunction writeFractalFile = GetWriteFractalFileFunction(format);
	if (writeFractalFile == NULL) {
		FractalNow_werror("Unsupported fractal format '%s'.\n", format);
	}
	res |= writeFractalFile(fractal, fileName, file);

	end:
	FractalNow_message(stdout, T_VERBOSE, "Writing fractal file body : %s.\n",
				(res == 0) ? "DONE" : "FAILED");

	return res;
}

int WriteFractalFile(const Fractal *fractal, const char *fileName)
{
	FractalNow_message(stdout, T_NORMAL, "Writing fractal file...\n");

	int res = 0;
	FILE *file;

	file=fopen(fileName,"w");
	if (!file) {
		FractalNow_open_werror(fileName);
	}
	
	const char *format = fractalFormatStr[nbFractalFormats-1];
	fprintf(file, "%s\n", format);
	res = WriteFractalFileBody(fractal, fileName, file, format);

	end:
	if (file && fclose(file)) {
		FractalNow_close_errmsg(fileName);
		res = 1;
	}

	FractalNow_message(stdout, T_NORMAL, "Writing fractal file : %s.\n",
				(res == 0) ? "DONE" : "FAILED");

	return res;
}

static inline Color aux_ComputeFractalColor(const Fractal *fractal, const RenderingParameters *render,
						const FractalEngine *engine,
						uint_fast32_t x, uint_fast32_t y,
						uint_fast32_t width, uint_fast32_t height,
						FractalCache *cache)
{
	CacheEntry entry = RunFractalEngine(engine, fractal, render, x, y, width, height);
	double value = entry.value;

	Color res;

	if (cache != NULL) {
		AddToCacheThreadSafe(cache, entry);
	} else {
		FreeCacheEntry(entry);
	}
	if (value < 0) {
		res = render->spaceColor;
	} else {

		value = render->transferFunctionPtr(value)*render->realMultiplier+
			render->realOffset;
		res = GetGradientColor(&render->gradient, (uint_fast64_t)(value));
	}

	return res;
}

static inline Color aux_ComputeFractalImagePixel(const Fractal *fractal,
							const RenderingParameters *render,
							const FractalEngine *fractalEngine,
							uint_fast32_t x, uint_fast32_t y,
							uint_fast32_t width, uint_fast32_t height,
							FractalCache *cache)
{
	/* We call auxiliary function because we don't need (and thus want) to
	 * to re-get the FractalLoop to use for each pixel. It is already stored
	 * in arg.
	 */
	return aux_ComputeFractalColor(fractal, render, fractalEngine,
					x, y, width, height, cache);
}

static inline Color ComputeFractalImagePixel(const DrawFractalArguments *arg,
						const FractalEngine *engine,
						uint_fast32_t width, uint_fast32_t height,
						uint_fast32_t x, uint_fast32_t y,
						int useCache, FractalCache *cache)
{
	const Fractal *fractal = arg->fractal;
	const RenderingParameters *render = arg->render;

	Color res;
	if (useCache && cache != NULL) {
		ArrayValue aVal = GetArrayValue(cache, x, y);

		if (isArrayValueValid(aVal, cache)) {
			res = GetColorFromAVal(aVal, render);
		} else {
			res = aux_ComputeFractalImagePixel(fractal, render, engine, x, y,
								width, height, cache);
		}
	} else {
		res = aux_ComputeFractalImagePixel(fractal, render, engine, x, y,
							width, height, cache);
	}


	return res;
}

/* Compute (all) fractal values of given rectangle and render in image.
 */
static void aux1_DrawFractalThreadRoutine(ThreadArgHeader *threadArgHeader,
						const DrawFractalArguments *arg,
						const FractalEngine *engine)
{
	Image *image = arg->image;
	uint_fast32_t nbRectangles = arg->nbRectangles;
	FractalCache *cache = arg->cache;
	UIRectangle *rectangle;
	uint_fast32_t rectHeight;
	int cancelRequested = CancelTaskRequested(threadArgHeader);

	uint_fast32_t counter = 0;
	for (uint_fast32_t i = 0; i < nbRectangles && !cancelRequested; ++i) {
		rectangle = &arg->rectangles[i];
		rectHeight = rectangle->y2+1 - rectangle->y1;

		Color color;
		for (uint_fast32_t j=rectangle->y1; j<=rectangle->y2 && !cancelRequested; j++) {
			/* Updating progress after each line should be precise enough. */
			SetThreadProgress(threadArgHeader, 100 * (i * rectHeight + (j-rectangle->y1)) /
								(rectHeight * nbRectangles));
			for (uint_fast32_t k=rectangle->x1; k<=rectangle->x2 && !cancelRequested; k++) {
				HandleRequests(32);
				color = ComputeFractalImagePixel(arg, engine, image->width, image->height,
									k, j, 1, cache);
				PutPixelUnsafe(image,k,j,color);
			}
		}
	}
	SetThreadProgress(threadArgHeader, 100);
}

static inline int IsTooBigForInterpolation(const UIRectangle *rectangle,
						uint_fast32_t quadInterpolationSize)
{
	return ((rectangle->x2-rectangle->x1+1 > quadInterpolationSize)
		|| (rectangle->y2-rectangle->y1+1 > quadInterpolationSize));
}

static inline int_fast8_t GetCornerIndex(const UIRectangle *rectangle, uint_fast32_t x, uint_fast32_t y)
{
	if ((x == rectangle->x1 && y == rectangle->y1)) {
		return 0;
	} else if (x == rectangle->x2 && y == rectangle->y1) {
		return 1;
	} else if (x == rectangle->x1 && y == rectangle->y2) {
		return 2;
	} else if (x == rectangle->x2 && y == rectangle->y2) {
		return 3;
	} else {
		return -1;
	}
}

/* Compute fractal values of given rectangle into fractal_table, according to
   its dissimilarity and the given dissimilarity threshold (i.e. either
   computes it really, or interpolate linearly from the corners), and render
   in image.
 */
static inline void aux2_DrawFractalThreadRoutine(const DrawFractalArguments *arg, const FractalEngine *engine,
							const UIRectangle *rectangle)
{
	Image *image = arg->image;
	uint_fast32_t width = image->width;
	uint_fast32_t height = image->height;
	double interpolationThreshold = arg->threshold;
	FractalCache *cache = arg->cache;

	Color corner[4];
	if (rectangle->x1 == rectangle->x2 && rectangle->y1 == rectangle->y2) {
		/* Rectangle is just one pixel.*/
		corner[0] = ComputeFractalImagePixel(arg,engine,width,height,rectangle->x1,rectangle->y1,1,
							cache);
		PutPixelUnsafe(image,rectangle->x1,rectangle->y1,corner[0]);
		return;
	} else if (rectangle->x1 == rectangle->x2) {
		/* Rectangle is a vertical line.
		   There are only two "corners".
		*/
		corner[0] = ComputeFractalImagePixel(arg,engine,width,height,rectangle->x1,rectangle->y1,1,
							cache);
		corner[1] = corner[0];
		corner[2] = ComputeFractalImagePixel(arg,engine,width,height,rectangle->x1,rectangle->y2,1,
							cache);
		corner[3] = corner[2];
		/* Even for a line, we can still use quad interpolation.*/
	} else if (rectangle->y1 == rectangle->y2) {
		/* Rectangle is a horizontal line.
		   There are only two "corners".
		*/
		corner[0] = ComputeFractalImagePixel(arg,engine,width,height,rectangle->x1,rectangle->y1,1,
							cache);
		corner[1] = ComputeFractalImagePixel(arg,engine,width,height,rectangle->x2,rectangle->y1,1,
							cache);
		corner[2] = corner[0];
		corner[3] = corner[1];
		/* Even for a line, we can still use quad interpolation.*/
	} else {
		/* "Real" rectangle. Compute four corners. */
		corner[0] = ComputeFractalImagePixel(arg,engine,width,height,rectangle->x1,rectangle->y1,1,
							cache);
		corner[1] = ComputeFractalImagePixel(arg,engine,width,height,rectangle->x2,rectangle->y1,1,
							cache);
		corner[2] = ComputeFractalImagePixel(arg,engine,width,height,rectangle->x1,rectangle->y2,1,
							cache);
		corner[3] = ComputeFractalImagePixel(arg,engine,width,height,rectangle->x2,rectangle->y2,1
							,cache);
	}

	Color color;
	int_fast8_t index = -1;
	if (QuadAvgDissimilarity(corner) < interpolationThreshold) {
		/* Linear interpolation */
		double sx = (double)rectangle->x2-rectangle->x1+1;
		double sy = (double)rectangle->y2-rectangle->y1+1;
		double x, y;
		for (uint_fast32_t i=rectangle->y1; i<=rectangle->y2; i++) {
			y = ((double)(i-rectangle->y1)) / sy;
			for (uint_fast32_t j=rectangle->x1; j<=rectangle->x2; j++) {
				index = GetCornerIndex(rectangle, j, i);
				if (index >= 0) {
					color = corner[index];
				} else {
					x = ((double)(j-rectangle->x1)) / sx;
					color = QuadLinearInterpolation(corner,x,y);
				}

				PutPixelUnsafe(image,j,i,color);
			}
		}
	} else {
		/* Real computation */
		for (uint_fast32_t i=rectangle->y1; i<=rectangle->y2; i++) {
			for (uint_fast32_t j=rectangle->x1; j<=rectangle->x2; j++) {
				index = GetCornerIndex(rectangle, j, i);
				if (index >= 0) {
					color = corner[index];
				} else {
					color = ComputeFractalImagePixel(arg,engine,width,height,j,i,1,
										cache);
				}

				PutPixelUnsafe(image,j,i,color);
			}
		}
	}
}

void *DrawFractalThreadRoutine(void *arg)
{
	ThreadArgHeader *threadArgHeader = GetThreadArgHeader(arg);
	DrawFractalArguments *c_arg = (DrawFractalArguments *)GetThreadArgBody(arg);
	FractalEngine engine;
	CreateFractalEngine(&engine, c_arg->fractal, c_arg->render, c_arg->floatPrecision);

	if (c_arg->size == 1) {
		aux1_DrawFractalThreadRoutine(threadArgHeader, c_arg, &engine);
	} else {
		UIRectangle *currentRect;
		int cancelRequested = CancelTaskRequested(threadArgHeader);
		uint_fast32_t counter = 0;

		for (uint_fast32_t i = 0; i < c_arg->nbRectangles && !cancelRequested; ++i) {
			currentRect = &c_arg->rectangles[i];
			/* Cut rectangle into smaller rectangles, so that
			   all rectangles are smaller than quadInterpolationSize.
			 */
			UIRectangle *rectangle;
			uint_fast32_t nbRectangles;
			CutUIRectangleMaxSize(*currentRect, c_arg->size, &rectangle, &nbRectangles);

			/* If the rectangle dissimilarity is greater that threshold,
			   we compute the fractal colors, otherwise we interpolate
			   them using the corner colors.
			   Check for cancellation after each rectangle is drawn.
			   There should be very small latency because rectangles
			   are fairly small (default size is 5*5 pixels).
			   If this results in too much latency when cancelling,
			   we might want to make this check in
			   aux2_DrawFractalThreadRoutine, after each scanline
			   for example.
			 */
			for (uint_fast32_t j = 0; j < nbRectangles && !cancelRequested; ++j) {
				/* Updating after each rectangle should be precise enough. */
				SetThreadProgress(threadArgHeader, 100 * (i * nbRectangles + j) /
								(nbRectangles * c_arg->nbRectangles));
				HandleRequests(0);

				aux2_DrawFractalThreadRoutine(c_arg, &engine, &rectangle[j]);
			}
			free(rectangle);
		}
		SetThreadProgress(threadArgHeader, 100);
	}

	FreeFractalEngine(&engine);

	int canceled = CancelTaskRequested(threadArgHeader);

	return (canceled ? PTHREAD_CANCELED : NULL);
}

char drawFractalMessage[] = "Drawing fractal";

Task *aux_CreateDrawFractalTask(Image *image, const Fractal *fractal, const RenderingParameters *render,
				uint_fast32_t quadInterpolationSize, double interpolationThreshold,
				FloatPrecision floatPrecision, FractalCache *cache,
				uint_fast32_t nbThreads)
{
	if (image->width < 2 || image->height < 2) {
		return DoNothingTask();
	}
	if (quadInterpolationSize == 0) {
		quadInterpolationSize = 1;
	}

	uint_fast32_t nbPixels = image->width*image->height;
	uint_fast32_t nbThreadsNeeded = nbThreads;
	uint_fast32_t rectanglesPerThread = DEFAULT_RECTANGLES_PER_THREAD;
	if (nbPixels <= nbThreadsNeeded) {
		nbThreadsNeeded = nbPixels;
		rectanglesPerThread = 1;
	} else if (nbPixels < nbThreadsNeeded*rectanglesPerThread) {
		rectanglesPerThread = nbPixels / nbThreadsNeeded;
	}
	uint_fast32_t nbRectangles = nbThreadsNeeded*rectanglesPerThread;

	UIRectangle *rectangle;
	rectangle = (UIRectangle *)safeMalloc("rectangles", nbRectangles * sizeof(UIRectangle));
	InitUIRectangle(&rectangle[0], 0, 0, image->width-1, image->height-1);
	if (CutUIRectangleInN(rectangle[0], nbRectangles, rectangle)) {
		FractalNow_error("Could not cut rectangle ((%"PRIuFAST32",%"PRIuFAST32"),\
(%"PRIuFAST32",%"PRIuFAST32") in %"PRIuFAST32" parts.\n", rectangle[0].x1, rectangle[0].y1,
			rectangle[0].x2, rectangle[0].y2, nbRectangles);
	}
	
	DrawFractalArguments *arg;
	arg = (DrawFractalArguments *)safeMalloc("arguments", nbThreadsNeeded *
							sizeof(DrawFractalArguments));

	for (uint_fast32_t i = 0; i < nbThreadsNeeded; ++i) {
		arg[i].threadId = i;
		arg[i].cache = cache;
		arg[i].image = image;
		arg[i].copyImage = NULL;
		arg[i].fractal = fractal;
		arg[i].render = render;
		arg[i].floatPrecision = floatPrecision;

		arg[i].rectangles = &rectangle[i*rectanglesPerThread];
		arg[i].nbRectangles = rectanglesPerThread;
		arg[i].size = quadInterpolationSize;
		arg[i].threshold = interpolationThreshold;
	}
	Task *task = CreateTask(drawFractalMessage, nbThreadsNeeded, arg, 
					sizeof(DrawFractalArguments), DrawFractalThreadRoutine,
					FreeDrawFractalArguments);

	free(arg);

	return task;
}

inline Task *CreateDrawFractalTask(Image *image, const Fractal *fractal, const RenderingParameters *render,
				uint_fast32_t quadInterpolationSize, double interpolationThreshold,
				FloatPrecision floatPrecision, FractalCache *cache,
				uint_fast32_t nbThreads)
{
	Task *res;
	if (cache == NULL) {
		res = aux_CreateDrawFractalTask(image, fractal, render, quadInterpolationSize,
				interpolationThreshold, floatPrecision, cache, nbThreads);
	} else {
		/* Create preview image from cache first. */
		Task *subTasks[2];
		subTasks[0] = CreateFractalCachePreviewTask(image, cache, fractal, render, 1, nbThreads);
		subTasks[1] = aux_CreateDrawFractalTask(image, fractal, render, quadInterpolationSize,
						interpolationThreshold, floatPrecision, cache, nbThreads);

		res = CreateCompositeTask(NULL, 2, subTasks);
	}
	return res;
}

void DrawFractal(Image *image, const Fractal *fractal, const RenderingParameters *render,
			uint_fast32_t quadInterpolationSize, double interpolationThreshold,
			FloatPrecision floatPrecision, FractalCache *cache, Threads *threads)
{
	Task *task = CreateDrawFractalTask(image, fractal, render, quadInterpolationSize,
				interpolationThreshold, floatPrecision, cache, threads->N);
	int unused = ExecuteTaskBlocking(task, threads);
	UNUSED(unused);
}

void *AntiAliaseFractalThreadRoutine(void *arg)
{
	ThreadArgHeader *threadArgHeader = GetThreadArgHeader(arg);
	DrawFractalArguments *c_arg = (DrawFractalArguments *)GetThreadArgBody(arg);
	Image *image = c_arg->image;
	const Image *copyImage = c_arg->copyImage;
	uint_fast32_t antialiasingSize = c_arg->size;
	double threshold = c_arg->threshold;
	FractalCache *cache = c_arg->cache;
	uint_fast32_t width = image->width;
	uint_fast32_t height = image->height;
	FractalEngine engine;
	CreateFractalEngine(&engine, c_arg->fractal, c_arg->render, c_arg->floatPrecision);

	Image tmpImage1, tmpImage2;
	CreateImage(&tmpImage1, antialiasingSize, antialiasingSize, image->bytesPerComponent);
	CreateImage(&tmpImage2, 1, antialiasingSize, image->bytesPerComponent);

	Filter horizontalGaussianFilter;
	Filter verticalGaussianFilter;
	CreateHorizontalGaussianFilter2(&horizontalGaussianFilter, antialiasingSize);
	CreateVerticalGaussianFilter2(&verticalGaussianFilter, antialiasingSize);

	uint_fast32_t center = (antialiasingSize-1) / 2;
	uint_fast32_t bigWidth = width * antialiasingSize;
	uint_fast32_t bigHeight = height * antialiasingSize;
	Color C[9], c;
	double max = 0;

	UIRectangle *rectangle;
	uint_fast32_t counter = 0;
	int cancelRequested = CancelTaskRequested(threadArgHeader);
	for (uint_fast32_t i = 0; i < c_arg->nbRectangles && !cancelRequested; ++i) {
		rectangle = &c_arg->rectangles[i];
		uint_fast32_t rectHeight = rectangle->y2+1 - rectangle->y1;

		uint_fast32_t y = rectangle->y1 * antialiasingSize;
		for (uint_fast32_t j = rectangle->y1; j <= rectangle->y2
				&& !cancelRequested; ++j, y+=antialiasingSize) {
			SetThreadProgress(threadArgHeader, 100 * (i * rectHeight + (j-rectangle->y1)) /
								(rectHeight * c_arg->nbRectangles));
			uint_fast32_t x = rectangle->x1 * antialiasingSize;
			for (uint_fast32_t k = rectangle->x1; k <= rectangle->x2
					&& !cancelRequested; ++k, x+=antialiasingSize) {

				C[0]  = iGetPixel(copyImage, k, j);
				C[1]  = iGetPixel(copyImage, k-1, j-1);
				C[2]  = iGetPixel(copyImage, k, j-1);
				C[3]  = iGetPixel(copyImage, k+1, j-1);
				C[4]  = iGetPixel(copyImage, k-1, j);
				C[5]  = iGetPixel(copyImage, k+1, j);
				C[6]  = iGetPixel(copyImage, k-1, j+1);
				C[7]  = iGetPixel(copyImage, k, j+1);
				C[8]  = iGetPixel(copyImage, k+1, j+1);

				max = 0;
				for (int_fast8_t l = 1; l < 9; ++l) {
					max = fmaxl(max, ColorManhattanDistance(C[0], C[l]));
				}

				if (max > threshold) {
					for (uint_fast32_t l=0; l<antialiasingSize; ++l) {
						for (uint_fast32_t m=0; m<antialiasingSize; ++m) {
							HandleRequests(32);
							c = ComputeFractalImagePixel(c_arg, &engine,
											bigWidth, bigHeight,
											x+m, y+l, 0, cache);
							PutPixelUnsafe(&tmpImage1, m, l, c);
						}
					}

					for (uint_fast32_t l = 0; l < antialiasingSize; ++l) {
						c = ApplyFilterOnSinglePixel(&tmpImage1, center, l,
										&horizontalGaussianFilter);
						PutPixelUnsafe(&tmpImage2, 0, l, c);
					}
					c = ApplyFilterOnSinglePixel(&tmpImage2, 0, center,
									&verticalGaussianFilter);

					PutPixelUnsafe(image, k, j, c);
				} else {
					HandleRequests(32);
				}
			}
		}
	}
	SetThreadProgress(threadArgHeader, 100);

	FreeFilter(horizontalGaussianFilter);
	FreeFilter(verticalGaussianFilter);
	FreeImage(tmpImage1);
	FreeImage(tmpImage2);
	FreeFractalEngine(&engine);

	int canceled = CancelTaskRequested(threadArgHeader);

	return (canceled ? PTHREAD_CANCELED : NULL);
}

char antiAliaseFractalMessage[] = "Anti-aliasing fractal";

Task *CreateAntiAliaseFractalTask(Image *image, const Fractal *fractal,
					const RenderingParameters *render, uint_fast32_t antiAliasingSize,
					double threshold, FloatPrecision floatPrecision,
					FractalCache *cache, uint_fast32_t nbThreads)
{
	if (antiAliasingSize == 0) {
		return DoNothingTask();
	}
	if (image->width*antiAliasingSize < 2 || image->height*antiAliasingSize < 2) {
		return DoNothingTask();
	}
	uint_fast32_t nbPixels = image->width*image->height;
	uint_fast32_t nbThreadsNeeded = nbThreads;
	uint_fast32_t rectanglesPerThread = DEFAULT_RECTANGLES_PER_THREAD;
	if (nbPixels <= nbThreadsNeeded) {
		nbThreadsNeeded = nbPixels;
		rectanglesPerThread = 1;
	} else if (nbPixels < nbThreadsNeeded*rectanglesPerThread) {
		rectanglesPerThread = nbPixels / nbThreadsNeeded;
	}
	uint_fast32_t nbRectangles = nbThreadsNeeded*rectanglesPerThread;

	UIRectangle *rectangle;
	rectangle = (UIRectangle *)safeMalloc("rectangles", nbRectangles * sizeof(UIRectangle));
	InitUIRectangle(&rectangle[0], 0, 0, image->width-1, image->height-1);
	if (CutUIRectangleInN(rectangle[0], nbRectangles, rectangle)) {
		FractalNow_error("Could not cut rectangle ((%"PRIuFAST32",%"PRIuFAST32"),\
(%"PRIuFAST32",%"PRIuFAST32") in %"PRIuFAST32" parts.\n", rectangle[0].x1, rectangle[0].y1,
			rectangle[0].x2, rectangle[0].y2, nbRectangles);
	}
	
	DrawFractalArguments *arg;
	arg = (DrawFractalArguments *)safeMalloc("arguments", nbThreadsNeeded*sizeof(DrawFractalArguments));

	Image *copyImage = (Image *)safeMalloc("copyImage", sizeof(Image));
	*copyImage = CloneImage(image);
	for (uint_fast32_t i = 0; i < nbThreadsNeeded; ++i) {
		arg[i].threadId = i;
		/* No copy for image.
		 * Concurrent read is OK.
		 */
		arg[i].image = image;
		arg[i].copyImage = copyImage;
		arg[i].cache = cache;
		/* Fractal is not copied because it is not modified.*/
		arg[i].fractal = fractal;
		/* Rendering parameters are not modified.*/
		arg[i].render = render;
		arg[i].floatPrecision = floatPrecision;

		arg[i].rectangles = &rectangle[i*rectanglesPerThread];
		arg[i].nbRectangles = rectanglesPerThread;
		arg[i].size = antiAliasingSize;
		arg[i].threshold = threshold;
	}
	Task *res = CreateTask(antiAliaseFractalMessage, nbThreadsNeeded, arg,
					sizeof(DrawFractalArguments), AntiAliaseFractalThreadRoutine,
					FreeDrawFractalArguments);

	free(arg);

	return res;
}

void AntiAliaseFractal(Image *image, const Fractal *fractal, const RenderingParameters *render,
			uint_fast32_t antiAliasingSize, double threshold,
			FloatPrecision floatPrecision, FractalCache *cache, Threads *threads)
{
	Task *task = CreateAntiAliaseFractalTask(image, fractal, render, antiAliasingSize,
						threshold, floatPrecision, cache, threads->N);
	int unused = ExecuteTaskBlocking(task, threads);
	UNUSED(unused);
}

void FreeFractal(Fractal fractal)
{
	cclearF(FP_MP, fractal.p);
	cclearF(FP_MP, fractal.c);
	clearF(FP_MP, fractal.centerX);
	clearF(FP_MP, fractal.centerY);
	clearF(FP_MP, fractal.spanX);
	clearF(FP_MP, fractal.spanY);
	clearF(FP_MP, fractal.x1);
	clearF(FP_MP, fractal.y1);
}

