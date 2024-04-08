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
#include <inttypes.h>
#include <pthread.h>
#include <signal.h>
#include <string.h>

typedef struct s_DrawFractalArguments {
	Image *image;
	Image *copyImage;
	Fractal *fractal;
	RenderingParameters *render;
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

void ReadFractalFile(Fractal *fractal, const char *fileName)
{
	info(T_NORMAL, "Reading fractal file...\n");

	FILE *file;
	char tmp[6][256];

	file=fopen(fileName,"r");
	if (!file) {
		open_error(fileName);
	}

	FractalFormula fractalFormula;
	safeReadString(file, fileName, tmp[0]);
	fractalFormula = GetFractalFormula(tmp[0]);
	FLOAT p;
	switch (fractalFormula) {
	case FRAC_MANDELBROT:
	case FRAC_JULIA:
		p = 2;
		break;
	case FRAC_MANDELBROTP:
	case FRAC_JULIAP:
	case FRAC_RUDY:
		p = safeReadFLOAT(file, fileName);
		break;
	default:
		error("Unknown fractal type \'%s\'.\n", tmp[0]);
		break;
	}

	if (p < 0 || p > 100) {
		error("Invalid fractal file : p must be between 0 and 100.\n");
	}
	FLOAT cx = 0, cy = 0;

	switch (fractalFormula) {
	case FRAC_MANDELBROT:
	case FRAC_MANDELBROTP:
		break;
	case FRAC_JULIA:
	case FRAC_JULIAP:
	case FRAC_RUDY:
		cx = safeReadFLOAT(file, fileName);
		cy = safeReadFLOAT(file, fileName);
		break;
	default:
		error("Unknown fractal type \'%s\'.\n", tmp[0]);
		break;
	}

	FLOAT centerX = safeReadFLOAT(file, fileName);
	FLOAT centerY = safeReadFLOAT(file, fileName);
	FLOAT spanX = safeReadFLOAT(file, fileName);
	if (spanX <= 0) {
		error("Invalid fractal file : spanX must be > 0.\n");
	}
	FLOAT spanY = safeReadFLOAT(file, fileName);
	if (spanY <= 0) {
		error("Invalid fractal file : spanY must be > 0.\n");
	}
	FLOAT escapeRadius = safeReadFLOAT(file, fileName);
	if (escapeRadius <= 1.) {
		error("Invalid config file : escape radius must be > 1.\n");
	}
	uint_fast32_t nbIterMax = safeReadUint32(file, fileName);

	InitFractal2(fractal, fractalFormula, p, cx+I*cy, centerX, centerY, spanX, spanY,
			escapeRadius, nbIterMax);

	if (fclose(file)) {
		close_error(fileName);
	}

	info(T_NORMAL, "Reading fractal file : DONE\n");
}

static inline Color aux_ComputeFractalColor(Fractal *fractal, RenderingParameters *render,
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

Color ComputeFractalColor(Fractal *fractal, RenderingParameters *render, FLOAT complex z)
{
	FractalLoop fractalLoop = GetFractalLoop(fractal->fractalFormula, fractal->p_is_integer,
					render->coloringMethod, render->addendFunction,
					render->interpolationMethod);
	return aux_ComputeFractalColor(fractal, render, fractalLoop, z);
}

static inline Color ComputeFractalImagePixel(DrawFractalArguments *arg,
						uint_fast32_t width, uint_fast32_t height,
						uint_fast32_t x, uint_fast32_t y)
{
	Fractal *fractal = arg->fractal;
	RenderingParameters *render = arg->render;
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
static void aux1_DrawFractalThreadRoutine(volatile sig_atomic_t *cancel, DrawFractalArguments *arg, Rectangle *rectangle)
{
	Image *image = arg->image;

	Color color;
	for (uint_fast32_t i=rectangle->y1; i<=rectangle->y2 && !(*cancel); i++) {
		for (uint_fast32_t j=rectangle->x1; j<=rectangle->x2 && !(*cancel); j++) {
			color = ComputeFractalImagePixel(arg, image->width, image->height, j, i);
			PutPixelUnsafe(image,j,i,color);
		}
	}
}

static inline int IsTooBigForInterpolation(Rectangle *rectangle, uint_fast32_t quadInterpolationSize)
{
	return ((rectangle->x2-rectangle->x1+1 > quadInterpolationSize)
		|| (rectangle->y2-rectangle->y1+1 > quadInterpolationSize));
}

static inline int_fast8_t GetCornerIndex(Rectangle *rectangle, uint_fast32_t x, uint_fast32_t y)
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
static inline void aux2_DrawFractalThreadRoutine(DrawFractalArguments *arg, Rectangle *rectangle)
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
	volatile sig_atomic_t *cancel = *((volatile sig_atomic_t **)arg);
	uint8_t *arg1 = (uint8_t *)arg;
	DrawFractalArguments *c_arg = (DrawFractalArguments *)(arg1+sizeof(volatile sig_atomic_t *));
	Rectangle *clipRect = c_arg->clipRect;

	info(T_VERBOSE,"Drawing fractal from (%"PRIuFAST32",%"PRIuFAST32") to \
(%"PRIuFAST32",%"PRIuFAST32")...\n", clipRect->x1, clipRect->y1, clipRect->x2, clipRect->y2);

	int old_val;
	pthread_setcanceltype(PTHREAD_CANCEL_ASYNCHRONOUS, &old_val);

	if (c_arg->size == 1) {
		/* In that case, check for cancelling will be made
		   inside aux1_DrawFractalThreadRoutine after EACH
		   PIXEL (a scanline may be very big, too big).
		 */
		aux1_DrawFractalThreadRoutine(cancel, c_arg, clipRect);
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
		   Check for cancelling after each rectangle is drawn.
		   There should be very small latency because rectangles
		   are fairly small (default size is 5*5 pixels).
		   If this results in too much latency when cancelling,
		   we might want to make this check in
		   aux2_DrawFractalThreadRoutine, after each scanline
		   for example.
		 */
		for (uint_fast32_t i = 0; i < nbRectangles && !(*cancel); ++i) {
			aux2_DrawFractalThreadRoutine(c_arg, &rectangle[i]);
		}
		free(rectangle);
	}

	info(T_VERBOSE,"Drawing fractal from (%"PRIuFAST32",%"PRIuFAST32") to \
(%"PRIuFAST32",%"PRIuFAST32") : %s.\n", clipRect->x1, clipRect->y1, clipRect->x2,
		clipRect->y2, (*cancel) ? "CANCELED" : "DONE");

	return ((*cancel) ? PTHREAD_CANCELED : NULL);
}

inline Action *LaunchDrawFractalFast(Image *image, Fractal *fractal, RenderingParameters *render,
			uint_fast32_t quadInterpolationSize, FLOAT interpolationThreshold)
{
	if (image->width < 2 || image->height < 2) {
		return DoNothingAction();
	}
	if (quadInterpolationSize == 0) {
		quadInterpolationSize = 1;
	}

	uint_fast32_t tableSize = image->width*image->height;
	uint_fast32_t realNbThreads = (nbThreads > tableSize) ? tableSize : nbThreads;

	Rectangle *rectangle;
	rectangle = (Rectangle *)safeMalloc("rectangles", realNbThreads * sizeof(Rectangle));
	InitRectangle(&rectangle[0], 0, 0, image->width-1, image->height-1);
	if (CutRectangleInN(rectangle[0], realNbThreads, rectangle)) {
		error("Could not cut rectangle ((%"PRIuFAST32",%"PRIuFAST32"),\
(%"PRIuFAST32",%"PRIuFAST32") in %"PRIuFAST32" parts.\n", rectangle[0].x1, rectangle[0].y1,
			rectangle[0].x2, rectangle[0].y2, realNbThreads);
	}
	
	DrawFractalArguments *arg;
	arg = (DrawFractalArguments *)safeMalloc("arguments", realNbThreads *sizeof(DrawFractalArguments));

	FractalLoop fractalLoop = GetFractalLoop(fractal->fractalFormula, fractal->p_is_integer,
					render->coloringMethod, render->addendFunction,
					render->interpolationMethod);
	for (uint_fast32_t i = 0; i < realNbThreads; ++i) {
		arg[i].image = image;
		arg[i].copyImage = NULL;
		arg[i].fractal = fractal;
		arg[i].render = render;
		arg[i].fractalLoop = fractalLoop;

		arg[i].clipRect = CopyRectangle(&rectangle[i]);
		arg[i].size = quadInterpolationSize;
		arg[i].threshold = interpolationThreshold;
	}
	Action *action = LaunchThreads("Drawing fractal", realNbThreads, arg, sizeof(DrawFractalArguments),
					DrawFractalThreadRoutine, FreeDrawFractalArguments);

	free(arg);
	free(rectangle);

	return action;
}

inline void DrawFractalFast(Image *image, Fractal *fractal, RenderingParameters *render,
			uint_fast32_t quadInterpolationSize, FLOAT interpolationThreshold)
{
	Action *action = LaunchDrawFractalFast(image, fractal, render, quadInterpolationSize, interpolationThreshold);
	int unused = WaitForActionTermination(action);
	(void)unused;
	FreeAction(*action);
	free(action);
}

void DrawFractal(Image *image, Fractal *fractal, RenderingParameters *render)
{
	DrawFractalFast(image, fractal, render, 1, 0);
}

void *AntiAliaseFractalThreadRoutine(void *arg)
{
	volatile sig_atomic_t *cancel = *((volatile sig_atomic_t **)arg);
	uint8_t *arg1 = (uint8_t *)arg;
	DrawFractalArguments *c_arg = (DrawFractalArguments *)(arg1+sizeof(volatile sig_atomic_t *));
	Image *image = c_arg->image;
	Image *copyImage = c_arg->copyImage;
	Rectangle *clipRect = c_arg->clipRect;
	uint_fast32_t antialiasingSize = c_arg->size;
	FLOAT threshold = c_arg->threshold;
	uint_fast32_t width = image->width;
	uint_fast32_t height = image->height;

	info(T_VERBOSE,"Anti-aliasing fractal from (%"PRIuFAST32",%"PRIuFAST32") to \
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

	uint_fast32_t y = clipRect->y1 * antialiasingSize;
	for (uint_fast32_t i = clipRect->y1; i <= clipRect->y2 && !(*cancel); ++i, y+=antialiasingSize) {
		uint_fast32_t x = clipRect->x1 * antialiasingSize;
		for (uint_fast32_t j = clipRect->x1; j <= clipRect->x2 && !(*cancel); ++j, x+=antialiasingSize) {
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
						c = ComputeFractalImagePixel(c_arg, bigWidth, bigHeight, x+l, y+k);
						PutPixelUnsafe(&tmpImage1, l, k, c);
					}
				}

				for (uint_fast32_t k = 0; k < antialiasingSize; ++k) {
					c = ApplyFilterOnSinglePixel(&tmpImage1, center, k, &horizontalGaussianFilter);
					PutPixelUnsafe(&tmpImage2, 0, k, c);
				}
				c = ApplyFilterOnSinglePixel(&tmpImage2, 0, center, &verticalGaussianFilter);

				PutPixelUnsafe(image, j, i, c);
			}
		}
	}

	FreeFilter(horizontalGaussianFilter);
	FreeFilter(verticalGaussianFilter);
	FreeImage(tmpImage1);
	FreeImage(tmpImage2);

	info(T_VERBOSE,"Anti-aliasing fractal from (%"PRIuFAST32",%"PRIuFAST32") to \
(%"PRIuFAST32",%"PRIuFAST32") : %s.\n", clipRect->x1, clipRect->y1, clipRect->x2,
		clipRect->y2, (*cancel) ? "CANCELED" : "DONE");

	return ((*cancel) ? PTHREAD_CANCELED : NULL);
}

inline Action *LaunchAntiAliaseFractal(Image *image, Fractal *fractal, RenderingParameters *render,
			uint_fast32_t antiAliasingSize, FLOAT threshold)
{
	if (antiAliasingSize == 0) {
		return DoNothingAction();
	}
	if (image->width*antiAliasingSize < 2 || image->height*antiAliasingSize < 2) {
		return DoNothingAction();
	}
	uint_fast32_t tableSize = image->width*image->height;
	uint_fast32_t realNbThreads = (nbThreads > tableSize) ? tableSize : nbThreads;

	Rectangle *rectangle;
	rectangle = (Rectangle *)safeMalloc("rectangles", realNbThreads * sizeof(Rectangle));
	InitRectangle(&rectangle[0], 0, 0, image->width-1, image->height-1);
	if (CutRectangleInN(rectangle[0], realNbThreads, rectangle)) {
		error("Could not cut rectangle ((%"PRIuFAST32",%"PRIuFAST32"),\
(%"PRIuFAST32",%"PRIuFAST32") in %"PRIuFAST32" parts.\n", rectangle[0].x1, rectangle[0].y1,
			rectangle[0].x2, rectangle[0].y2, realNbThreads);
	}
	
	DrawFractalArguments *arg;
	arg = (DrawFractalArguments *)safeMalloc("arguments", realNbThreads*sizeof(DrawFractalArguments));

	FractalLoop fractalLoop = GetFractalLoop(fractal->fractalFormula, fractal->p_is_integer,
					render->coloringMethod, render->addendFunction,
					render->interpolationMethod);
	for (uint_fast32_t i = 0; i < realNbThreads; ++i) {
		arg[i].image = image;
		arg[i].copyImage = CopyImage(image);
		arg[i].fractal = fractal;
		arg[i].render = render;
		arg[i].fractalLoop = fractalLoop;

		arg[i].clipRect = CopyRectangle(&rectangle[i]);
		arg[i].size = antiAliasingSize;
		arg[i].threshold = threshold;
	}
	Action *res = LaunchThreads("Anti-aliasing fractal", realNbThreads, arg, sizeof(DrawFractalArguments),
					AntiAliaseFractalThreadRoutine, FreeDrawFractalArguments);

	free(rectangle);
	free(arg);

	return res;
}

void AntiAliaseFractal(Image *image, Fractal *fractal, RenderingParameters *render,
			uint_fast32_t antiAliasingSize, FLOAT threshold)
{
	Action *action = LaunchAntiAliaseFractal(image, fractal, render, antiAliasingSize, threshold);
	int unused = WaitForActionTermination(action);
	(void)unused;
	FreeAction(*action);
	free(action);
}

