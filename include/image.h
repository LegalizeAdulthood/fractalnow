/*
 *  image.h -- part of fractal2D
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
 
#ifndef __IMAGE_H__
#define __IMAGE_H__

#include "error.h"
#include "color.h"
#include <stdint.h>
#include <stdlib.h>

typedef struct s_Image {
	Color *data;
	uint32_t width, height;
} Image;

void CreateUnitializedImage(Image *image, uint32_t width, uint32_t height);

// Get pixel at point (x,y) (integers) of image
// Assumes (x,y) is indeed a point in image
Color iGetPixelUnsafe(Image *image, uint32_t x, uint32_t y);
Color iGetPixel(Image *image, int32_t x, int32_t y);

// Put a pixel at point (x,y) of image
// Assumes (x,y) is indeed a point in image
void PutPixel(Image *image, uint32_t x, uint32_t y, Color color);

void ApplyGaussianBlur(Image *dst, Image *src, double radius);
void DownscaleImage(Image *dst, Image *src);

void FreeImage(Image *image);

#endif
