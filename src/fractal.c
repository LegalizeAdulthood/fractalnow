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
#include "queue.h"
#include <complex.h>
#include <math.h>
#include <pthread.h>
#include <string.h>

typedef struct s_DrawFractalArguments {
	Image *image;
	Fractal *fractal;
	Rectangle *clipRect;
	Gradient *gradient;
	Color spaceColor;
	double multiplier;
	uint32_t quadInterpolationSize;
	double interpolationThreshold;
} DrawFractalArguments;

void InitFractal(Fractal *fractal, double x1, double y1, double x2, double y2,
			double escapeRadius, uint32_t maxIter)
{
	fractal->x1 = x1;
	fractal->x2 = x2;
	fractal->y1 = y1;
	fractal->y2 = y2;
	fractal->escapeRadius = escapeRadius;
	fractal->maxIter = maxIter;
}

void InitFractal2(Fractal *fractal, double centerX, double centerY, double spanX,
			double spanY, double escapeRadius, uint32_t maxIter)
{
	fractal->x1=centerX-spanX/2;
	fractal->x2=centerX+spanX/2;
	fractal->y1=centerY-spanY/2;
	fractal->y2=centerY+spanY/2;
	fractal->escapeRadius = escapeRadius;
	fractal->maxIter = maxIter;
}

Color ComputeFractalPixel(Fractal *fractal, Gradient *gradient, Color spaceColor,
				double multiplier, uint32_t width, uint32_t height,
				uint32_t x, uint32_t y)
{
	double dwidth = (double)width - 1;
	double dheight = (double)height - 1;

	double complex c = (fractal->x1*dwidth+x*(fractal->x2-fractal->x1)) / dwidth + I*(fractal->y1*dheight+y*(fractal->y2-fractal->y1)) / dheight;
	double complex z = 0;
	double normZ = 0;

	uint32_t n;
	double data;
	for (n=0; n<fractal->maxIter && normZ < fractal->escapeRadius; n++) {
		z = z*z+c;
		normZ = creal(z)*creal(z) + cimag(z)*cimag(z);
	}
	if (normZ < fractal->escapeRadius) {
		return spaceColor;
	} else {
		data = sqrt((double)(n) + (log(log(fractal->escapeRadius)/log(normZ))) / log(2.)) * multiplier;
		return GetGradientColor(gradient, (uint32_t)(data));
	}
}

int IsTooBigForInterpolation(Rectangle *rectangle, uint32_t quadInterpolationSize)
{
	return ((rectangle->x2-rectangle->x1 > quadInterpolationSize)
		|| (rectangle->y2-rectangle->y1 > quadInterpolationSize));
}

void *DrawFractalThreadRoutine(void *arg)
{
	DrawFractalArguments *c_arg = (DrawFractalArguments *)arg;
	Fractal *fractal = c_arg->fractal;
	Gradient *gradient = c_arg->gradient;
	Color spaceColor = c_arg->spaceColor;
	double multiplier  = c_arg->multiplier;
	Image *image = c_arg->image;
	Rectangle *clipRect = c_arg->clipRect;
	uint32_t width = image->width;
	uint32_t height = image->height;

	info(T_VERBOSE,"Drawing fractal from (%u,%u) to (%u,%u)...\n",clipRect->x1,clipRect->y1,
			clipRect->x2,clipRect->y2);

	if (width<=1 || height<=1) {
		error("Image is too small : %dx%d\n",width,height);
	}

	Color color;
	if (c_arg->quadInterpolationSize == 1) {
		for (uint32_t i=clipRect->y1; i<=clipRect->y2; i++) {
			for (uint32_t j=clipRect->x1; j<=clipRect->x2; j++) {
				color = ComputeFractalPixel(fractal, gradient, spaceColor, multiplier,
								width, height, j, i);
				PutPixel(image,j,i,color);
			}
		}
	} else {
		Queue queue_src, queue_dst;
		InitEmptyQueue(&queue_src);
		InitEmptyQueue(&queue_dst);
		Rectangle *currentRectangle = (Rectangle *)malloc(sizeof(Rectangle));
		if (currentRectangle == NULL) {
			alloc_error("rectangle");
		}
		InitRectangle(currentRectangle, clipRect->x1, clipRect->y1, clipRect->x2, clipRect->y2);
		PushQueue(&queue_src, (void *)currentRectangle);
		Rectangle *new_rectangle[2];

		while (!IsQueueEmpty(&queue_src)) {
			currentRectangle = (Rectangle *)PopQueue(&queue_src);

			if (IsTooBigForInterpolation(currentRectangle, c_arg->quadInterpolationSize)) {
				new_rectangle[0] = (Rectangle *)malloc(sizeof(Rectangle));
				if (currentRectangle == NULL) {
					alloc_error("rectangle");
				}
				new_rectangle[1] = (Rectangle *)malloc(sizeof(Rectangle));
				if (currentRectangle == NULL) {
					alloc_error("rectangle");
				}

				CutRectangleInHalf(*currentRectangle, new_rectangle[0], new_rectangle[1]);
				PushQueue(&queue_src, (void *)new_rectangle[0]);
				PushQueue(&queue_src, (void *)new_rectangle[1]);

				free(currentRectangle);
			} else {
				PushQueue(&queue_dst, (void *)currentRectangle);
			}
		}

		while (!IsQueueEmpty(&queue_dst)) {
			currentRectangle = (Rectangle *)PopQueue(&queue_dst);

			Color corner[4];
			corner[0] = ComputeFractalPixel(fractal,gradient,spaceColor,multiplier,width,
							height,currentRectangle->x1,currentRectangle->y1);
			corner[1] = ComputeFractalPixel(fractal,gradient,spaceColor,multiplier,width,
							height,currentRectangle->x2,currentRectangle->y1);
			corner[2] = ComputeFractalPixel(fractal,gradient,spaceColor,multiplier,width,
							height,currentRectangle->x1,currentRectangle->y2);
			corner[3] = ComputeFractalPixel(fractal,gradient,spaceColor,multiplier,width,
							height,currentRectangle->x2,currentRectangle->y2);

			if (QuadHeterogeneity(corner[0],corner[1],corner[2],corner[3]) >= c_arg->interpolationThreshold) {
				for (uint32_t i=currentRectangle->y1; i<=currentRectangle->y2; i++) {
					for (uint32_t j=currentRectangle->x1; j<=currentRectangle->x2; j++) {
						if ((j == currentRectangle->x1 && i == currentRectangle->y1)) {
							color = corner[0];
						} else if (j == currentRectangle->x2 && i == currentRectangle->y1) {
							color = corner[1];
						} else if (j == currentRectangle->x1 && i == currentRectangle->y2) {
							color = corner[2];
						} else if (j == currentRectangle->x2 && i == currentRectangle->y2) {
							color = corner[3];
						} else {
							color = ComputeFractalPixel(fractal, c_arg->gradient,
										c_arg->spaceColor, c_arg->multiplier,
										width, height, j, i);
						}
						PutPixel(image,j,i,color);
					}
				}
			} else {
				// linear interpolation
				double sx = (double)currentRectangle->x2-currentRectangle->x1;
				double sy = (double)currentRectangle->y2-currentRectangle->y1;
				double x, y;
				for (uint32_t i=currentRectangle->y1; i<=currentRectangle->y2; i++) {
					y = ((double)(i-currentRectangle->y1)) / sy;
					for (uint32_t j=currentRectangle->x1; j<=currentRectangle->x2; j++) {
						if ((j == currentRectangle->x1 && i == currentRectangle->y1)) {
							color = corner[0];
						} else if (j == currentRectangle->x2 && i == currentRectangle->y1) {
							color = corner[1];
						} else if (j == currentRectangle->x1 && i == currentRectangle->y2) {
							color = corner[2];
						} else if (j == currentRectangle->x2 && i == currentRectangle->y2) {
							color = corner[3];
						} else {
							x = ((double)(j-currentRectangle->x1)) / sx;
							color = LinearInterpolation(corner[0],corner[1],corner[2],corner[3],x,y);
						}
						PutPixel(image,j,i,color);
					}
				}
			}

			free(currentRectangle);
		}
	}

	info(T_VERBOSE,"Drawing fractal from (%u,%u) to (%u,%u) : DONE.\n",clipRect->x1,clipRect->y1,
		clipRect->x2,clipRect->y2);

	return NULL;
}

void DrawFractalFast(Image *image, Fractal *fractal, Gradient *gradient, Color spaceColor,
		double multiplier, uint32_t quadInterpolationSize, double interpolationThreshold)
{
	info(T_NORMAL,"Drawing fractal...\n");

	uint32_t nbPixels = image->width*image->height;
	uint32_t realNbThreads = (nbThreads > nbPixels) ? nbPixels : nbThreads;

	Rectangle *rectangle;
	rectangle = (Rectangle *)malloc(realNbThreads * sizeof(Rectangle));
	if (rectangle == NULL) {
		alloc_error("rectangles");
	}
	InitRectangle(&rectangle[0], 0, 0, image->width-1, image->height-1);
	CutRectangleInN(rectangle[0], realNbThreads, rectangle);
	
	DrawFractalArguments *arg;
	arg = (DrawFractalArguments *)malloc(realNbThreads * sizeof(DrawFractalArguments));
	if (arg == NULL) {
		alloc_error("arguments");
	}

        pthread_t *thread;
	thread = (pthread_t *)malloc(realNbThreads * sizeof(pthread_t)); 
	if (thread == NULL) {
		alloc_error("threads");
	}
	for (uint32_t i = 0; i < realNbThreads; ++i) {
		arg[i].image = image;
		arg[i].fractal = fractal;
		arg[i].clipRect = &rectangle[i];
		arg[i].quadInterpolationSize = quadInterpolationSize;
		arg[i].interpolationThreshold = interpolationThreshold;
		arg[i].gradient = gradient;
		arg[i].spaceColor = spaceColor;
		arg[i].multiplier = multiplier;

                safePThreadCreate(&(thread[i]),NULL,DrawFractalThreadRoutine,(void *)&arg[i]);
	}

	void *status;
	for (uint32_t i = 0; i < realNbThreads; ++i) {
		safePThreadJoin(thread[i], &status);

		if (status != NULL) {
			error("Thread ended because of an error.\n");
		}
	}

	free(rectangle);
	free(arg);
	free(thread);

	info(T_NORMAL,"Drawing fractal : DONE.\n");
}

inline void DrawFractal(Image *image, Fractal *fractal, Gradient *gradient, Color spaceColor,
			double multiplier)
{
	DrawFractalFast(image, fractal, gradient, spaceColor, multiplier, 1, 0);
}

