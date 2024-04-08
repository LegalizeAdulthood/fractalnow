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
#include "misc.h"
#include "rectangle.h"
#include <inttypes.h>
#include <signal.h>
#include <string.h>

typedef struct s_ApplyFilterArguments {
	Image *dst;
	Rectangle *dstRect;
	Image *src;
	Filter *filter;
} ApplyFilterArguments;

void FreeApplyFilterArguments(void *arg)
{
	ApplyFilterArguments *c_arg = (ApplyFilterArguments *)arg;
	free(c_arg->dstRect);
	FreeFilter(*c_arg->filter);
	free(c_arg->filter);
}

void InitFilter(Filter *filter, uint_fast32_t sx, uint_fast32_t sy, uint_fast32_t cx, uint_fast32_t cy, FLOAT *data)
{
	filter->sx = sx;
	filter->sy = sy;
	filter->cx = cx;
	filter->cy = cy;
	filter->data = data;
}

void InitFilter2(Filter *filter, uint_fast32_t sx, uint_fast32_t sy, FLOAT *data)
{
	filter->sx = sx;
	filter->sy = sy;
	filter->cx = (sx-1) / 2;
	filter->cy = (sy-1) / 2;
	filter->data = data;
}

Filter *CopyFilter(Filter *filter)
{
	Filter *res = (Filter *)safeMalloc("filter", sizeof(Filter));
	FLOAT *data = (FLOAT *)safeMalloc("filter data", filter->sx*filter->sy*sizeof(FLOAT));
	memcpy(data, filter->data, filter->sx*filter->sy*sizeof(FLOAT));

	InitFilter(res, filter->sx, filter->sy, filter->cx, filter->cy, data);

	return res;
}

void CreateHorizontalGaussianFilter(Filter *filter, FLOAT sigma)
{
	if (sigma <= 0) {
		error("Sigma must be > 0.\n");
	}

	uint_fast32_t radius = floorF(3. * sigma);

	filter->sx = radius*2+1;
	filter->sy = 1;
	filter->cx = radius;
	filter->cy = 0;
	filter->data = (FLOAT *)safeMalloc("filter", filter->sx*filter->sy*sizeof(FLOAT));

	FLOAT sigma2_x_2 = sigma*sigma*2;
	FLOAT *value = filter->data;
	int_fast64_t size = (int_fast64_t)filter->cx;
	FLOAT sum = 0;
	for (int_fast64_t i = -size; i <= size; ++i) {
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

	uint_fast32_t radius = floorF(3. * sigma);

	filter->sx = 1;
	filter->sy = radius*2+1;
	filter->cx = 0;
	filter->cy = radius;
	filter->data = (FLOAT *)safeMalloc("filter", filter->sx*filter->sy*sizeof(FLOAT));

	FLOAT sigma2_x_2 = sigma*sigma*2;
	FLOAT *value = filter->data;
	int_fast64_t size = (int_fast64_t)filter->cy;
	FLOAT sum = 0;
	for (int_fast64_t i = -size; i <= size; ++i) {
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
	uint_fast32_t radius = floorF(3. * sigma);

	filter->sx = radius*2+1;
	filter->sy = radius*2+1;
	filter->cx = radius;
	filter->cy = radius;
	filter->data = (FLOAT *)safeMalloc("filter", filter->sx*filter->sy*sizeof(FLOAT));

	FLOAT sigma2_x_2 = sigma*sigma*2;
	FLOAT *value = filter->data;
	int_fast64_t size = (int_fast64_t)filter->cx;
	FLOAT sum = 0;
	for (int_fast64_t i = -size; i <= size; ++i) {
		for (int_fast64_t j = -size; j <= size; ++j) {
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

inline FLOAT GetFilterValueUnsafe(Filter *filter, uint_fast32_t x, uint_fast32_t y)
{
	return filter->data[x+y*filter->sx];
}

void MultiplyFilterByScalar(Filter *filter, FLOAT scalar) 
{
	FLOAT *value = filter->data;
	for (uint_fast32_t i = 0; i < filter->sx; ++i) {
		for (uint_fast32_t j = 0; j < filter->sy; ++j) {
			*(value++) *= scalar;
		}
	}
}

int NormalizeFilter(Filter *filter)
{
	FLOAT *value = filter->data;
	FLOAT sum = 0;
	for (uint_fast32_t i = 0; i < filter->sx; ++i) {
		for (uint_fast32_t j = 0; j < filter->sy; ++j) {
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

Color ApplyFilterOnSinglePixel(Image *src, uint_fast32_t x, uint_fast32_t y, Filter *filter)
{
	FLOAT value;
	Color color;

	FLOAT r, g, b;

	r = 0;
	g = 0;
	b = 0;
	for (uint_fast32_t i = 0; i < filter->sx; ++i) {
		for (uint_fast32_t j = 0; j < filter->sy; ++j) {
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
	volatile sig_atomic_t *cancel = *((volatile sig_atomic_t **)arg);
	uint8_t *arg1 = (uint8_t *)arg;
	ApplyFilterArguments *c_arg = (ApplyFilterArguments *)(arg1+sizeof(volatile sig_atomic_t *));
	Image *dst = c_arg->dst;
	Rectangle *dstRect = c_arg->dstRect;
	Image *src = c_arg->src;
	Filter *filter = c_arg->filter;

	info(T_VERBOSE,"Applying filter from (%"PRIuFAST32",%"PRIuFAST32") to \
(%"PRIuFAST32",%"PRIuFAST32")...\n", dstRect->x1, dstRect->y1, dstRect->x2, dstRect->y2);

	for (uint_fast32_t i=dstRect->x1; i <= dstRect->x2 && !(*cancel); ++i) {
		for (uint_fast32_t j = dstRect->y1; j <= dstRect->y2 && !(*cancel); ++j) {
			PutPixelUnsafe(dst, i, j, ApplyFilterOnSinglePixel(src, i, j, filter));
		}
	}

	info(T_VERBOSE,"Applying filter from (%"PRIuFAST32",%"PRIuFAST32") to \
(%"PRIuFAST32",%"PRIuFAST32") : %s.\n", dstRect->x1, dstRect->y1, dstRect->x2,
		dstRect->y2, (*cancel) ? "CANCELED" : "DONE");

	return ((*cancel) ? PTHREAD_CANCELED : NULL);
}

inline Action *LaunchApplyFilter(Image *dst, Image *src, Filter *filter)
{
	if (src->width == 0 || src->height == 0) {
		return DoNothingAction();
	}

	uint_fast32_t nbPixels = src->width*src->height;
	uint_fast32_t realNbThreads = (nbThreads > nbPixels) ? nbPixels : nbThreads;

	Rectangle *rectangle;
	rectangle = (Rectangle *)safeMalloc("rectangles", realNbThreads * sizeof(Rectangle));
	InitRectangle(&rectangle[0], 0, 0, dst->width-1, dst->height-1);
	if (CutRectangleInN(rectangle[0], realNbThreads, rectangle)) {
		error("Could not cut rectangle ((%"PRIuFAST32",%"PRIuFAST32"),\
(%"PRIuFAST32",%"PRIuFAST32") in %"PRIuFAST32" parts.\n", rectangle[0].x1, rectangle[0].y1,
			rectangle[0].x2, rectangle[0].y2, realNbThreads);
	}

	ApplyFilterArguments *arg;
	arg = (ApplyFilterArguments *)safeMalloc("arguments", realNbThreads * sizeof(ApplyFilterArguments));
	for (uint_fast32_t i = 0; i < realNbThreads; ++i) {
		arg[i].dst = dst;
		arg[i].dstRect = CopyRectangle(&rectangle[i]);
		arg[i].src = src;
		arg[i].filter = CopyFilter(filter);
	}
	Action *res = LaunchThreads("Applying filter", realNbThreads, arg, sizeof(ApplyFilterArguments),
					ApplyFilterThreadRoutine, FreeApplyFilterArguments);

	free(rectangle);
	free(arg);

	return res;
}

void ApplyFilter(Image *dst, Image *src, Filter *filter)
{
	Action *action = LaunchApplyFilter(dst, src, filter);
	int unused = WaitForFinished(action);
	(void)unused;
	FreeAction(*action);
	free(action);
}

void FreeFilter(Filter filter)
{
	free(filter.data);
}

