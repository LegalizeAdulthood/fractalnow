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
#include <pthread.h>
#include <string.h>

typedef struct s_ComputeFractalArguments {
	FloatTable *fractal_table;
	Fractal *fractal;
	Rectangle *clipRect;
	uint32_t quadInterpolationSize;
	FLOAT interpolationThreshold;
} ComputeFractalArguments;

typedef struct s_RenderFractalArguments {
	Image *image;
	FloatTable *fractal_table;
	Rectangle *clipRect;
	Gradient *gradient;
	Color spaceColor;
	FLOAT multiplier;
} RenderFractalArguments;

FLOAT ComputeMandelbrotFractalValue(Fractal *fractal, FLOAT x, FLOAT y);
FLOAT ComputeJuliaFractalValue(Fractal *fractal, FLOAT x, FLOAT y);

void InitFractal(Fractal *fractal, FractalType fractalType, FLOAT complex c, FLOAT x1,
		FLOAT y1, FLOAT x2, FLOAT y2, FLOAT escapeRadius, uint32_t maxIter)
{
	switch (fractalType) {
	case FRAC_MANDELBROT:
		fractal->computeFunction = ComputeMandelbrotFractalValue;
		break;
	case FRAC_JULIA:
		fractal->computeFunction = ComputeJuliaFractalValue;
		break;
	default:
		error("Unknown fractal type.\n");
		break;
	}

	fractal->c = c;
	fractal->x1 = x1;
	fractal->x2 = x2;
	fractal->y1 = y1;
	fractal->y2 = y2;
	fractal->escapeRadius = escapeRadius;
	fractal->maxIter = maxIter;
}

void InitFractal2(Fractal *fractal, FractalType fractalType, FLOAT complex c, FLOAT centerX,
		FLOAT centerY, FLOAT spanX, FLOAT spanY, FLOAT escapeRadius, uint32_t maxIter)
{
	FLOAT x1 = centerX-spanX/2;
	FLOAT x2 = centerX+spanX/2;
	FLOAT y1 = centerY-spanY/2;
	FLOAT y2 = centerY+spanY/2;

	InitFractal(fractal, fractalType, c, x1, y1, x2, y2, escapeRadius, maxIter);
}

void ReadFractalFile(Fractal *fractal, char *fileName)
{
	info(T_NORMAL, "Reading fractal file...\n");

	FILE *file;
	char tmp[6][256];

	file=fopen(fileName,"r");
	if (!file) {
		open_error(fileName);
	}

	if ( fscanf(file,"%s",tmp[0]) == EOF ) {
		read_error(fileName);
	}
	FractalType fractalType;
	FLOAT cx = 0, cy = 0;

	if (strcmp(tmp[0], "MANDELBROT") == 0) {
		fractalType = FRAC_MANDELBROT;
	} else if (strcmp(tmp[0], "JULIA") == 0) {
		fractalType = FRAC_JULIA;

		if ( fscanf(file,"%s",tmp[0]) == EOF ) {
			read_error(fileName);
		}
		if ( fscanf(file,"%s",tmp[1]) == EOF ) {
			read_error(fileName);
		}
		cx = strtoF(tmp[0], NULL);
		cy = strtoF(tmp[1], NULL);
	} else {
		error("Invalid fractal file : unknown fractal type (%s).\n", tmp[0]);
	}

	for (uint32_t i=0; i<6; i++) {
		if ( fscanf(file,"%s",tmp[i]) == EOF ) {
			read_error(fileName);
		}
	}

	FLOAT centerX;
	FLOAT centerY;
	FLOAT spanX;
	FLOAT spanY;
	FLOAT escapeRadius;
	uint32_t nbIterMax;

	centerX = strtoF(tmp[0], NULL);
	centerY = strtoF(tmp[1], NULL);
	spanX = strtoF(tmp[2], NULL);
	if (spanX <= 0) {
		error("Invalid fractal file : spanX must be > 0.\n");
	}
	spanY = strtoF(tmp[3], NULL);
	if (spanY <= 0) {
		error("Invalid fractal file : spanY must be > 0.\n");
	}
	escapeRadius = strtoF(tmp[4], NULL);
	if (escapeRadius <= 0) {
		error("Invalid config file : escape radius must be > 0.\n");
	}
	nbIterMax = (uint32_t)strtoll(tmp[5], NULL, 10);

	InitFractal2(fractal, fractalType, cx+I*cy, centerX, centerY, spanX, spanY,
			escapeRadius, nbIterMax);

	if (fclose(file)) {
		close_error(fileName);
	}

	info(T_NORMAL, "Reading fractal file : DONE\n");
}

inline FLOAT ComputeFractalValue(Fractal *fractal, FLOAT complex z0, FLOAT complex c)
{
	FLOAT complex z = z0;
	FLOAT normZ = 0;

	uint32_t n;
	FLOAT data;
	for (n=0; n<fractal->maxIter && normZ < fractal->escapeRadius; n++) {
		z = z*z+c;
		normZ = creal(z)*creal(z) + cimag(z)*cimag(z);
	}

	if (normZ < fractal->escapeRadius) {
		return -1;
	} else {
		data = sqrt((FLOAT)(n) + (log(log(fractal->escapeRadius)/log(normZ))) / log(2.));
		return data;
	}
}

FLOAT ComputeMandelbrotFractalValue(Fractal *fractal, FLOAT x, FLOAT y)
{
	return ComputeFractalValue(fractal, 0, x+I*y);
}

FLOAT ComputeJuliaFractalValue(Fractal *fractal, FLOAT x, FLOAT y)
{
	return ComputeFractalValue(fractal, x+I*y, fractal->c);
}

FLOAT ComputeFractalValueForTable(Fractal *fractal,
				uint32_t width, uint32_t height,
				uint32_t x, uint32_t y)
{
	FLOAT fx = (fractal->x1*(width-1)+x*(fractal->x2-fractal->x1)) / (width-1);
	FLOAT fy = (fractal->y1*(height-1)+y*(fractal->y2-fractal->y1)) / (height-1);

	return fractal->computeFunction(fractal, fx, fy);
}

int IsTooBigForInterpolation(Rectangle *rectangle, uint32_t quadInterpolationSize)
{
	return ((rectangle->x2-rectangle->x1 > quadInterpolationSize)
		|| (rectangle->y2-rectangle->y1 > quadInterpolationSize));
}

/* Compute the dissimilarity of the quadrilateral and compare it with given threshold.
   Note that when one corner is < 0 (meaning here that it is inside the fractal set),
   then the quad cannot be interpolated (unless ALL the corners belong to the
   fractal set).
*/
int QuadCanBeInterpolated(FLOAT v[4], FLOAT threshold)//1, FLOAT v2, FLOAT v3, FLOAT v4, FLOAT threshold)
{
	if (v[0] < 0 || v[1] < 0 || v[2] < 0 || v[3] < 0) {
		if (v[0] < 0 && v[1] < 0 && v[2] < 0 && v[3] < 0) {
			return 1;
		} else {
			return 0;
		}
	}
	
	FLOAT avg;
	avg = (v[0] + v[1] + v[2] + v[3]) / 4;
	FLOAT avgError = (fabsF(v[0]-avg)+fabsF(v[1]-avg)+
			fabsF(v[2]-avg)+fabsF(v[3]-avg)) / 4;

	return (avgError < threshold);
}

inline FLOAT QuadLinearInterpolation(FLOAT v[4], FLOAT x, FLOAT y)
{
	return (v[0]*(1.-x)+v[1]*x)*(1.-y) + (v[2]*(1.-x)+v[3]*x)*y;
}

/* Compute (all) fractal values of given rectangle into fractal_table.
 */
void aux1_ComputeFractalThreadRoutine(FloatTable *fractal_table, Fractal *fractal,
					Rectangle *rectangle)
{
	uint32_t width = fractal_table->width;
	uint32_t height = fractal_table->height;

	FLOAT value = -1;
	for (uint32_t i=rectangle->y1; i<=rectangle->y2; i++) {
		for (uint32_t j=rectangle->x1; j<=rectangle->x2; j++) {
			value = ComputeFractalValueForTable(fractal, width, height, j, i);
			SetDataUnsafe(fractal_table,j,i,value);
		}
	}
}

/* Compute fractal values of given rectangle into fractal_table, according to
   its dissimilarity and the given dissimilarity threshold (i.e. either
   computes it really, or interpolate linearly from the corners).
 */
void aux2_ComputeFractalThreadRoutine(FloatTable *fractal_table, Fractal *fractal,
					Rectangle *rectangle, FLOAT interpolationThreshold)
{
	uint32_t width = fractal_table->width;
	uint32_t height = fractal_table->height;


	FLOAT corner[4];
	corner[0] = ComputeFractalValueForTable(fractal,width,height,rectangle->x1,rectangle->y1);
	corner[1] = ComputeFractalValueForTable(fractal,width,height,rectangle->x2,rectangle->y1);
	corner[2] = ComputeFractalValueForTable(fractal,width,height,rectangle->x1,rectangle->y2);
	corner[3] = ComputeFractalValueForTable(fractal,width,height,rectangle->x2,rectangle->y2);

	FLOAT value = -1;
	if (QuadCanBeInterpolated(corner, interpolationThreshold)) {
		/* Linear interpolation */
		FLOAT sx = (FLOAT)rectangle->x2-rectangle->x1;
		FLOAT sy = (FLOAT)rectangle->y2-rectangle->y1;
		FLOAT x, y;
		for (uint32_t i=rectangle->y1; i<=rectangle->y2; i++) {
			y = ((FLOAT)(i-rectangle->y1)) / sy;
			for (uint32_t j=rectangle->x1; j<=rectangle->x2; j++) {
				if ((j == rectangle->x1 && i == rectangle->y1)) {
					value = corner[0];
				} else if (j == rectangle->x2 && i == rectangle->y1) {
					value = corner[1];
				} else if (j == rectangle->x1 && i == rectangle->y2) {
					value = corner[2];
				} else if (j == rectangle->x2 && i == rectangle->y2) {
					value = corner[3];
				} else {
					x = ((FLOAT)(j-rectangle->x1)) / sx;
					value = QuadLinearInterpolation(corner,x,y);
				}
				SetDataUnsafe(fractal_table,j,i,value);
			}
		}
	} else {
		/* Real computation */
		for (uint32_t i=rectangle->y1; i<=rectangle->y2; i++) {
			for (uint32_t j=rectangle->x1; j<=rectangle->x2; j++) {
				if ((j == rectangle->x1 && i == rectangle->y1)) {
					value = corner[0];
				} else if (j == rectangle->x2 && i == rectangle->y1) {
					value = corner[1];
				} else if (j == rectangle->x1 && i == rectangle->y2) {
					value = corner[2];
				} else if (j == rectangle->x2 && i == rectangle->y2) {
					value = corner[3];
				} else {
					value = ComputeFractalValueForTable(fractal, width, height, j, i);
				}
				SetDataUnsafe(fractal_table,j,i,value);
			}
		}
	}
}

void *ComputeFractalThreadRoutine(void *arg)
{
	ComputeFractalArguments *c_arg = (ComputeFractalArguments *)arg;
	FloatTable *fractal_table = c_arg->fractal_table;
	Fractal *fractal = c_arg->fractal;
	Rectangle *clipRect = c_arg->clipRect;
	uint32_t width = fractal_table->width;
	uint32_t height = fractal_table->height;

	info(T_VERBOSE,"Computing fractal from (%lu,%lu) to (%lu,%lu)...\n",
		(unsigned long)clipRect->x1,(unsigned long)clipRect->y1,
		(unsigned long)clipRect->x2,(unsigned long)clipRect->y2);

	if (width<2 || height<2) {
		error("Table is too small : %lux%lu\n",
			(unsigned long)width,(unsigned long)height);
	}

	if (c_arg->quadInterpolationSize == 1) {
		aux1_ComputeFractalThreadRoutine(fractal_table, fractal, clipRect);
	} else {
		/* First, cut rectangle into smaller rectangles, until
		   all rectangles are smaller than quadInterpolationSize.
		   We do that by creating a queue, pushing clipRect in it,
		   cutting rectangles and pushing the ones that are too big
		   in the first queue, and those that are small enough in
		   the second queue.
		   Process is stopped when the first queue is empty :
		   then all the rectangles in the second queue are small
		   enough.
		 */
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

		/* Then we process all the rectangles in the second queue.
		 * If the rectangle dissimilarity is greater that threshold,
		 * we compute the fractal values, otherwize we interpolate
		 * them using the corner values.
		*/
		while (!IsQueueEmpty(&queue_dst)) {
			currentRectangle = (Rectangle *)PopQueue(&queue_dst);

			aux2_ComputeFractalThreadRoutine(fractal_table, fractal, currentRectangle,
							c_arg->interpolationThreshold);
			free(currentRectangle);
		}
	}

	info(T_VERBOSE,"Computing fractal from (%lu,%lu) to (%lu,%lu) : DONE.\n",
		(unsigned long)clipRect->x1,(unsigned long)clipRect->y1,
		(unsigned long)clipRect->x2,(unsigned long)clipRect->y2);

	return NULL;
}

void ComputeFractalFast(FloatTable *fractal_table, Fractal *fractal,
			uint32_t quadInterpolationSize, FLOAT interpolationThreshold)
{
	info(T_NORMAL,"Computing fractal...\n");

	uint32_t tableSize = fractal_table->width*fractal_table->height;
	uint32_t realNbThreads = (nbThreads > tableSize) ? tableSize : nbThreads;

	Rectangle *rectangle;
	rectangle = (Rectangle *)malloc(realNbThreads * sizeof(Rectangle));
	if (rectangle == NULL) {
		alloc_error("rectangles");
	}
	InitRectangle(&rectangle[0], 0, 0, fractal_table->width-1, fractal_table->height-1);
	if (CutRectangleInN(rectangle[0], realNbThreads, rectangle)) {
		error("Could not rectangle ((%lu,%lu),(%lu,%lu) in %lu parts.\n",
			(unsigned long)rectangle[0].x1, (unsigned long)rectangle[0].y1,
			(unsigned long)rectangle[0].x2, (unsigned long)rectangle[0].y2,
			(unsigned long)realNbThreads);
	}
	
	ComputeFractalArguments *arg;
	arg = (ComputeFractalArguments *)malloc(realNbThreads * sizeof(ComputeFractalArguments));
	if (arg == NULL) {
		alloc_error("arguments");
	}

        pthread_t *thread;
	thread = (pthread_t *)malloc(realNbThreads * sizeof(pthread_t)); 
	if (thread == NULL) {
		alloc_error("threads");
	}
	for (uint32_t i = 0; i < realNbThreads; ++i) {
		arg[i].fractal_table = fractal_table;
		arg[i].fractal = fractal;
		arg[i].clipRect = &rectangle[i];
		arg[i].quadInterpolationSize = quadInterpolationSize;
		arg[i].interpolationThreshold = interpolationThreshold;

                safePThreadCreate(&(thread[i]),NULL,ComputeFractalThreadRoutine,(void *)&arg[i]);
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

	info(T_NORMAL,"Computing fractal : DONE.\n");
}

inline void ComputeFractal(FloatTable *fractal_table, Fractal *fractal)
{
	ComputeFractalFast(fractal_table, fractal, 1, 0);
}

void *RenderFractalThreadRoutine(void *arg)
{
	RenderFractalArguments *c_arg = (RenderFractalArguments *)arg;
	Image *image = c_arg->image;
	FloatTable *fractal_table = c_arg->fractal_table;
	Rectangle *clipRect = c_arg->clipRect;
	Gradient *gradient = c_arg->gradient;
	Color spaceColor = c_arg->spaceColor;
	FLOAT multiplier  = c_arg->multiplier;
	uint32_t width = fractal_table->width;
	uint32_t height = fractal_table->height;

	info(T_VERBOSE,"Rendering fractal from (%lu,%lu) to (%lu,%lu)...\n",
		(unsigned long)clipRect->x1,(unsigned long)clipRect->y1,
		(unsigned long)clipRect->x2,(unsigned long)clipRect->y2);

	if (width<2 || height<2) {
		error("Table is too small : %lux%lu\n",
		(unsigned long)width,(unsigned long)height);
	}

	FLOAT value = -1;
	Color color;
	FLOAT multiplier2 = multiplier*multiplier;
	for (uint32_t i=clipRect->y1; i<=clipRect->y2; i++) {
		for (uint32_t j=clipRect->x1; j<=clipRect->x2; j++) {
			value = GetDataUnsafe(fractal_table, j, i);
			color = (value < 0) ? spaceColor: GetGradientColor(gradient,
							(uint32_t)(value*multiplier2));
			PutPixelUnsafe(image,j,i,color);
		}
	}

	info(T_VERBOSE,"Rendering fractal from (%lu,%lu) to (%lu,%lu) : DONE.\n",
		(unsigned long)clipRect->x1,(unsigned long)clipRect->y1,
		(unsigned long)clipRect->x2,(unsigned long)clipRect->y2);

	return NULL;
}

void RenderFractal(Image *image, FloatTable *fractal_table, RenderingParameters *param)
{
	info(T_NORMAL,"Rendering fractal...\n");

	uint32_t nbPixels = image->width*image->height;
	uint32_t realNbThreads = (nbThreads > nbPixels) ? nbPixels : nbThreads;

	Rectangle *rectangle;
	rectangle = (Rectangle *)malloc(realNbThreads * sizeof(Rectangle));
	if (rectangle == NULL) {
		alloc_error("rectangles");
	}
	InitRectangle(&rectangle[0], 0, 0, image->width-1, image->height-1);
	if (CutRectangleInN(rectangle[0], realNbThreads, rectangle)) {
		error("Could not cut rectangle ((%lu,%lu),(%lu,%lu) in %lu parts.\n",
			(unsigned long)rectangle[0].x1, (unsigned long)rectangle[0].y1,
			(unsigned long)rectangle[0].x2, (unsigned long)rectangle[0].y2,
			(unsigned long)realNbThreads);
	}
	
	RenderFractalArguments *arg;
	arg = (RenderFractalArguments *)malloc(realNbThreads * sizeof(RenderFractalArguments));
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
		arg[i].fractal_table = fractal_table;
		arg[i].clipRect = &rectangle[i];
		arg[i].gradient = &param->gradient;
		arg[i].spaceColor = param->spaceColor;
		arg[i].multiplier = param->multiplier;

                safePThreadCreate(&(thread[i]),NULL,RenderFractalThreadRoutine,(void *)&arg[i]);
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

	info(T_NORMAL,"Rendering fractal : DONE.\n");
}

