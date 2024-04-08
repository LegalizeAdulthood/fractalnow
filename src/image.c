/*
 *  image.c -- part of fractal2D
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
#include "rectangle.h"
#include "thread.h"
#include <inttypes.h>
#include <stdlib.h>
#include <string.h>

void CreateUnitializedImage(Image *image, uint_fast32_t width, uint_fast32_t height, uint_fast8_t bytesPerComponent)
{
	if (bytesPerComponent != 1 && bytesPerComponent != 2) {
		error("Invalid bytes per component. Only 1 and 2 are allowed.\n");
	}
	image->data = (uint8_t *)safeMalloc("image", width*height*3*bytesPerComponent);
	image->width = width;
	image->height = height;
	image->bytesPerComponent = bytesPerComponent;
}

static inline void ImageRGB8ToBytesArray(uint8_t *array, Image *image)
{
	memcpy(array, image->data, image->width*image->height*3*image->bytesPerComponent);
}

static inline void ImageRGB16ToBytesArray(uint8_t *array, Image *image)
{
	//memcpy(array, image->data, image->width*image->height*3*image->bytesPerComponent);
	uint8_t *p_array = array;
	uint16_t *data = (uint16_t *)image->data;

	for (uint_fast32_t i = 0; i < image->height; ++i) {
		for (uint_fast32_t j = 0; j < image->width; ++j) {
			*(p_array++) = (uint8_t)((*data) >> 8);
			*(p_array++) = (uint8_t)((*data) & 0xFF);
			++data;
			*(p_array++) = (uint8_t)((*data) >> 8);
			*(p_array++) = (uint8_t)((*data) & 0xFF);
			++data;
			*(p_array++) = (uint8_t)((*data) >> 8);
			*(p_array++) = (uint8_t)((*data) & 0xFF);
			++data;
		}
	}
}

void ImageToBytesArray(uint8_t *array, Image *image)
{
	switch(image->bytesPerComponent) {
	case 1:
		ImageRGB8ToBytesArray(array, image);
		break;
	case 2:
		ImageRGB16ToBytesArray(array, image);
		break;
	default:
		error("Invalid bytes per component (%"PRIuFAST8").\n",
			image->bytesPerComponent);
		break;
	}
}

typedef enum {
	LEFT = 0, TOP, RIGHT, BOTTOM, TOPLEFT, TOPRIGHT, BOTTOMRIGHT, BOTTOMLEFT, CENTER
} Region;

inline Color iGetPixelUnsafe(Image *image, uint_fast32_t x, uint_fast32_t y) {
	Color res;
	res.bytesPerComponent = image->bytesPerComponent;
	switch (res.bytesPerComponent) {
	case 1:
		{
		uint8_t *data = image->data + (y*image->width+x)*3;
		res.r = *(data++);
		res.g = *(data++);
		res.b = *(data++);
		break;
		}
	case 2:
		{
		uint16_t *data = (uint16_t *)(image->data + (y*image->width+x)*6);
		res.r = *(data++);
		res.g = *(data++);
		res.b = *(data++);
		}
		break;
	default:
		error("Invalid bytes per component.\n");
		break;
	}
	
	return res;
}

Region iGetImageRegion(Image *image, int_fast64_t x, int_fast64_t y)
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

inline Color iGetPixel(Image *image, int_fast64_t x, int_fast64_t y) {
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
		error("Unknown region.\n");
		break;
	}
	return res;
}


inline void PutPixelUnsafe(Image *image, uint_fast32_t x, uint_fast32_t y, Color color)
{
	switch (image->bytesPerComponent) {
	case 1:
		{
		uint8_t *data = image->data + (y*image->width+x)*3;
		*(data++) = color.r;
		*(data++) = color.g;
		*(data++) = color.b;
		break;
		}
	case 2:
		{
		uint16_t *data = (uint16_t *)(image->data + (y*image->width+x)*6);
		*(data++) = color.r;
		*(data++) = color.g;
		*(data++) = color.b;
		}
		break;
	default:
		error("Invalid bytes per component.\n");
		break;
	}
}

void ApplyGaussianBlur(Image *dst, Image *src, FLOAT radius)
{
	info(T_NORMAL,"Applying gaussian blur...\n");

	Filter horizFilter, vertFilter;
	CreateHorizontalGaussianFilter2(&horizFilter, radius);
	CreateVerticalGaussianFilter2(&vertFilter, radius);

	Image temp;
	CreateUnitializedImage(&temp, src->width, src->height, src->bytesPerComponent);

	ApplyFilter(&temp, src, &horizFilter);
	ApplyFilter(dst, &temp, &vertFilter);

	FreeFilter(&horizFilter);
	FreeFilter(&vertFilter);
	FreeImage(&temp);

	info(T_NORMAL,"Applying gaussian blur : DONE.\n");
}

typedef struct s_DownscaleImageArguments {
	Image *dst;
	Rectangle *dstRect;
	Image *src;
	FLOAT invScaleX;
	FLOAT invScaleY;
	Filter *horizontalGaussianFilter;
	Filter *verticalGaussianFilter;
} DownscaleImageArguments;

void *DownscaleImageThreadRoutine(void *arg)
{
	DownscaleImageArguments *c_arg = (DownscaleImageArguments *)arg;
	Image *dst = c_arg->dst;
	Rectangle *dstRect = c_arg->dstRect;
	Image *src = c_arg->src;
	FLOAT invScaleX = c_arg->invScaleX;
	FLOAT invScaleY = c_arg->invScaleY;
	Filter *horizontalGaussianFilter = c_arg->horizontalGaussianFilter;
	Filter *verticalGaussianFilter = c_arg->verticalGaussianFilter;
	Image tmpImage;
	CreateUnitializedImage(&tmpImage, 1, verticalGaussianFilter->sy, src->bytesPerComponent);

	info(T_VERBOSE,"Downscaling image from (%"PRIuFAST32",%"PRIuFAST32") to \
(%"PRIuFAST32",%"PRIuFAST32")...\n", dstRect->x1,dstRect->y1, dstRect->x2,dstRect->y2);

	for (uint_fast32_t i = dstRect->x1; i <= dstRect->x2; ++i) {
		uint_fast32_t x = (i+0.5)*invScaleX;
		for (uint_fast32_t j = dstRect->y1; j <= dstRect->y2; ++j) {
			uint_fast32_t y = (j+0.5)*invScaleY;

			for (uint_fast32_t k = 0; k < verticalGaussianFilter->sy; ++k) {
				PutPixelUnsafe(&tmpImage, 0, k, ApplyFilterOnSinglePixel(
					src, x, y-verticalGaussianFilter->cy+k, horizontalGaussianFilter));
			}
			PutPixelUnsafe(dst, i, j, ApplyFilterOnSinglePixel(&tmpImage,
				0, verticalGaussianFilter->cy, verticalGaussianFilter));
		}
	}

	FreeImage(&tmpImage);

	info(T_VERBOSE,"Downscaling image from (%"PRIuFAST32",%"PRIuFAST32") to \
(%"PRIuFAST32",%"PRIuFAST32") : DONE.\n", dstRect->x1,dstRect->y1, dstRect->x2,dstRect->y2);

	return NULL;
}

void DownscaleImage(Image *dst, Image *src)
{
	info(T_NORMAL,"Downscaling image...\n");

	if (dst->width == 0 || dst->height == 0) {
		error("Dst image for downscaling is empty.\n");
	}
	if (src->width == 0 || src->height == 0) {
		error("Src image for downscaling is empty.\n");
	}
	if (dst->width > src->width || dst->height > src->height) {
		error("Downscaling to a bigger image makes no sense.\n");
	}

	FLOAT invScaleX = src->width / (FLOAT)dst->width;
	FLOAT invScaleY = src->height / (FLOAT)dst->height;

	Filter horizontalGaussianFilter;
	Filter verticalGaussianFilter;

	CreateHorizontalGaussianFilter2(&horizontalGaussianFilter, invScaleX);
	CreateVerticalGaussianFilter2(&verticalGaussianFilter, invScaleY);

	uint_fast32_t nbPixels = src->width*src->height;
	uint_fast32_t realNbThreads = (nbThreads > nbPixels) ? nbPixels : nbThreads;

	Rectangle *rectangle;
	rectangle = (Rectangle *)safeMalloc("rectangles", realNbThreads * sizeof(Rectangle));
	InitRectangle(&rectangle[0], 0, 0, dst->width-1, dst->height-1);
	if (CutRectangleInN(rectangle[0], realNbThreads, rectangle)) {
		error("Could not cut rectangle ((%"PRIuFAST32",%"PRIuFAST32"),(%"PRIuFAST32",%"PRIuFAST32") \
in %"PRIuFAST32" parts.\n", rectangle[0].x1, rectangle[0].y1, rectangle[0].x2, rectangle[0].y2,
			realNbThreads);
	}

	DownscaleImageArguments *arg;
	arg = (DownscaleImageArguments *)safeMalloc("arguments", realNbThreads * sizeof(DownscaleImageArguments));
	for (uint_fast32_t i = 0; i < realNbThreads; ++i) {
		arg[i].dst = dst;
		arg[i].dstRect = &rectangle[i];
		arg[i].src = src;
		arg[i].invScaleX = invScaleX;
		arg[i].invScaleY = invScaleY;
		arg[i].horizontalGaussianFilter = &horizontalGaussianFilter;
		arg[i].verticalGaussianFilter = &verticalGaussianFilter;
	}
	LaunchThreadsAndWait(realNbThreads, arg, sizeof(DownscaleImageArguments), DownscaleImageThreadRoutine);

	free(rectangle);
	free(arg);
	FreeFilter(&horizontalGaussianFilter);
	FreeFilter(&verticalGaussianFilter);

	info(T_NORMAL,"Downscaling image : DONE.\n");
}

void FreeImage(Image *image)
{
	free(image->data);
}
