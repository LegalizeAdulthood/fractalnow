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

#ifdef __cplusplus
extern "C" {
#endif

#include "error.h"
#include "color.h"
#include "floating_point.h"
#include "thread.h"
#include <stdint.h>

/**
 * \struct s_Image
 * \brief A simple image.
 *
 * RGB8 images pixels are stored using uint32_t (the
 * most significant 8 bits are not used (0xFF)).
 * RGB16 images pixels are stored using uint64_t bytes (the
 * most significant 16 bites are not used (0xFFFF)).
 *
 * A 2D array of colors representing an image.
 */
typedef struct s_Image {
	uint8_t *data;
 /*!< Imaga data : 32-bits aligned.*/
	int data_is_external;
 /*!< 1 if data should not be freed by Image structure.*/
	uint_fast32_t width;
 /*!< Image width.*/
	uint_fast32_t height;
 /*!< Image height.*/
	uint_fast8_t bytesPerComponent;
 /*!< Colors bytes per component.*/
} Image;

/**
 * \fn void CreateImage(Image *image, uint_fast32_t width, uint_fast32_t height, uint_fast8_t bytesPerComponent)
 * \brief Create an image.
 *
 * Create a (black) image of given width and height.
 *
 * \param image Pointer to image structure to create.
 * \param width Image width.
 * \param height Image height.
 * \param bytesPerComponent Colors bytes per component.
 */
void CreateImage(Image *image, uint_fast32_t width, uint_fast32_t height,
			uint_fast8_t bytesPerComponent);

/**
 * \fn void CreateImage2(Image *image, uint8_t *data, uint_fast32_t width, uint_fast32_t height, uint_fast8_t bytesPerComponent)
 * \brief Create an image using an already allocated data array.
 *
 * Data format must be as described in image structure.
 * Remark : in this case data will be NOT be freed by FreeImage,
 * and must stay alive as long as image is used.
 *
 * \param image Pointer to image structure to create.
 * \param data Image data to use for image.
 * \param width Image width.
 * \param height Image height.
 * \param bytesPerComponent Colors bytes per component.
 */
void CreateImage2(Image *image, uint8_t *data, uint_fast32_t width, uint_fast32_t height,
			uint_fast8_t bytesPerComponent);

/**
 * \fn Image *CopyImage(Image *image)
 * \brief Copy image.
 *
 * \param image Pointer to image to be copied.
 * \return Newly allocated copy of image.
 */
Image *CopyImage(Image *image);

/**
 * \fn uint8_t *ImageToBytesArray(Image *image)
 * \brief Convert image to bytes array.
 *
 * For RGB8 images : the returned bytes array is the sequence r1 g1 b1 r2 g2 b2, etc.,
 * without padding (not 32-bits aligned).
 * For RGB16 images : the returned bytes array is the sequence r1h r1l g1h g1l
 * b1h b1l r2h r2b g2h g2l b2h b2l (h for high (MSB), and l for low (LSB)).
 * without padding (not 32-bits aligned). 
 *
 * Thus the array size (in bytes) is width*height*3*bytesPerComponent.
 * Convenient to write directly a binary PPM file for example.
 * 
 * \param image Image to convert to bytes array.
 * \return Copy of image data.
 */
uint8_t *ImageToBytesArray(Image *image);

/**
 * \fn Color iGetPixelUnsafe(Image *image, uint_fast32_t x, uint_fast32_t y)
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
Color iGetPixelUnsafe(Image *image, uint_fast32_t x, uint_fast32_t y);

/**
 * \fn Color iGetPixel(Image *image, int_fast64_t x, int_fast64_t y)
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
Color iGetPixel(Image *image, int_fast64_t x, int_fast64_t y);

/**
 * \fn void PutPixelUnsafe(Image *image, uint_fast32_t x, uint_fast32_t y, Color color)
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
void PutPixelUnsafe(Image *image, uint_fast32_t x, uint_fast32_t y, Color color);

/**
 * \fn void ApplyGaussianBlur(Image *dst, Image *src, FLOAT radius)
 * \brief Apply gaussian blur on image.
 *
 * This function does no work in place.
 * Applied successively a horizontal and vertical gaussian filter
 * to avoid quadratic computation cost.
 *
 * \param dst Pointer to already created destination image.
 * \param src Pointer to source image (to apply blur on).
 * \param radius Gaussian blur radius.
 */
void ApplyGaussianBlur(Image *dst, Image *src, FLOAT radius);

/**
 * \fn Action *LaunchApplyGaussianBlur(Image *dst, Image *src, FLOAT radius)
 * \brief Launch apply gaussian blur action, but does not wait for termination.
 *
 * Action returned can be used to wait for termination or cancel gaussian blur applying.
 * A square gaussian filter is applied, so the computational cost is quadratic,
 * unlike ApplyGaussianBlur function.
 *
 * \param dst Pointer to already created destination image.
 * \param src Pointer to source image (to apply blur on).
 * \param radius Gaussian blur radius.
 * \return Corresponding newly-allocated action.
 */
Action *LaunchApplyGaussianBlur(Image *dst, Image *src, FLOAT radius);

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
 * \fn Action *LaunchDownscaleImage(Image *dst, Image *src)
 * \brief Launch image downscaling, but does not wait for termination.
 *
 * Action returned can be used to wait for termination or cancel image downscaling.
 *
 * \param dst Pointer to already created destination image.
 * \param src Pointer to source image (to be downscaled).
 * \return Corresponding (newly-allocated) action.
 */
Action *LaunchDownscaleImage(Image *dst, Image *src);

/**
 * \fn void FreeImage(Image image)
 * \brief Free image.
 *
 * \param image Image to free.
 */
void FreeImage(Image image);

#ifdef __cplusplus
}
#endif

#endif
