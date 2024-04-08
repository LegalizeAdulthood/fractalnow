/*
 *  fractal.c -- part of fractal2D
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
#include "file_parsing.h"
#include "filter.h"
#include "fractal_loop.h"
#include "misc.h"
#include "rectangle.h"
#include "thread.h"
#include <ctype.h>
#include <inttypes.h>
#include <pthread.h>
#include <signal.h>
#include <string.h>

typedef struct s_DrawFractalArguments {
	Image *image;
	Image *copyImage;
	const Fractal *fractal;
	const RenderingParameters *render;
	FractalLoop fractalLoop;
	Rectangle *clipRect;
	uint_fast32_t size;
	FLOAT threshold;
} DrawFractalArguments;

void FreeDrawFractalArguments(void *arg)
{
	DrawFractalArguments *c_arg = (DrawFractalArguments *)arg;
	free(c_arg->clipRect);
	if (c_arg->copyImage != NULL) {
		FreeImage(*c_arg->copyImage);
		free(c_arg->copyImage);
	}
}

const char *fractalFormatStr[] = {
	(const char *)"f072"
};

int  ReadFractalFileV072(Fractal *fractal, const char *fileName, FILE *file);
typedef int (*ReadFractalFileFunction)(Fractal *fractal, const char *fileName,
					FILE *file);

const ReadFractalFileFunction readFractalFileFunction[] = {
	ReadFractalFileV072,
};
const uint_fast32_t nbFractalFormats = sizeof(fractalFormatStr) / sizeof(const char *);

static inline void aux_InitFractal(Fractal *fractal, FractalFormula fractalFormula, FLOAT p,
				FLOAT complex c, FLOAT escapeRadius, uint_fast32_t maxIter)
{
	fractal->fractalFormula = fractalFormula;
	if (fractalFormula == FRAC_MANDELBROT || fractalFormula == FRAC_JULIA) {
		p = 2;
	}
	fractal->p = p;
	fractal->p_is_integer = isInteger(p);
	fractal->p_int = (uint_fast8_t)p;
	fractal->logP = logF(p);
	fractal->c = c;
	fractal->escapeRadius = escapeRadius;
	fractal->escapeRadius2 = escapeRadius*escapeRadius;
	fractal->escapeRadiusP = powF(escapeRadius,p);
	fractal->logEscapeRadius = logF(escapeRadius);
	fractal->maxIter = maxIter;
}

void InitFractal(Fractal *fractal, FractalFormula fractalFormula, FLOAT p, FLOAT complex c,
		FLOAT x1, FLOAT y1, FLOAT x2, FLOAT y2, FLOAT escapeRadius,
		uint_fast32_t maxIter)
{
	fractal->x1 = x1;
	fractal->x2 = x2;
	fractal->y1 = y1;
	fractal->y2 = y2;
	fractal->centerX = (x1+x2) / 2;
	fractal->centerY = (y1+y2) / 2;
	fractal->spanX = x2-x1;
	fractal->spanY = y2-y1;

	aux_InitFractal(fractal, fractalFormula, p, c, escapeRadius, maxIter);
}

void InitFractal2(Fractal *fractal, FractalFormula fractalFormula, FLOAT p, FLOAT complex c,
		FLOAT centerX, FLOAT centerY, FLOAT spanX, FLOAT spanY,
		FLOAT escapeRadius, uint_fast32_t maxIter)
{
	fractal->x1 = centerX-spanX/2;
	fractal->x2 = centerX+spanX/2;
	fractal->y1 = centerY-spanY/2;
	fractal->y2 = centerY+spanY/2;
	fractal->centerX = centerX;
	fractal->centerY = centerY;
	fractal->spanX = spanX;
	fractal->spanY = spanY;

	aux_InitFractal(fractal, fractalFormula, p, c, escapeRadius, maxIter);
}

Fractal CopyFractal(const Fractal *fractal)
{
	Fractal res = *fractal;
	return res;
}

int ReadFractalFileV072(Fractal *fractal, const char *fileName, FILE *file)
{
	int res = 0;
	char tmp[6][256];

	FractalFormula fractalFormula;
	if (readString(file, tmp[0]) < 1) {
		fractal2D_read_werror(fileName);
	}
	if (GetFractalFormula(&fractalFormula, tmp[0])) {
		fractal2D_werror("Invalid fractal file : could not get fractal formula.\n");
	}
	FLOAT p;
	switch (fractalFormula) {
	case FRAC_MANDELBROT:
	case FRAC_JULIA:
		p = 2;
		break;
	case FRAC_MANDELBROTP:
	case FRAC_JULIAP:
	case FRAC_RUDY:
		if (readFLOAT(file, &p) < 1) {
			fractal2D_read_werror(fileName);
		}
		break;
	default:
		fractal2D_werror("Unknown fractal type \'%s\'.\n", tmp[0]);
		break;
	}

	if (p < 0 || p > 100) {
		fractal2D_werror("Invalid fractal file : p must be between 0 and 100.\n");
	}
	FLOAT cx = 0.5, cy = 0.5;

	switch (fractalFormula) {
	case FRAC_MANDELBROT:
	case FRAC_MANDELBROTP:
		break;
	case FRAC_JULIA:
	case FRAC_JULIAP:
	case FRAC_RUDY:
		if (readFLOAT(file, &cx) < 1) {
			fractal2D_read_werror(fileName);
		}
		if (readFLOAT(file, &cy) < 1) {
			fractal2D_read_werror(fileName);
		}
		break;
	default:
		fractal2D_werror("Unknown fractal type \'%s\'.\n", tmp[0]);
		break;
	}

	FLOAT centerX, centerY;
	if (readFLOAT(file, &centerX) < 1) {
		fractal2D_read_werror(fileName);
	}
	if (readFLOAT(file, &centerY) < 1) {
		fractal2D_read_werror(fileName);
	}
	FLOAT spanX, spanY;
	if (readFLOAT(file, &spanX) < 1) {
		fractal2D_read_werror(fileName);
	}
	if (spanX <= 0) {
		fractal2D_werror("Invalid fractal file : spanX must be > 0.\n");
	}
	if (readFLOAT(file, &spanY) < 1) {
		fractal2D_read_werror(fileName);
	}
	if (spanY <= 0) {
		fractal2D_werror("Invalid fractal file : spanY must be > 0.\n");
	}
	FLOAT escapeRadius;
	if (readFLOAT(file, &escapeRadius) < 1) {
		fractal2D_read_werror(fileName);
	}
	if (escapeRadius <= 1.) {
		fractal2D_werror("Invalid config file : escape radius must be > 1.\n");
	}
	uint32_t nbIterMax;
	if (readUint32(file, &nbIterMax) < 1) {
		fractal2D_read_werror(fileName);
	}

	InitFractal2(fractal, fractalFormula, p, cx+I*cy, centerX, centerY, spanX, spanY,
			escapeRadius, nbIterMax);

	end:

	return res;
}

int ReadFractalFileBody(Fractal *fractal, const char *fileName, FILE *file, const char *format)
{
	fractal2D_message(stdout, T_VERBOSE, "Reading fractal file body...\n");
	int res = 0;
	if (strlen(format) != 4) {
		fractal2D_werror("Invalid fractal format '%s'.\n", format);
	}
	char formatStr[5];
	strcpy(formatStr, format);
	toLowerCase(formatStr);

	ReadFractalFileFunction readFractalFile;
	uint_fast32_t i;
	for (i = 0; i < nbFractalFormats; ++i) {
		if (strcmp(formatStr, fractalFormatStr[i]) == 0) {
			readFractalFile = (ReadFractalFileFunction)readFractalFileFunction[i];
			break;
		}
	}
	if (i == nbFractalFormats) {
		fractal2D_werror("Unsupported fractal format '%s'.\n", format);
	}

	res |= readFractalFile(fractal, fileName, file);

	end:
	fractal2D_message(stdout, T_VERBOSE, "Reading fractal file body : %s.\n", (res == 0) ? "DONE" : "FAILED");

	return res;
}

int ReadFractalFile(Fractal *fractal, const char *fileName)
{
	fractal2D_message(stdout, T_NORMAL, "Reading fractal file...\n");

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
	res = ReadFractalFileBody(fractal, fileName, file, formatStr);

	end:
	if (file && fclose(file)) {
		fractal2D_close_errmsg(fileName);
		res = 1;
	}

	fractal2D_message(stdout, T_NORMAL, "Reading fractal file : %s.\n", (res == 0) ? "DONE" : "FAILED");

	return res;
}

static inline Color aux_ComputeFractalColor(const Fractal *fractal, const RenderingParameters *render,
						FractalLoop fractalLoop, FLOAT complex z)
{
	FLOAT value = fractalLoop(fractal, render, z);
	Color res;
	if (value < 0) {
		res = render->spaceColor;
	} else {
		value = render->transferFunctionPtr(value)*render->realMultiplier+
			render->realOffset;
		res = GetGradientColor(&render->gradient, (uint_fast64_t)(value));
	}
	return res;
}

Color ComputeFractalColor(const Fractal *fractal, const RenderingParameters *render, FLOAT complex z)
{
	FractalLoop fractalLoop = GetFractalLoop(fractal->fractalFormula, fractal->p_is_integer,
					render->coloringMethod, render->addendFunction,
					render->interpolationMethod);
	return aux_ComputeFractalColor(fractal, render, fractalLoop, z);
}

static inline Color ComputeFractalImagePixel(const DrawFractalArguments *arg,
						uint_fast32_t width, uint_fast32_t height,
						uint_fast32_t x, uint_fast32_t y)
{
	const Fractal *fractal = arg->fractal;
	const RenderingParameters *render = arg->render;
	FractalLoop fractalLoop = arg->fractalLoop;

	FLOAT fx = fractal->x1 + (x+0.5) * fractal->spanX / width;
	FLOAT fy = fractal->y1 + (y+0.5) * fractal->spanY / height;

	/* We call auxiliary function because we don't need (and thus want) to
	 * to re-get the FractalLoop to use for each pixel. It is already stored
	 * in arg.
	 */
	return aux_ComputeFractalColor(fractal, render, fractalLoop, fx+I*fy);
}

/* Compute (all) fractal values of given rectangle and render in image.
 */
static void aux1_DrawFractalThreadRoutine(ThreadArgHeader *threadArgHeader,
						const DrawFractalArguments *arg,
						const Rectangle *rectangle)
{
	volatile sig_atomic_t *cancel = threadArgHeader->cancel;
	Image *image = arg->image;
	uint_fast32_t rectHeight = rectangle->y2-rectangle->y1+1;

	Color color;
	for (uint_fast32_t i=rectangle->y1; i<=rectangle->y2 && !(*cancel); i++) {
		for (uint_fast32_t j=rectangle->x1; j<=rectangle->x2 && !(*cancel); j++) {
			color = ComputeFractalImagePixel(arg, image->width, image->height, j, i);
			PutPixelUnsafe(image,j,i,color);
		}
		/* Updating progress after each row should be precise enough. */
		threadArgHeader->progress = 100 * (i-rectangle->y1) / rectHeight;
	}
	threadArgHeader->progress = 1;
}

static inline int IsTooBigForInterpolation(const Rectangle *rectangle,
						uint_fast32_t quadInterpolationSize)
{
	return ((rectangle->x2-rectangle->x1+1 > quadInterpolationSize)
		|| (rectangle->y2-rectangle->y1+1 > quadInterpolationSize));
}

static inline int_fast8_t GetCornerIndex(const Rectangle *rectangle, uint_fast32_t x, uint_fast32_t y)
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
static inline void aux2_DrawFractalThreadRoutine(const DrawFractalArguments *arg, const Rectangle *rectangle)
{
	Image *image = arg->image;
	uint_fast32_t width = image->width;
	uint_fast32_t height = image->height;
	FLOAT interpolationThreshold = arg->threshold;

	Color corner[4];
	if (rectangle->x1 == rectangle->x2 && rectangle->y1 == rectangle->y2) {
		/* Rectangle is just one pixel.*/
		corner[0] = ComputeFractalImagePixel(arg,width,height,rectangle->x1,rectangle->y1);
		PutPixelUnsafe(image,rectangle->x1,rectangle->y1,corner[0]);
		return;
	} else if (rectangle->x1 == rectangle->x2) {
		/* Rectangle is a vertical line.
		   There are only two "corners".
		*/
		corner[0] = ComputeFractalImagePixel(arg,width,height,rectangle->x1,rectangle->y1);
		corner[1] = corner[0];
		corner[2] = ComputeFractalImagePixel(arg,width,height,rectangle->x1,rectangle->y2);
		corner[3] = corner[2];
		/* Even for a line, we can still use quad interpolation.*/
	} else if (rectangle->y1 == rectangle->y2) {
		/* Rectangle is a horizontal line.
		   There are only two "corners".
		*/
		corner[0] = ComputeFractalImagePixel(arg,width,height,rectangle->x1,rectangle->y1);
		corner[1] = ComputeFractalImagePixel(arg,width,height,rectangle->x2,rectangle->y1);
		corner[2] = corner[0];
		corner[3] = corner[1];
		/* Even for a line, we can still use quad interpolation.*/
	} else {
		/* "Real" rectangle. Compute four corners. */
		corner[0] = ComputeFractalImagePixel(arg,width,height,rectangle->x1,rectangle->y1);
		corner[1] = ComputeFractalImagePixel(arg,width,height,rectangle->x2,rectangle->y1);
		corner[2] = ComputeFractalImagePixel(arg,width,height,rectangle->x1,rectangle->y2);
		corner[3] = ComputeFractalImagePixel(arg,width,height,rectangle->x2,rectangle->y2);
	}

	Color color;
	int_fast8_t index = -1;
	if (QuadAvgDissimilarity(corner) < interpolationThreshold) {
		/* Linear interpolation */
		FLOAT sx = (FLOAT)rectangle->x2-rectangle->x1+1;
		FLOAT sy = (FLOAT)rectangle->y2-rectangle->y1+1;
		FLOAT x, y;
		for (uint_fast32_t i=rectangle->y1; i<=rectangle->y2; i++) {
			y = ((FLOAT)(i-rectangle->y1)) / sy;
			for (uint_fast32_t j=rectangle->x1; j<=rectangle->x2; j++) {
				index = GetCornerIndex(rectangle, j, i);
				if (index >= 0) {
					color = corner[index];
				} else {
					x = ((FLOAT)(j-rectangle->x1)) / sx;
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
					color = ComputeFractalImagePixel(arg,width,height,j,i);
				}

				PutPixelUnsafe(image,j,i,color);
			}
		}
	}
}

void *DrawFractalThreadRoutine(void *arg)
{
	ThreadArgHeader *threadArgHeader = GetThreadArgHeader(arg);
	volatile sig_atomic_t *cancel = threadArgHeader->cancel;
	const DrawFractalArguments *c_arg = (DrawFractalArguments *)GetThreadArgBody(arg);
	const Rectangle *clipRect = c_arg->clipRect;

	fractal2D_message(stdout, T_VERBOSE,"Drawing fractal from (%"PRIuFAST32",%"PRIuFAST32") to \
(%"PRIuFAST32",%"PRIuFAST32")...\n", clipRect->x1, clipRect->y1, clipRect->x2, clipRect->y2);

	if (c_arg->size == 1) {
		/* In that case, check for cancellation will be made
		   inside aux1_DrawFractalThreadRoutine after EACH
		   PIXEL (a scanline may be very big, too big).
		 */
		aux1_DrawFractalThreadRoutine(threadArgHeader, c_arg, clipRect);
	} else {
		/* Cut rectangle into smaller rectangles, so that
		   all rectangles are smaller than quadInterpolationSize.
		 */
		Rectangle *rectangle;
		uint_fast32_t nbRectangles;
		CutRectangleMaxSize(*clipRect, c_arg->size, &rectangle, &nbRectangles);

		/* If the rectangle dissimilarity is greater that threshold,
		   we compute the fractal colors, otherwize we interpolate
		   them using the corner colors.
		   Check for cancellation after each rectangle is drawn.
		   There should be very small latency because rectangles
		   are fairly small (default size is 5*5 pixels).
		   If this results in too much latency when cancelling,
		   we might want to make this check in
		   aux2_DrawFractalThreadRoutine, after each scanline
		   for example.
		 */
		for (uint_fast32_t i = 0; i < nbRectangles && !(*cancel); ++i) {
			aux2_DrawFractalThreadRoutine(c_arg, &rectangle[i]);
			threadArgHeader->progress = 100 * i / nbRectangles;
		}
		threadArgHeader->progress = 100;
		free(rectangle);
	}

	fractal2D_message(stdout, T_VERBOSE,"Drawing fractal from (%"PRIuFAST32",%"PRIuFAST32") to \
(%"PRIuFAST32",%"PRIuFAST32") : %s.\n", clipRect->x1, clipRect->y1, clipRect->x2,
		clipRect->y2, (*cancel) ? "CANCELED" : "DONE");

	return ((*cancel) ? PTHREAD_CANCELED : NULL);
}

inline Action *LaunchDrawFractalFast(Image *image, const Fractal *fractal,
			const RenderingParameters *render, uint_fast32_t quadInterpolationSize,
			FLOAT interpolationThreshold, Threads *threads)
{
	if (image->width < 2 || image->height < 2) {
		return DoNothingAction();
	}
	if (quadInterpolationSize == 0) {
		quadInterpolationSize = 1;
	}

	uint_fast32_t tableSize = image->width*image->height;
	uint_fast32_t nbThreadsNeeded = (threads->N > tableSize) ? tableSize : threads->N;

	Rectangle *rectangle;
	rectangle = (Rectangle *)safeMalloc("rectangles", nbThreadsNeeded * sizeof(Rectangle));
	InitRectangle(&rectangle[0], 0, 0, image->width-1, image->height-1);
	if (CutRectangleInN(rectangle[0], nbThreadsNeeded, rectangle)) {
		fractal2D_error("Could not cut rectangle ((%"PRIuFAST32",%"PRIuFAST32"),\
(%"PRIuFAST32",%"PRIuFAST32") in %"PRIuFAST32" parts.\n", rectangle[0].x1, rectangle[0].y1,
			rectangle[0].x2, rectangle[0].y2, nbThreadsNeeded);
	}
	
	DrawFractalArguments *arg;
	arg = (DrawFractalArguments *)safeMalloc("arguments", nbThreadsNeeded *
							sizeof(DrawFractalArguments));

	FractalLoop fractalLoop = GetFractalLoop(fractal->fractalFormula, fractal->p_is_integer,
					render->coloringMethod, render->addendFunction,
					render->interpolationMethod);
	for (uint_fast32_t i = 0; i < nbThreadsNeeded; ++i) {
		arg[i].image = image;
		arg[i].copyImage = NULL;
		arg[i].fractal = fractal;
		arg[i].render = render;
		arg[i].fractalLoop = fractalLoop;

		arg[i].clipRect = (Rectangle *)safeMalloc("clipRect", sizeof(Rectangle));
		*(arg[i].clipRect) = CopyRectangle(&rectangle[i]);
		arg[i].size = quadInterpolationSize;
		arg[i].threshold = interpolationThreshold;
	}
	Action *action = LaunchAction("Drawing fractal", threads, nbThreadsNeeded, arg, 
					sizeof(DrawFractalArguments), DrawFractalThreadRoutine,
					FreeDrawFractalArguments);

	free(arg);
	free(rectangle);

	return action;
}

inline void DrawFractalFast(Image *image, const Fractal *fractal, const RenderingParameters *render,
			uint_fast32_t quadInterpolationSize, FLOAT interpolationThreshold,
			Threads *threads)
{
	Action *action = LaunchDrawFractalFast(image, fractal, render, quadInterpolationSize,
						interpolationThreshold, threads);
	int unused = GetActionResult(action);
	(void)unused;
	FreeAction(action);
}

void DrawFractal(Image *image, const Fractal *fractal, const RenderingParameters *render,
			Threads *threads)
{
	DrawFractalFast(image, fractal, render, 1, 0, threads);
}

void *AntiAliaseFractalThreadRoutine(void *arg)
{
	ThreadArgHeader *threadArgHeader = GetThreadArgHeader(arg);
	volatile sig_atomic_t *cancel = threadArgHeader->cancel;
	const DrawFractalArguments *c_arg = (DrawFractalArguments *)GetThreadArgBody(arg);
	Image *image = c_arg->image;
	const Image *copyImage = c_arg->copyImage;
	const Rectangle *clipRect = c_arg->clipRect;
	uint_fast32_t antialiasingSize = c_arg->size;
	FLOAT threshold = c_arg->threshold;
	uint_fast32_t width = image->width;
	uint_fast32_t height = image->height;

	fractal2D_message(stdout, T_VERBOSE,"Anti-aliasing fractal from (%"PRIuFAST32",%"PRIuFAST32") to \
(%"PRIuFAST32",%"PRIuFAST32")...\n", clipRect->x1, clipRect->y1, clipRect->x2, clipRect->y2);

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
	FLOAT max = 0;
	uint_fast32_t rectHeight = clipRect->y2 - clipRect->y1 + 1;

	uint_fast32_t y = clipRect->y1 * antialiasingSize;
	for (uint_fast32_t i = clipRect->y1; i <= clipRect->y2 && !(*cancel); ++i, y+=antialiasingSize) {
		uint_fast32_t x = clipRect->x1 * antialiasingSize;
		for (uint_fast32_t j = clipRect->x1; j <= clipRect->x2 && !(*cancel); ++j, 
				x+=antialiasingSize) {
			C[0]  = iGetPixel(copyImage, j, i);
			C[1]  = iGetPixel(copyImage, j-1, i-1);
			C[2]  = iGetPixel(copyImage, j, i-1);
			C[3]  = iGetPixel(copyImage, j+1, i-1);
			C[4]  = iGetPixel(copyImage, j-1, i);
			C[5]  = iGetPixel(copyImage, j+1, i);
			C[6]  = iGetPixel(copyImage, j-1, i+1);
			C[7]  = iGetPixel(copyImage, j, i+1);
			C[8]  = iGetPixel(copyImage, j+1, i+1);

			max = 0;
			for (int_fast8_t k = 1; k < 9; ++k) {
				max = fmaxF(max, ColorManhattanDistance(C[0], C[k]));
			}

			if (max > threshold) {
				for (uint_fast32_t k=0; k<antialiasingSize; ++k) {
					for (uint_fast32_t l=0; l<antialiasingSize; ++l) {
						c = ComputeFractalImagePixel(c_arg, bigWidth, bigHeight,
										x+l, y+k);
						PutPixelUnsafe(&tmpImage1, l, k, c);
					}
				}

				for (uint_fast32_t k = 0; k < antialiasingSize; ++k) {
					c = ApplyFilterOnSinglePixel(&tmpImage1, center, k,
									&horizontalGaussianFilter);
					PutPixelUnsafe(&tmpImage2, 0, k, c);
				}
				c = ApplyFilterOnSinglePixel(&tmpImage2, 0, center,
								&verticalGaussianFilter);

				PutPixelUnsafe(image, j, i, c);
			}
		}
		threadArgHeader->progress = (i-clipRect->y1) * 100 / rectHeight;
	}
	threadArgHeader->progress = 100;

	FreeFilter(horizontalGaussianFilter);
	FreeFilter(verticalGaussianFilter);
	FreeImage(tmpImage1);
	FreeImage(tmpImage2);

	fractal2D_message(stdout, T_VERBOSE,"Anti-aliasing fractal from (%"PRIuFAST32",%"PRIuFAST32") to \
(%"PRIuFAST32",%"PRIuFAST32") : %s.\n", clipRect->x1, clipRect->y1, clipRect->x2,
		clipRect->y2, (*cancel) ? "CANCELED" : "DONE");

	return ((*cancel) ? PTHREAD_CANCELED : NULL);
}

inline Action *LaunchAntiAliaseFractal(Image *image, const Fractal *fractal,
			const RenderingParameters *render, uint_fast32_t antiAliasingSize,
			FLOAT threshold, Threads *threads)
{
	if (antiAliasingSize == 0) {
		return DoNothingAction();
	}
	if (image->width*antiAliasingSize < 2 || image->height*antiAliasingSize < 2) {
		return DoNothingAction();
	}
	uint_fast32_t tableSize = image->width*image->height;
	uint_fast32_t nbThreadsNeeded = (threads->N > tableSize) ? tableSize : threads->N;

	Rectangle *rectangle;
	rectangle = (Rectangle *)safeMalloc("rectangles", nbThreadsNeeded * sizeof(Rectangle));
	InitRectangle(&rectangle[0], 0, 0, image->width-1, image->height-1);
	if (CutRectangleInN(rectangle[0], nbThreadsNeeded, rectangle)) {
		fractal2D_error("Could not cut rectangle ((%"PRIuFAST32",%"PRIuFAST32"),\
(%"PRIuFAST32",%"PRIuFAST32") in %"PRIuFAST32" parts.\n", rectangle[0].x1, rectangle[0].y1,
			rectangle[0].x2, rectangle[0].y2, nbThreadsNeeded);
	}
	
	DrawFractalArguments *arg;
	arg = (DrawFractalArguments *)safeMalloc("arguments", nbThreadsNeeded*sizeof(DrawFractalArguments));

	FractalLoop fractalLoop = GetFractalLoop(fractal->fractalFormula, fractal->p_is_integer,
					render->coloringMethod, render->addendFunction,
					render->interpolationMethod);
	for (uint_fast32_t i = 0; i < nbThreadsNeeded; ++i) {
		arg[i].image = image;
		arg[i].copyImage = (Image *)safeMalloc("copyImage", sizeof(Image));
		*(arg[i].copyImage) = CopyImage(image);
		arg[i].fractal = fractal;
		arg[i].render = render;
		arg[i].fractalLoop = fractalLoop;

		arg[i].clipRect = (Rectangle *)safeMalloc("clipRect", sizeof(Rectangle));
		*(arg[i].clipRect) = CopyRectangle(&rectangle[i]);
		arg[i].size = antiAliasingSize;
		arg[i].threshold = threshold;
	}
	Action *res = LaunchAction("Anti-aliasing fractal", threads, nbThreadsNeeded, arg,
					sizeof(DrawFractalArguments), AntiAliaseFractalThreadRoutine,
					FreeDrawFractalArguments);

	free(rectangle);
	free(arg);

	return res;
}

void AntiAliaseFractal(Image *image, const Fractal *fractal, const RenderingParameters *render,
			uint_fast32_t antiAliasingSize, FLOAT threshold, Threads *threads)
{
	Action *action = LaunchAntiAliaseFractal(image, fractal, render, antiAliasingSize, threshold,
							threads);
	int unused = GetActionResult(action);
	(void)unused;
	FreeAction(action);
}

