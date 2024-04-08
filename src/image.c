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
#include "rectangle.h"
#include "thread.h"

void CreateUnitializedImage(Image *image, uint32_t width, uint32_t height)
{
	image->data = (Color *)malloc(width*height*sizeof(Color));
	if (image->data == NULL) {
		alloc_error("image");
	}
	image->width = width;
	image->height = height;
}

typedef enum {
	LEFT = 0, TOP, RIGHT, BOTTOM, TOPLEFT, TOPRIGHT, BOTTOMRIGHT, BOTTOMLEFT, CENTER
} Region;

inline Color iGetPixelUnsafe(Image *image, uint32_t x, uint32_t y) {
	return *(image->data+y*image->width+x);
}

Region iGetImageRegion(Image *image, int32_t x, int32_t y)
{
	Region region;

	if (x < 0) {
		if (y < 0) {
			region = TOPLEFT;
		} else if ((uint32_t)y >= image->height) {
			region = BOTTOMLEFT;
		} else {
			region = LEFT;
		}
	} else if ((uint32_t)x >= image->width) {
		if (y < 0) {
			region = TOPRIGHT;
		} else if ((uint32_t)y >= image->height) {
			region = BOTTOMRIGHT;
		} else {
			region = RIGHT;
		}
	} else {
		if (y < 0) {
			region = TOP;
		} else if ((uint32_t)y >= image->height) {
			region = BOTTOM;
		} else {
			region = CENTER;
		}
	}

	return region;
}

inline Color iGetPixel(Image *image, int32_t x, int32_t y) {
	Region region = iGetImageRegion(image, x, y);

	switch (region) {
	case TOPLEFT:
		return iGetPixelUnsafe(image, 0, 0);
		break;
	case TOPRIGHT:
		return iGetPixelUnsafe(image, image->width-1, 0);
		break;
	case BOTTOMLEFT:
		return iGetPixelUnsafe(image, 0, image->height-1);
		break;
	case BOTTOMRIGHT:
		return iGetPixelUnsafe(image, image->width-1, image->height-1);
		break;
	case TOP:
		return iGetPixelUnsafe(image, x, 0);
		break;
	case LEFT:
		return iGetPixelUnsafe(image, 0, y);
		break;
	case RIGHT:
		return iGetPixelUnsafe(image, image->width-1, y);
		break;
	case BOTTOM:
		return iGetPixelUnsafe(image, x, image->height-1);
		break;
	case CENTER:
		return iGetPixelUnsafe(image, x, y);
		break;
	default:
		error("Unknown region.\n");
		break;
	}
}


inline void PutPixel(Image *image, uint32_t x, uint32_t y, Color color)
{
	*(image->data+y*image->width+x)=color;
}

void ApplyGaussianBlur(Image *dst, Image *src, double radius)
{
	info(T_NORMAL,"Applying gaussian blur...\n");

	Filter horizFilter, vertFilter;
	CreateHorizontalGaussianFilter2(&horizFilter, radius);
	CreateVerticalGaussianFilter2(&vertFilter, radius);

	Image temp;
	CreateUnitializedImage(&temp, src->width, src->height);

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
	double invScaleX;
	double invScaleY;
	Filter *horizontalGaussianFilter;
	Filter *verticalGaussianFilter;
} DownscaleImageArguments;

void *DownscaleImageThreadRoutine(void *arg)
{
	DownscaleImageArguments *c_arg = (DownscaleImageArguments *)arg;
	Image *dst = c_arg->dst;
	Rectangle *dstRect = c_arg->dstRect;
	Image *src = c_arg->src;
	double invScaleX = c_arg->invScaleX;
	double invScaleY = c_arg->invScaleY;
	Filter *horizontalGaussianFilter = c_arg->horizontalGaussianFilter;
	Filter *verticalGaussianFilter = c_arg->verticalGaussianFilter;
	Image tmpImage;
	CreateUnitializedImage(&tmpImage, 1, verticalGaussianFilter->sy);

	info(T_VERBOSE,"Downscaling image from (%u,%u) to (%u,%u)...\n",dstRect->x1,dstRect->y1,
			dstRect->x2,dstRect->y2);

	for (uint32_t i = dstRect->x1; i <= dstRect->x2; ++i) {
		uint32_t x = (i+0.5)*invScaleX;
		for (uint32_t j = dstRect->y1; j <= dstRect->y2; ++j) {
			uint32_t y = (j+0.5)*invScaleY;

			for (uint32_t k = 0; k < verticalGaussianFilter->sy; ++k) {
				PutPixel(&tmpImage, 0, k, ApplyFilterOnSinglePixel(
					src, x, y-verticalGaussianFilter->cy+k, horizontalGaussianFilter));
			}
			PutPixel(dst, i, j, ApplyFilterOnSinglePixel(&tmpImage,
				0, verticalGaussianFilter->cy, verticalGaussianFilter));
		}
	}

	FreeImage(&tmpImage);

	info(T_VERBOSE,"Downscaling image from (%u,%u) to (%u,%u) : DONE.\n",dstRect->x1,dstRect->y1,
			dstRect->x2,dstRect->y2);

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

	double invScaleX = src->width / (double)dst->width;
	double invScaleY = src->height / (double)dst->height;

	Filter horizontalGaussianFilter;
	Filter verticalGaussianFilter;

	CreateHorizontalGaussianFilter2(&horizontalGaussianFilter, invScaleX);
	CreateVerticalGaussianFilter2(&verticalGaussianFilter, invScaleY);

	uint32_t nbPixels = src->width*src->height;
	uint32_t realNbThreads = (nbThreads > nbPixels) ? nbPixels : nbThreads;

	Rectangle *rectangle;
	rectangle = (Rectangle *)malloc(realNbThreads * sizeof(Rectangle));
	if (rectangle == NULL) {
		alloc_error("rectangles");
	}
	InitRectangle(&rectangle[0], 0, 0, dst->width-1, dst->height-1);
	CutRectangleInN(rectangle[0], realNbThreads, rectangle);

        pthread_t *thread;
	thread = (pthread_t *)malloc(realNbThreads * sizeof(pthread_t)); 
	if (thread == NULL) {
		alloc_error("threads");
	}
	DownscaleImageArguments *arg;
	arg = (DownscaleImageArguments *)malloc(realNbThreads * sizeof(DownscaleImageArguments));
	if (arg == NULL) {
		alloc_error("arguments");
	}
	for (uint32_t i = 0; i < realNbThreads; ++i) {
		arg[i].dst = dst;
		arg[i].dstRect = &rectangle[i];
		arg[i].src = src;
		arg[i].invScaleX = invScaleX;
		arg[i].invScaleY = invScaleY;
		arg[i].horizontalGaussianFilter = &horizontalGaussianFilter;
		arg[i].verticalGaussianFilter = &verticalGaussianFilter;

                safePThreadCreate(&(thread[i]),NULL,DownscaleImageThreadRoutine,(void *)&arg[i]);
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
	FreeFilter(&horizontalGaussianFilter);
	FreeFilter(&verticalGaussianFilter);

	info(T_NORMAL,"Downscaling image : DONE.\n");
}

void FreeImage(Image *image)
{
	free(image->data);
}
