/*
 *  filter.c -- part of fractal2D
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
 
#include "filter.h"
#include "rectangle.h"
#include "thread.h"

typedef struct s_ApplyFilterArguments {
	Image *dst;
	Rectangle *dstRect;
	Image *src;
	Filter *filter;
} ApplyFilterArguments;

void InitFilter(Filter *filter, uint32_t sx, uint32_t sy, uint32_t cx, uint32_t cy, FLOAT *data)
{
	filter->sx = sx;
	filter->sy = sy;
	filter->cx = cx;
	filter->cy = cy;
	filter->data = data;
}

void InitFilter2(Filter *filter, uint32_t sx, uint32_t sy, FLOAT *data)
{
	filter->sx = sx;
	filter->sy = sy;
	filter->cx = (sx-1) / 2;
	filter->cy = (sy-1) / 2;
	filter->data = data;
}

void CreateHorizontalGaussianFilter(Filter *filter, FLOAT sigma)
{
	if (sigma <= 0) {
		error("Sigma must be > 0.\n");
	}

	uint32_t radius = ceil(3. * sigma);
	if (radius % 2 == 0) {
		++radius;
	}

	filter->sx = radius*2+1;
	filter->sy = 1;
	filter->cx = radius;
	filter->cy = 0;

	filter->data = (FLOAT *)malloc(filter->sx*filter->sy*sizeof(FLOAT));
	if (filter->data == NULL) {
		alloc_error("filter");
	}


	FLOAT sigma2_x_2 = sigma*sigma*2;
	FLOAT *value = filter->data;
	int64_t size = (int64_t)filter->cx;
	FLOAT sum = 0;
	for (int64_t i = -size; i <= size; ++i) {
		*value = exp(-i*i/sigma2_x_2);
		sum += *(value++);
	}
	if (sum != 0.) {
		MultiplyFilterByScalar(filter, 1./sum);
	}
}

inline void CreateHorizontalGaussianFilter2(Filter *filter, FLOAT radius)
{
	if (radius <= 0) {
		error("Radius must be > 0.\n");
	}

	CreateHorizontalGaussianFilter(filter, radius / 3.);
}

void CreateVerticalGaussianFilter(Filter *filter, FLOAT sigma)
{
	if (sigma <= 0) {
		error("Sigma must be > 0.\n");
	}

	uint32_t radius = ceil(3. * sigma);
	if (radius % 2 == 0) {
		++radius;
	}

	filter->sx = 1;
	filter->sy = radius*2+1;
	filter->cx = 0;
	filter->cy = radius;

	filter->data = (FLOAT *)malloc(filter->sx*filter->sy*sizeof(FLOAT));
	if (filter->data == NULL) {
		alloc_error("filter");
	}

	FLOAT sigma2_x_2 = sigma*sigma*2;
	FLOAT *value = filter->data;
	int64_t size = (int64_t)filter->cy;
	FLOAT sum = 0;
	for (int64_t i = -size; i <= size; ++i) {
		*value = exp(-i*i/sigma2_x_2);
		sum += *(value++);
	}
	if (sum != 0.) {
		MultiplyFilterByScalar(filter, 1./sum);
	}
}

inline void CreateVerticalGaussianFilter2(Filter *filter, FLOAT radius)
{
	if (radius <= 0) {
		error("Radius must be > 0.\n");
	}

	CreateVerticalGaussianFilter(filter, radius / 3.);
}

void CreateGaussianFilter(Filter *filter, FLOAT sigma)
{
	if (sigma <= 0) {
		error("Sigma must be > 0.\n");
	}
	uint32_t radius = ceil(3. * sigma);
	if (radius % 2 == 0) {
		++radius;
	}

	filter->sx = radius*2+1;
	filter->sy = radius*2+1;
	filter->cx = radius;
	filter->cy = radius;

	filter->data = (FLOAT *)malloc(filter->sx*filter->sy*sizeof(FLOAT));
	if (filter->data == NULL) {
		alloc_error("filter");
	}

	FLOAT sigma2_x_2 = sigma*sigma*2;
	FLOAT *value = filter->data;
	int64_t size = (int64_t)filter->cx;
	FLOAT sum = 0;
	for (int64_t i = -size; i <= size; ++i) {
		for (int64_t j = -size; j <= size; ++j) {
			*value = exp(-(i*i+j*j)/sigma2_x_2);
			sum += *(value++);
		}
	}
	if (sum != 0.) {
		MultiplyFilterByScalar(filter, 1./sum);
	}
}

void CreateGaussianFilter2(Filter *filter, FLOAT radius)
{
	if (radius <= 0) {
		error("Radius must be > 0.\n");
	}
	CreateGaussianFilter(filter, radius / 3.);
}

inline FLOAT GetFilterValueUnsafe(Filter *filter, uint32_t x, uint32_t y)
{
	return filter->data[x+y*filter->sx];
}

void MultiplyFilterByScalar(Filter *filter, FLOAT scalar) 
{
	FLOAT *value = filter->data;
	for (uint32_t i = 0; i < filter->sx; ++i) {
		for (uint32_t j = 0; j < filter->sy; ++j) {
			*(value++) *= scalar;
		}
	}
}

int NormalizeFilter(Filter *filter)
{
	FLOAT *value = filter->data;
	FLOAT sum = 0;
	for (uint32_t i = 0; i < filter->sx; ++i) {
		for (uint32_t j = 0; j < filter->sy; ++j) {
			sum += *(value++);
		}
	}
	if (sum != 0.) {
		// Otherwize do nothing : can't be normalized anyway
		MultiplyFilterByScalar(filter, 1./sum);

		return 0;
	} else {
		return 1;
	}
}

Color ApplyFilterOnSinglePixel(Image *src, uint32_t x, uint32_t y, Filter *filter)
{
	FLOAT value;
	Color color;

	FLOAT r, g, b;

	r = 0;
	g = 0;
	b = 0;
	for (uint32_t i = 0; i < filter->sx; ++i) {
		for (uint32_t j = 0; j < filter->sy; ++j) {
			color = iGetPixel(src, x-filter->cx+i, y-filter->cy+j);
			value = GetFilterValueUnsafe(filter, i, j);
			r += color.r * value;
			g += color.g * value;
			b += color.b * value;
		}
	}

	color.r = r;
	color.g = g;
	color.b = b;

	return color;
}

void *ApplyFilterThreadRoutine(void *arg)
{
	ApplyFilterArguments *c_arg = (ApplyFilterArguments *)arg;
	Image *dst = c_arg->dst;
	Rectangle *dstRect = c_arg->dstRect;
	Image *src = c_arg->src;
	Filter *filter = c_arg->filter;

	info(T_VERBOSE,"Applying filter from (%lu,%lu) to (%lu,%lu)...\n",
			(unsigned long)dstRect->x1,(unsigned long)dstRect->y1,
			(unsigned long)dstRect->x2,(unsigned long)dstRect->y2);

	for (uint32_t i=dstRect->x1; i <= dstRect->x2; ++i) {
		for (uint32_t j = dstRect->y1; j <= dstRect->y2; ++j) {
			PutPixelUnsafe(dst, i, j, ApplyFilterOnSinglePixel(src, i, j, filter));
		}
	}

	info(T_VERBOSE,"Applying filter from (%lu,%lu) to (%lu,%lu) : DONE.\n",
		(unsigned long)dstRect->x1,(unsigned long)dstRect->y1,
		(unsigned long)dstRect->x2,(unsigned long)dstRect->y2);

	return NULL;
}

void ApplyFilter(Image *dst, Image *src, Filter *filter)
{
	info(T_NORMAL,"Applying filter...\n");

	uint32_t nbPixels = src->width*src->height;
	uint32_t realNbThreads = (nbThreads > nbPixels) ? nbPixels : nbThreads;

	Rectangle *rectangle;
	rectangle = (Rectangle *)malloc(realNbThreads * sizeof(Rectangle));
	if (rectangle == NULL) {
		alloc_error("rectangles");
	}
	InitRectangle(&rectangle[0], 0, 0, dst->width-1, dst->height-1);
	if (CutRectangleInN(rectangle[0], realNbThreads, rectangle)) {
		error("Could not cut rectangle ((%lu,%lu),(%lu,%lu) in %lu parts.\n",
			(unsigned long)rectangle[0].x1, (unsigned long)rectangle[0].y1,
			(unsigned long)rectangle[0].x2, (unsigned long)rectangle[0].y2,
			(unsigned long)realNbThreads);
	}

        pthread_t *thread;
	thread = (pthread_t *)malloc(realNbThreads * sizeof(pthread_t)); 
	if (thread == NULL) {
		alloc_error("threads");
	}
	ApplyFilterArguments *arg;
	arg = (ApplyFilterArguments *)malloc(realNbThreads * sizeof(ApplyFilterArguments));
	if (arg == NULL) {
		alloc_error("arguments");
	}
	for (uint32_t i = 0; i < realNbThreads; ++i) {
		arg[i].dst = dst;
		arg[i].dstRect = &rectangle[i];
		arg[i].src = src;
		arg[i].filter = filter;

                safePThreadCreate(&(thread[i]),NULL,ApplyFilterThreadRoutine,(void *)&arg[i]);
	}

	void *status;
	for (uint32_t i = 0; i < realNbThreads; ++i) {
		safePThreadJoin(thread[i], &status);

		if (status != NULL) {
			error("Thread ended because of an error.\n");
		}
	}

	free(rectangle);
	free(thread);
	free(arg);

	info(T_NORMAL,"Applying filter : DONE.\n");
}

void FreeFilter(Filter *filter)
{
	free(filter->data);
}

