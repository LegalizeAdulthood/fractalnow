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
 
 /**
  * \file image.h
  * \brief Header file related to images.
  * \author Marc Pegon
  */

#ifndef __IMAGE_H__
#define __IMAGE_H__

#include "error.h"
#include "color.h"
#include "floating_point.h"
#include <stdint.h>

/**
 * \struct s_Image
 * \brief A simple image.
 *
 * A 2D array of colors representing an image.
 */
typedef struct s_Image {
	Color *data;
 /*!< Imaga data.*/
	uint32_t width;
 /*!< Image width.*/
	uint32_t height;
 /*!< Image height.*/
	int bytesPerComponent;
 /*!< Colors bytes per component.*/
} Image;

/**
 * \fn void CreateUnitializedImage(Image *image, uint32_t width, uint32_t height, int bytesPerComponent)
 * \brief Create an unitialized image.
 *
 * Create an unitialized image (allocate memory for data) of given width and height.
 *
 * \param image Pointer to image structure to create.
 * \param width Image width.
 * \param height Image height.
 * \param bytesPerComponent Colors bytes per component.
 */
void CreateUnitializedImage(Image *image, uint32_t width, uint32_t height,
				int bytesPerComponent);

/**
 * \fn Color iGetPixelUnsafe(Image *image, uint32_t x, uint32_t y)
 * \brief Get some pixel of image.
 *
 * Get pixel (x,y) of image.
 * Warning : this function is NOT safe, it doesn't check that (x,y)
 * is actually a point inside image.
 *
 * \param image Image to get pixel from.
 * \param x X pixel coordinate.
 * \param y Y pixel coordinate.
 * \return Color at position (x,y) of image.
 */
Color iGetPixelUnsafe(Image *image, uint32_t x, uint32_t y);

/**
 * \fn Color iGetPixel(Image *image, int32_t x, int32_t y)
 * \brief Get some pixel of image.
 *
 * Get pixel (x,y) of image.
 * This function IS safe (pixels outside of the image will be
 * duplicates of the pixels at the sides of it).
 *
 * \param image Image to get pixel from.
 * \param x X pixel coordinate.
 * \param y Y pixel coordinate.
 * \return Color at position (x,y) of image.
 */
Color iGetPixel(Image *image, int32_t x, int32_t y);

/**
 * \fn void PutPixelUnsafe(Image *image, uint32_t x, uint32_t y, Color color)
 * \brief Put pixel at some particular position in image.
 *
 * Put pixel at position (x,y) of image.
 * Warning : this function is NOT safe, it doesn't check that (x,y)
 * is actually a point inside image.
 * Also make sure that color format is the same as image format :
 * it is NOT checked by the function.
 *
 * \param image Image to put pixel to.
 * \param x X pixel coordinate.
 * \param y Y pixel coordinate.
 * \param color Color to put at position (x,y) of image.
 */
void PutPixelUnsafe(Image *image, uint32_t x, uint32_t y, Color color);

/**
 * \fn void ApplyGaussianBlur(Image *dst, Image *src, FLOAT radius)
 * \brief Apply gaussian blur on image.
 *
 * This function does no work in place.
 *
 * \param dst Pointer to already created destination image.
 * \param src Pointer to source image (to apply blur on).
 * \param radius Gaussian blur radius.
 */
void ApplyGaussianBlur(Image *dst, Image *src, FLOAT radius);

/**
 * \fn void DownscaleImage(Image *dst, Image *src)
 * \brief Downscale image.
 *
 * This function does no work in place.
 * Destination image must already have been created, and its size
 * (both with and height) must be greater than that of the source.
 *
 * \param dst Pointer to already created destination image.
 * \param src Pointer to source image (to be downscaled).
 */
void DownscaleImage(Image *dst, Image *src);

/**
 * \fn void FreeImage(Image *image)
 * \brief Free image.
 *
 * \param image Pointer to image to free.
 */
void FreeImage(Image *image);

#endif
