/*
 *  image.c -- part of FractalNow
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
 
#include "image.h"
#include "filter.h"
#include "misc.h"
#include "uirectangle.h"
#include <inttypes.h>
#include <stdlib.h>
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

inline void CreateImage(Image *image, uint_fast32_t width, uint_fast32_t height,
			uint_fast8_t bytesPerComponent)
{
	if (bytesPerComponent != 1 && bytesPerComponent != 2) {
		FractalNow_error("Invalid bytes per component. Only 1 and 2 are allowed.\n");
	}
	image->data = (uint8_t *)safeCalloc("image data", width*height, 4*bytesPerComponent);
	image->data_is_external = 0;
	image->width = width;
	image->height = height;
	image->bytesPerComponent = bytesPerComponent;
}

void CreateImage2(Image *image, uint8_t *data, uint_fast32_t width, uint_fast32_t height,
			uint_fast8_t bytesPerComponent)
{
	if (bytesPerComponent != 1 && bytesPerComponent != 2) {
		FractalNow_error("Invalid bytes per component. Only 1 and 2 are allowed.\n");
	}
	image->data = data;
	image->data_is_external = 1;
	image->width = width;
	image->height = height;
	image->bytesPerComponent = bytesPerComponent;
}

Image CloneImage(const Image *image)
{
	Image res;
	CreateImage(&res, image->width, image->height, image->bytesPerComponent);
	memcpy(res.data, image->data, image->width*image->height*4*image->bytesPerComponent);

	return res;
}

static inline uint8_t *ImageRGB8ToBytesArray(const Image *image)
{
	if (image->width == 0 || image->height == 0) {
		return NULL;
	}

	uint8_t *res = (uint8_t *)safeCalloc("image bytes array", image->width*image->height,
						3*image->bytesPerComponent);

	uint32_t *p_data32 = (uint32_t *)image->data;
	uint32_t data32;
	uint8_t *p_res = res;
	for (uint_fast32_t i = 0; i < image->height; ++i) {
		for (uint_fast32_t j = 0; j < image->width; ++j) {
			data32 = *p_data32;
			*(p_res++) = GET_R8(data32);
			*(p_res++) = GET_G8(data32);
			*(p_res++) = GET_B8(data32);
			++p_data32;
		}
	}

	return res;
}

static inline uint8_t *ImageRGB16ToBytesArray(const Image *image)
{
	if (image->width == 0 || image->height == 0) {
		return NULL;
	}

	uint8_t *res = (uint8_t *)safeCalloc("image bytes array", image->width*image->height,
						3*image->bytesPerComponent);

	uint64_t *p_data64 = (uint64_t *)image->data;
	uint64_t data64;
	uint16_t r, g, b;
	uint8_t *p_res = res;
	for (uint_fast32_t i = 0; i < image->height; ++i) {
		for (uint_fast32_t j = 0; j < image->width; ++j) {
			data64 = *p_data64;
			/* We want the components in the order RGB, so we need to extract
			 * them from uint64_t (the machine can be MSB first, or LSB first..)
			 */
			r = GET_R16(data64);
			g = GET_G16(data64);
			b = GET_B16(data64);
			/* Then we want for each component the most significant byte first.*/
			*(p_res++) = r >> 8;
			*(p_res++) = r & 0xFF;
			*(p_res++) = g >> 8;
			*(p_res++) = g & 0xFF;
			*(p_res++) = b >> 8;
			*(p_res++) = b & 0xFF;
			++p_data64;
		}
	}

	return res;
}

uint8_t *ImageToBytesArray(const Image *image)
{
	uint8_t *res;
	switch(image->bytesPerComponent) {
	case 1:
		res = ImageRGB8ToBytesArray(image);
		break;
	case 2:
		res = ImageRGB16ToBytesArray(image);
		break;
	default:
		FractalNow_error("Invalid bytes per component (%"PRIuFAST8").\n",
			image->bytesPerComponent);
		break;
	}

	return res;
}

typedef enum {
	LEFT = 0, TOP, RIGHT, BOTTOM, TOPLEFT, TOPRIGHT, BOTTOMRIGHT, BOTTOMLEFT, CENTER
} Region;

inline Color iGetPixelUnsafe(const Image *image, uint_fast32_t x, uint_fast32_t y) {
	Color res;
	res.bytesPerComponent = image->bytesPerComponent;
	switch (res.bytesPerComponent) {
	case 1:
		{
		uint32_t data32 = *((uint32_t *)(image->data) + y*image->width+x);
		res.r = GET_R8(data32);
		res.g = GET_G8(data32);
		res.b = GET_B8(data32);
		break;
		}
	case 2:
		{
		uint64_t data64 = *((uint64_t *)(image->data) + y*image->width+x);
		res.r = GET_R16(data64);
		res.g = GET_G16(data64);
		res.b = GET_B16(data64);
		}
		break;
	default:
		FractalNow_error("Invalid bytes per component.\n");
		break;
	}
	
	return res;
}

Region iGetImageRegion(const Image *image, int_fast64_t x, int_fast64_t y)
{
	Region region;

	if (x < 0) {
		if (y < 0) {
			region = TOPLEFT;
		} else if ((uint_fast32_t)y >= image->height) {
			region = BOTTOMLEFT;
		} else {
			region = LEFT;
		}
	} else if ((uint_fast32_t)x >= image->width) {
		if (y < 0) {
			region = TOPRIGHT;
		} else if ((uint_fast32_t)y >= image->height) {
			region = BOTTOMRIGHT;
		} else {
			region = RIGHT;
		}
	} else {
		if (y < 0) {
			region = TOP;
		} else if ((uint_fast32_t)y >= image->height) {
			region = BOTTOM;
		} else {
			region = CENTER;
		}
	}

	return region;
}

Color blackColorRGB8 = {1, 0, 0, 0};
Color blackColorRGB16 = {2, 0, 0, 0};

Color iGetPixel(const Image *image, int_fast64_t x, int_fast64_t y) {
	if (image->width == 0 || image->height == 0) {
		if (image->bytesPerComponent == 1) {
			return blackColorRGB8;
		} else {
			return blackColorRGB16;
		}
	}

	Region region = iGetImageRegion(image, x, y);
	Color res;

	switch (region) {
	case TOPLEFT:
		res = iGetPixelUnsafe(image, 0, 0);
		break;
	case TOPRIGHT:
		res = iGetPixelUnsafe(image, image->width-1, 0);
		break;
	case BOTTOMLEFT:
		res = iGetPixelUnsafe(image, 0, image->height-1);
		break;
	case BOTTOMRIGHT:
		res = iGetPixelUnsafe(image, image->width-1, image->height-1);
		break;
	case TOP:
		res = iGetPixelUnsafe(image, x, 0);
		break;
	case LEFT:
		res = iGetPixelUnsafe(image, 0, y);
		break;
	case RIGHT:
		res = iGetPixelUnsafe(image, image->width-1, y);
		break;
	case BOTTOM:
		res = iGetPixelUnsafe(image, x, image->height-1);
		break;
	case CENTER:
		res = iGetPixelUnsafe(image, x, y);
		break;
	default:
		FractalNow_error("Unknown region.\n");
		break;
	}
	return res;
}


inline void PutPixelUnsafe(Image *image, uint_fast32_t x, uint_fast32_t y, Color color)
{
	switch (image->bytesPerComponent) {
	case 1:
		{
		uint32_t *data32 = (uint32_t *)(image->data) + y*image->width+x;
		*data32 = RGB8_TO_UINT32(color.r, color.g, color.b);
		break;
		}
	case 2:
		{
		uint64_t *data64 = (uint64_t *)(image->data) + y*image->width+x;
		*data64 = RGB16_TO_UINT64(color.r, color.g, color.b);
		}
		break;
	default:
		FractalNow_error("Invalid bytes per component.\n");
		break;
	}
}

char applyGaussianBlurMessage[] = "Applying gaussian blur";

Task *CreateApplyGaussianBlurTask(Image *dst, Image *temp, const Image *src, double radius, 
						uint_fast32_t nbThreads)
{
	Filter horizontalGaussianFilter;
	Filter verticalGaussianFilter;
	CreateHorizontalGaussianFilter2(&horizontalGaussianFilter, radius);
	CreateVerticalGaussianFilter2(&verticalGaussianFilter, radius);

	Task *subTasks[2];
	subTasks[0] = CreateApplyFilterTask(temp, src, &horizontalGaussianFilter, nbThreads);
	subTasks[1] = CreateApplyFilterTask(dst, temp, &verticalGaussianFilter, nbThreads);
	Task *res = CreateCompositeTask(applyGaussianBlurMessage, 2,  subTasks);

	FreeFilter(horizontalGaussianFilter);
	FreeFilter(verticalGaussianFilter);

	return res;
}

void ApplyGaussianBlur(Image *dst, const Image *src, double radius, Threads *threads)
{
	Image temp;
	CreateImage(&temp, src->width, src->height, src->bytesPerComponent);

	Task *task = CreateApplyGaussianBlurTask(dst, &temp, src, radius, threads->N);
	int unused = ExecuteTaskBlocking(task, threads);
	UNUSED(unused);

	FreeImage(temp);
}

typedef struct s_DownscaleImageArguments {
	uint_fast32_t threadId;
	Image *dst;
	uint_fast32_t nbRectangles;
	UIRectangle *rectangles;
	const Image *src;
	double invScaleX;
	double invScaleY;
	Filter *horizontalGaussianFilter;
	Filter *verticalGaussianFilter;
} DownscaleImageArguments;

void FreeDownscaleImageArguments(void *arg)
{
	DownscaleImageArguments *c_arg = (DownscaleImageArguments *)arg;
	if (c_arg->threadId == 0) {
		free(c_arg->rectangles);
		FreeFilter(*c_arg->horizontalGaussianFilter);
		FreeFilter(*c_arg->verticalGaussianFilter);
		free(c_arg->horizontalGaussianFilter);
		free(c_arg->verticalGaussianFilter);
	}
}

void *DownscaleImageThreadRoutine(void *arg)
{
	ThreadArgHeader *threadArgHeader = GetThreadArgHeader(arg);
	DownscaleImageArguments *c_arg = (DownscaleImageArguments *)GetThreadArgBody(arg);
	Image *dst = c_arg->dst;
	const Image *src = c_arg->src;
	double invScaleX = c_arg->invScaleX;
	double invScaleY = c_arg->invScaleY;
	Filter *horizontalGaussianFilter = c_arg->horizontalGaussianFilter;
	Filter *verticalGaussianFilter = c_arg->verticalGaussianFilter;
	Image tmpImage;
	CreateImage(&tmpImage, horizontalGaussianFilter->sx, 1, src->bytesPerComponent);

	uint_fast32_t nbRectangles = c_arg->nbRectangles;
	UIRectangle *dstRect;
	uint_fast32_t rectHeight;
	uint_fast32_t counter = 0;
	int cancelRequested = CancelTaskRequested(threadArgHeader);
	for (uint_fast32_t i = 0; i < nbRectangles && !cancelRequested; ++i) {
		dstRect = &c_arg->rectangles[i];
		rectHeight = dstRect->y2+1 - dstRect->y1;

		for (uint_fast32_t j = dstRect->y1; j <= dstRect->y2 && !cancelRequested; ++j) {
			SetThreadProgress(threadArgHeader, 100 * (i * rectHeight + (j-dstRect->y1)) /
								(rectHeight * nbRectangles));
			uint_fast32_t y = (j+0.5)*invScaleY;
			for (uint_fast32_t k = dstRect->x1; k <= dstRect->x2 && !cancelRequested; ++k) {
				HandleRequests(32);

				uint_fast32_t x = (k+0.5)*invScaleX;

				for (uint_fast32_t l = 0; l < horizontalGaussianFilter->sx; ++l) {
					PutPixelUnsafe(&tmpImage, l, 0, ApplyFilterOnSinglePixel(src,
						x-horizontalGaussianFilter->cx+l, y, verticalGaussianFilter));
				}
				PutPixelUnsafe(dst, k, j, ApplyFilterOnSinglePixel(&tmpImage,
					horizontalGaussianFilter->cx, 0, horizontalGaussianFilter));
			}
		}
	}
	SetThreadProgress(threadArgHeader, 100);

	FreeImage(tmpImage);

	int canceled = CancelTaskRequested(threadArgHeader);

	return (canceled ? PTHREAD_CANCELED : NULL);
}

char downscaleImageMessage[] = "Downscaling image";

Task *CreateDownscaleImageTask(Image *dst, const Image *src, uint_fast32_t nbThreads)
{
	if (src->width == 0 || src->height == 0 || dst->width == 0 || dst->height == 0) {
		return DoNothingTask();
	}
	if (dst->width > src->width || dst->height > src->height) {
		FractalNow_error("Downscaling to a bigger image makes no sense.\n");
	}

	double invScaleX = src->width / (double)dst->width;
	double invScaleY = src->height / (double)dst->height;

	Filter *horizontalGaussianFilter =
		(Filter *)safeMalloc("horizontal gaussian filter", sizeof(Filter));
	Filter *verticalGaussianFilter =
		(Filter *)safeMalloc("vertical gaussian filter", sizeof(Filter));

	CreateHorizontalGaussianFilter2(horizontalGaussianFilter, invScaleX);
	CreateVerticalGaussianFilter2(verticalGaussianFilter, invScaleY);

	uint_fast32_t nbPixels = dst->width*dst->height;
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
	InitUIRectangle(&rectangle[0], 0, 0, dst->width-1, dst->height-1);
	if (CutUIRectangleInN(rectangle[0], nbRectangles, rectangle)) {
		FractalNow_error("Could not cut rectangle ((%"PRIuFAST32",%"PRIuFAST32"),\
(%"PRIuFAST32",%"PRIuFAST32") in %"PRIuFAST32" parts.\n", rectangle[0].x1, rectangle[0].y1,
				rectangle[0].x2, rectangle[0].y2, nbRectangles);
	}

	DownscaleImageArguments *arg;
	arg = (DownscaleImageArguments *)safeMalloc("arguments", nbThreadsNeeded *
							sizeof(DownscaleImageArguments));
	for (uint_fast32_t i = 0; i < nbThreadsNeeded; ++i) {
		arg[i].threadId = i;
		arg[i].dst = dst;
		arg[i].nbRectangles = rectanglesPerThread;
		arg[i].rectangles = &rectangle[i*rectanglesPerThread];
		arg[i].src = src;
		arg[i].invScaleX = invScaleX;
		arg[i].invScaleY = invScaleY;
		/* Filters copied to avoid concurrent read. */
		arg[i].horizontalGaussianFilter = horizontalGaussianFilter;
		arg[i].verticalGaussianFilter = verticalGaussianFilter;
	}
	Task *res = CreateTask(downscaleImageMessage, nbThreadsNeeded, arg,
					sizeof(DownscaleImageArguments), DownscaleImageThreadRoutine,
					FreeDownscaleImageArguments);

	free(arg);

	return res;
}

void DownscaleImage(Image *dst, const Image *src, Threads *threads)
{
	Task *task = CreateDownscaleImageTask(dst, src, threads->N);
	int unused = ExecuteTaskBlocking(task, threads);
	UNUSED(unused);
}

void FreeImage(Image image)
{
	if (image.width != 0 && image.height != 0
		&& !image.data_is_external) {
		free(image.data);
	}
}

