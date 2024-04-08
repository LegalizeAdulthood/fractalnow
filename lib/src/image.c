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
#include <inttypes.h>
#include <signal.h>
#include <stdlib.h>
#include <string.h>

inline void CreateImage(Image *image, uint_fast32_t width, uint_fast32_t height, uint_fast8_t bytesPerComponent)
{
	if (bytesPerComponent != 1 && bytesPerComponent != 2) {
		error("Invalid bytes per component. Only 1 and 2 are allowed.\n");
	}
	if (width == 0 || height == 0) {
		image->data = NULL;
	} else {
		image->data = (uint8_t *)safeCalloc("image data", width*height, 4*bytesPerComponent);
	}
	image->data_is_external = 0;
	image->width = width;
	image->height = height;
	image->bytesPerComponent = bytesPerComponent;
}

void CreateImage2(Image *image, uint8_t *data, uint_fast32_t width, uint_fast32_t height,
			uint_fast8_t bytesPerComponent)
{
	if (bytesPerComponent != 1 && bytesPerComponent != 2) {
		error("Invalid bytes per component. Only 1 and 2 are allowed.\n");
	}
	image->data = data;
	image->data_is_external = 1;
	image->width = width;
	image->height = height;
	image->bytesPerComponent = bytesPerComponent;
}

Image *CopyImage(Image *image)
{
	Image *res = (Image *)safeMalloc("image copy", sizeof(Image));
	CreateImage(res, image->width, image->height, image->bytesPerComponent);
	memcpy(res->data, image->data, image->width*image->height*4*image->bytesPerComponent);

	return res;
}

static inline uint8_t *ImageRGB8ToBytesArray(Image *image)
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

static inline uint8_t *ImageRGB16ToBytesArray(Image *image)
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

uint8_t *ImageToBytesArray(Image *image)
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
		error("Invalid bytes per component (%"PRIuFAST8").\n",
			image->bytesPerComponent);
		break;
	}

	return res;
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

Color blackColorRGB8 = {1, 0, 0, 0};
Color blackColorRGB16 = {2, 0, 0, 0};

Color iGetPixel(Image *image, int_fast64_t x, int_fast64_t y) {
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
		error("Invalid bytes per component.\n");
		break;
	}
}

Action *LaunchApplyGaussianBlur(Image *dst, Image *src, FLOAT radius)
{
	Filter gaussianFilter;
	CreateGaussianFilter(&gaussianFilter, radius);

	Action *res = LaunchApplyFilter(dst, src, &gaussianFilter);

	FreeFilter(gaussianFilter);

	return res;
}

void ApplyGaussianBlur(Image *dst, Image *src, FLOAT radius)
{
	Filter horizFilter, vertFilter;
	CreateHorizontalGaussianFilter2(&horizFilter, radius);
	CreateVerticalGaussianFilter2(&vertFilter, radius);

	Image temp;
	CreateImage(&temp, src->width, src->height, src->bytesPerComponent);

	ApplyFilter(&temp, src, &horizFilter);
	ApplyFilter(dst, &temp, &vertFilter);

	FreeFilter(horizFilter);
	FreeFilter(vertFilter);
	FreeImage(temp);
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

void FreeDownscaleImageArguments(void *arg)
{
	DownscaleImageArguments *c_arg = (DownscaleImageArguments *)arg;
	free(c_arg->dstRect);
	FreeFilter(*c_arg->horizontalGaussianFilter);
	FreeFilter(*c_arg->verticalGaussianFilter);
	free(c_arg->horizontalGaussianFilter);
	free(c_arg->verticalGaussianFilter);
}

void *DownscaleImageThreadRoutine(void *arg)
{
	volatile sig_atomic_t *cancel = *((volatile sig_atomic_t **)arg);
	uint8_t *arg1 = (uint8_t *)arg;
	DownscaleImageArguments *c_arg = (DownscaleImageArguments *)(arg1+sizeof(volatile sig_atomic_t *));
	Image *dst = c_arg->dst;
	Rectangle *dstRect = c_arg->dstRect;
	Image *src = c_arg->src;
	FLOAT invScaleX = c_arg->invScaleX;
	FLOAT invScaleY = c_arg->invScaleY;
	Filter *horizontalGaussianFilter = c_arg->horizontalGaussianFilter;
	Filter *verticalGaussianFilter = c_arg->verticalGaussianFilter;
	Image tmpImage;
	CreateImage(&tmpImage, 1, verticalGaussianFilter->sy, src->bytesPerComponent);

	info(T_VERBOSE,"Downscaling image from (%"PRIuFAST32",%"PRIuFAST32") to \
(%"PRIuFAST32",%"PRIuFAST32")...\n", dstRect->x1,dstRect->y1, dstRect->x2,dstRect->y2);

	for (uint_fast32_t i = dstRect->x1; i <= dstRect->x2 && !(*cancel); ++i) {
		uint_fast32_t x = (i+0.5)*invScaleX;
		for (uint_fast32_t j = dstRect->y1; j <= dstRect->y2 && !(*cancel); ++j) {
			uint_fast32_t y = (j+0.5)*invScaleY;

			for (uint_fast32_t k = 0; k < verticalGaussianFilter->sy; ++k) {
				PutPixelUnsafe(&tmpImage, 0, k, ApplyFilterOnSinglePixel(
					src, x, y-verticalGaussianFilter->cy+k, horizontalGaussianFilter));
			}
			PutPixelUnsafe(dst, i, j, ApplyFilterOnSinglePixel(&tmpImage,
				0, verticalGaussianFilter->cy, verticalGaussianFilter));
		}
	}

	FreeImage(tmpImage);

	info(T_VERBOSE,"Downscaling image from (%"PRIuFAST32",%"PRIuFAST32") to \
(%"PRIuFAST32",%"PRIuFAST32") : %s.\n", dstRect->x1,dstRect->y1, dstRect->x2,
		dstRect->y2, (*cancel) ? "CANCELED" : "DONE");

	return ((*cancel) ? PTHREAD_CANCELED : NULL);
}

inline Action *LaunchDownscaleImage(Image *dst, Image *src)
{
	if (src->width == 0 || src->height == 0 || dst->width == 0 || dst->height == 0) {
		return DoNothingAction();
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
		arg[i].dstRect = CopyRectangle(&rectangle[i]);
		arg[i].src = src;
		arg[i].invScaleX = invScaleX;
		arg[i].invScaleY = invScaleY;
		arg[i].horizontalGaussianFilter = CopyFilter(&horizontalGaussianFilter);
		arg[i].verticalGaussianFilter = CopyFilter(&verticalGaussianFilter);
	}
	Action *res = LaunchThreads("Downscaling image", realNbThreads, arg, sizeof(DownscaleImageArguments),
					DownscaleImageThreadRoutine, FreeDownscaleImageArguments);

	free(rectangle);
	free(arg);
	FreeFilter(horizontalGaussianFilter);
	FreeFilter(verticalGaussianFilter);

	return res;
}

void DownscaleImage(Image *dst, Image *src)
{
	Action *action = LaunchDownscaleImage(dst, src);
	int unused = WaitForActionTermination(action);
	(void)unused;
	FreeAction(*action);
	free(action);
}

void FreeImage(Image image)
{
	if (image.width != 0 && image.height != 0
		&& !image.data_is_external) {
		free(image.data);
	}
}
