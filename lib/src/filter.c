/*
 *  filter.c -- part of FractalNow
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

#define HandleRequests(max_counter) \
if (counter == max_counter) {\
	HandlePauseRequest(threadArgHeader);\
	cancelRequested =\
		CancelTaskRequested(threadArgHeader);\
	counter = 0;\
} else {\
	++counter;\
}

typedef struct s_ApplyFilterArguments {
	uint_fast32_t threadId;
	Image *dst;
	uint_fast32_t nbRectangles;
	Rectangle *rectangles;
	const Image *src;
	Filter *filter;
} ApplyFilterArguments;

void FreeApplyFilterArguments(void *arg)
{
	ApplyFilterArguments *c_arg = (ApplyFilterArguments *)arg;
	if (c_arg->threadId == 0) {
		free(c_arg->rectangles);
		FreeFilter(*c_arg->filter);
		free(c_arg->filter);
	}
}

void InitFilter(Filter *filter, uint_fast32_t sx, uint_fast32_t sy,
		uint_fast32_t cx, uint_fast32_t cy, FLOATT *data)
{
	filter->sx = sx;
	filter->sy = sy;
	filter->cx = cx;
	filter->cy = cy;
	filter->data = data;
}

void InitFilter2(Filter *filter, uint_fast32_t sx, uint_fast32_t sy, FLOATT *data)
{
	filter->sx = sx;
	filter->sy = sy;
	filter->cx = (sx-1) / 2;
	filter->cy = (sy-1) / 2;
	filter->data = data;
}

Filter CopyFilter(const Filter *filter)
{
	Filter res;
	FLOATT *data = (FLOATT *)safeMalloc("filter data", filter->sx*filter->sy*sizeof(FLOATT));
	memcpy(data, filter->data, filter->sx*filter->sy*sizeof(FLOATT));

	InitFilter(&res, filter->sx, filter->sy, filter->cx, filter->cy, data);

	return res;
}

void CreateHorizontalGaussianFilter(Filter *filter, FLOATT sigma)
{
	if (sigma <= 0) {
		FractalNow_error("Sigma must be > 0.\n");
	}

	uint_fast32_t radius = floorF(3. * sigma);

	filter->sx = radius*2+1;
	filter->sy = 1;
	filter->cx = radius;
	filter->cy = 0;
	filter->data = (FLOATT *)safeMalloc("filter", filter->sx*filter->sy*sizeof(FLOATT));

	FLOATT sigma2_x_2 = sigma*sigma*2;
	FLOATT *value = filter->data;
	int_fast64_t size = (int_fast64_t)filter->cx;
	FLOATT sum = 0;
	for (int_fast64_t i = -size; i <= size; ++i) {
		*value = expF(-i*i/sigma2_x_2);
		sum += *(value++);
	}
	if (sum != 0.) {
		MultiplyFilterByScalar(filter, 1./sum);
	}
}

inline void CreateHorizontalGaussianFilter2(Filter *filter, FLOATT radius)
{
	if (radius <= 0) {
		FractalNow_error("Radius must be > 0.\n");
	}

	CreateHorizontalGaussianFilter(filter, radius / 3.);
}

void CreateVerticalGaussianFilter(Filter *filter, FLOATT sigma)
{
	if (sigma <= 0) {
		FractalNow_error("Sigma must be > 0.\n");
	}

	uint_fast32_t radius = floorF(3. * sigma);

	filter->sx = 1;
	filter->sy = radius*2+1;
	filter->cx = 0;
	filter->cy = radius;
	filter->data = (FLOATT *)safeMalloc("filter", filter->sx*filter->sy*sizeof(FLOATT));

	FLOATT sigma2_x_2 = sigma*sigma*2;
	FLOATT *value = filter->data;
	int_fast64_t size = (int_fast64_t)filter->cy;
	FLOATT sum = 0;
	for (int_fast64_t i = -size; i <= size; ++i) {
		*value = expF(-i*i/sigma2_x_2);
		sum += *(value++);
	}
	if (sum != 0.) {
		MultiplyFilterByScalar(filter, 1./sum);
	}
}

inline void CreateVerticalGaussianFilter2(Filter *filter, FLOATT radius)
{
	if (radius <= 0) {
		FractalNow_error("Radius must be > 0.\n");
	}

	CreateVerticalGaussianFilter(filter, radius / 3.);
}

void CreateGaussianFilter(Filter *filter, FLOATT sigma)
{
	if (sigma <= 0) {
		FractalNow_error("Sigma must be > 0.\n");
	}
	uint_fast32_t radius = floorF(3. * sigma);

	filter->sx = radius*2+1;
	filter->sy = radius*2+1;
	filter->cx = radius;
	filter->cy = radius;
	filter->data = (FLOATT *)safeMalloc("filter", filter->sx*filter->sy*sizeof(FLOATT));

	FLOATT sigma2_x_2 = sigma*sigma*2;
	FLOATT *value = filter->data;
	int_fast64_t size = (int_fast64_t)filter->cx;
	FLOATT sum = 0;
	for (int_fast64_t i = -size; i <= size; ++i) {
		for (int_fast64_t j = -size; j <= size; ++j) {
			*value = expF(-(i*i+j*j)/sigma2_x_2);
			sum += *(value++);
		}
	}
	if (sum != 0.) {
		MultiplyFilterByScalar(filter, 1./sum);
	}
}

void CreateGaussianFilter2(Filter *filter, FLOATT radius)
{
	if (radius <= 0) {
		FractalNow_error("Radius must be > 0.\n");
	}
	CreateGaussianFilter(filter, radius / 3.);
}

inline FLOATT GetFilterValueUnsafe(const Filter *filter, uint_fast32_t x, uint_fast32_t y)
{
	return filter->data[x+y*filter->sx];
}

void MultiplyFilterByScalar(Filter *filter, FLOATT scalar) 
{
	FLOATT *value = filter->data;
	for (uint_fast32_t i = 0; i < filter->sx; ++i) {
		for (uint_fast32_t j = 0; j < filter->sy; ++j) {
			*(value++) *= scalar;
		}
	}
}

int NormalizeFilter(Filter *filter)
{
	FLOATT *value = filter->data;
	FLOATT sum = 0;
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

Color ApplyFilterOnSinglePixel(const Image *src, uint_fast32_t x, uint_fast32_t y,
				const Filter *filter)
{
	FLOATT value;
	Color color;

	FLOATT r, g, b;

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

	color.bytesPerComponent = src->bytesPerComponent;
	color.r = r;
	color.g = g;
	color.b = b;

	return color;
}

void *ApplyFilterThreadRoutine(void *arg)
{
	ThreadArgHeader *threadArgHeader = GetThreadArgHeader(arg);
	ApplyFilterArguments *c_arg = (ApplyFilterArguments *)GetThreadArgBody(arg);
	Image *dst = c_arg->dst;
	const Image *src = c_arg->src;
	Filter *filter = c_arg->filter;

	uint_fast32_t nbRectangles = c_arg->nbRectangles;
	Rectangle *dstRect;
	uint_fast32_t rectHeight;
	uint_fast32_t counter = 0;
	int cancelRequested = CancelTaskRequested(threadArgHeader);
	for (uint_fast32_t i = 0; i < nbRectangles && !cancelRequested; ++i) {
		dstRect = &c_arg->rectangles[i];
		rectHeight = dstRect->y2+1 - dstRect->y1;

		for (uint_fast32_t j = dstRect->y1; j <= dstRect->y2 && !cancelRequested; ++j) {
			SetThreadProgress(threadArgHeader, 100 * (i * rectHeight + (j-dstRect->y1)) /
								(rectHeight * nbRectangles));
			for (uint_fast32_t k = dstRect->x1; k <= dstRect->x2
					&& !cancelRequested; ++k) {
				HandleRequests(32);

				PutPixelUnsafe(dst, k, j, ApplyFilterOnSinglePixel(src, k, j, filter));
			}
		}
	}
	SetThreadProgress(threadArgHeader, 100);

	int canceled = CancelTaskRequested(threadArgHeader);

	return (canceled ? PTHREAD_CANCELED : NULL);
}

char applyFilterMessage[] = "Applying filter";

Task *CreateApplyFilterTask(Image *dst, const Image *src, const Filter *filter, uint_fast32_t nbThreads)
{
	if (src->width == 0 || src->height == 0) {
		return DoNothingTask();
	}

	uint_fast32_t nbPixels = src->width*src->height;
	uint_fast32_t nbThreadsNeeded = nbThreads;
	uint_fast32_t rectanglesPerThread = DEFAULT_RECTANGLES_PER_THREAD;
	if (nbPixels <= nbThreadsNeeded) {
		nbThreadsNeeded = nbPixels;
		rectanglesPerThread = 1;
	} else if (nbPixels < nbThreadsNeeded*rectanglesPerThread) {
		rectanglesPerThread = nbPixels / nbThreadsNeeded;
	}
	uint_fast32_t nbRectangles = nbThreadsNeeded*rectanglesPerThread;

	Rectangle *rectangle;
	rectangle = (Rectangle *)safeMalloc("rectangles", nbRectangles * sizeof(Rectangle));
	InitRectangle(&rectangle[0], 0, 0, dst->width-1, dst->height-1);
	if (CutRectangleInN(rectangle[0], nbRectangles, rectangle)) {
		FractalNow_error("Could not cut rectangle ((%"PRIuFAST32",%"PRIuFAST32"),\
(%"PRIuFAST32",%"PRIuFAST32") in %"PRIuFAST32" parts.\n", rectangle[0].x1, rectangle[0].y1,
			rectangle[0].x2, rectangle[0].y2, nbRectangles);
	}

	ApplyFilterArguments *arg;
	arg = (ApplyFilterArguments *)safeMalloc("arguments", nbThreadsNeeded *
							sizeof(ApplyFilterArguments));
	Filter *copyFilter = (Filter *)safeMalloc("copy filter", sizeof(Filter));
	*copyFilter = CopyFilter(filter);
	for (uint_fast32_t i = 0; i < nbThreadsNeeded; ++i) {
		arg[i].threadId = i;
		arg[i].dst = dst;
		arg[i].nbRectangles = rectanglesPerThread;
		arg[i].rectangles = &rectangle[i*rectanglesPerThread];
		arg[i].src = src;
		arg[i].filter = copyFilter;
	}
	Task *res = CreateTask(applyFilterMessage, nbThreadsNeeded, arg,
					sizeof(ApplyFilterArguments), ApplyFilterThreadRoutine,
					FreeApplyFilterArguments);

	free(arg);

	return res;
}

void ApplyFilter(Image *dst, const Image *src, const Filter *filter, Threads *threads)
{
	Task *task = CreateApplyFilterTask(dst, src, filter, threads->N);
	int unused = ExecuteTaskBlocking(task, threads);
	UNUSED(unused);
}

void FreeFilter(Filter filter)
{
	free(filter.data);
}

