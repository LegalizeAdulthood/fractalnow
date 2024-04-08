/*
 *  filter.h -- part of fractal2D
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
  * \file filter.h
  * \brief Header file related to filters.
  * \author Marc Pegon
  *
  * Convolution filters are used to apply transformations on image, such
  * as gaussian blur.
  */

#ifndef __FILTER_H__
#define __FILTER_H__

#ifdef __cplusplus
extern "C" {
#endif

#include "floating_point.h"
#include "image.h"
#include "thread.h"
#include <stdint.h>

/**
 * \struct s_Filter
 * \brief Structure to represent a filter.
 *
 * A filter is typically a matrix (kernel) with a center of application.
 */
typedef struct s_Filter
{
	uint_fast32_t sx; /*!< Number of columns.*/
	uint_fast32_t sy; /*!< Number of rows.*/
	uint_fast32_t cx; /*!< X coordinate of filter's center.*/
	uint_fast32_t cy; /*!< Y coordinate of filter's center.*/
	FLOAT *data; /*!< Filter data (the elements of the matrix).*/
} Filter;

/**
 * \fn void InitFilter(Filter *filter, uint_fast32_t sx, uint_fast32_t sy, uint_fast32_t cx, uint_fast32_t cy, FLOAT *data)
 * \brief Initialize filter.
 *
 * Data will be owned by filter, and freed when the filter is freed,
 * so it must have been dynamically allocated.
 *
 * \param filter Pointer to the filter structure to initialize.
 * \param sx Number of columns.
 * \param sy Number of lines.
 * \param cx X coordinate of filter's center.
 * \param cy Y coordinate of filter's center.
 * \param data Filter data (NOT copied, and will be freed when the filter is freed).
 */
void InitFilter(Filter *filter, uint_fast32_t sx, uint_fast32_t sy, uint_fast32_t cx, uint_fast32_t cy,
		FLOAT *data);

/**
 * \fn void InitFilter2(Filter *filter, uint_fast32_t sx, uint_fast32_t sy, FLOAT *data)
 * \brief Initialize filter without specifying center.
 *
 * The center is automatically computed : ((sx-1)/2, (sy-1)/2).
 * Data will be owned by filter, and freed when the filter is freed,
 * so it must have been dynamically allocated.
 *
 * \param filter Pointer to the filter structure to initialize.
 * \param sx Number of columns.
 * \param sy Number of lines.
 * \param data Filter data (NOT copied, and will be freed when the filter is freed).
 */
void InitFilter2(Filter *filter, uint_fast32_t sx, uint_fast32_t sy, FLOAT *data);

/**
 * \fn Filter CopyFilter(const Filter *filter)
 * \brief Copy filter.
 *
 * \param filter Pointer to filter to copy.
 * \return Copy of filter.
 */
Filter CopyFilter(const Filter *filter);

/**
 * \fn void CreateHorizontalGaussianFilter(Filter *filter, FLOAT sigma)
 * \brief Create a horizontal gaussian filter given sigma.
 *
 * \param filter Pointer to the filter structure to initialize.
 * \param sigma See Wikipedia (or others) for that.
 */
void CreateHorizontalGaussianFilter(Filter *filter, FLOAT sigma);

/**
 * \fn void CreateHorizontalGaussianFilter2(Filter *filter, FLOAT radius)
 * \brief Create a horizontal gaussian filter given its radius.
 *
 * \param filter Pointer to the filter structure to initialize.
 * \param radius Blur radius.
 */
void CreateHorizontalGaussianFilter2(Filter *filter, FLOAT radius);

/**
 * \fn void CreateVerticalGaussianFilter(Filter *filter, FLOAT sigma)
 * \brief Create a vertical gaussian filter given sigma.
 *
 * \param filter Pointer to the filter structure to initialize.
 * \param sigma See Wikipedia (or others) for that.
 */
void CreateVerticalGaussianFilter(Filter *filter, FLOAT sigma);

/**
 * \fn void CreateVerticalGaussianFilter2(Filter *filter, FLOAT radius)
 * \brief Create a vertical gaussian filter given its radius.
 *
 * \param filter Pointer to the filter structure to initialize.
 * \param radius Blur radius.
 */
void CreateVerticalGaussianFilter2(Filter *filter, FLOAT radius);

/**
 * \fn void CreateGaussianFilter(Filter *filter, FLOAT sigma)
 * \brief Create a square gaussian filter given sigma.
 *
 * Note that it is actually more efficient (computationally speaking)
 * to apply a horizontal and then vertical gaussian filter (or vice
 * versa), with the same sigma, than to apply a square gaussian
 * filter.
 *
 * \param filter Pointer to the filter structure to initialize.
 * \param sigma See Wikipedia (or others) for that.
 */
void CreateGaussianFilter(Filter *filter, FLOAT sigma);

/**
 * \fn void CreateGaussianFilter2(Filter *filter, FLOAT radius)
 * \brief Create a square gaussian filter given its radius.
 *
 * Note that it is actually more efficient (computationally speaking)
 * to apply a horizontal and then vertical gaussian filter (or vice
 * versa), with the same radius, than to apply a square gaussian
 * filter.
 *
 * \param filter Pointer to the filter structure to initialize.
 * \param radius Blur Radius
 */
void CreateGaussianFilter2(Filter *filter, FLOAT radius);

/**
 * \fn FLOAT GetFilterValueUnsafe(const Filter *filter, uint_fast32_t x, uint_fast32_t y)
 * \brief Get some particular value of filter.
 *
 * Get value of filter at row x, column y. Warning, this function is
 * function is unsafe, meaning it does not check whether (x,y) is
 * within range or not.
 *
 * \param filter Filter we want to get some particular value.
 * \param x Row of the filter value.
 * \param y Column of the filter value.
 * \return Value at position (x,y) of filter.
 */
FLOAT GetFilterValueUnsafe(const Filter *filter, uint_fast32_t x, uint_fast32_t y);

/**
 * \fn void MultiplyFilterByScalar(Filter *filter, FLOAT scalar)
 * \brief Multiply whole filter by scalar.
 *
 * Multiply each filter's value by a scalar.
 *
 * \param filter Filter to be modified.
 * \param scalar Scalar to multiply the filter by.
 */
void MultiplyFilterByScalar(Filter *filter, FLOAT scalar);

/**
 * \fn int NormalizeFilter(Filter *filter)
 * \brief Normalize filter.
 *
 * Warning : some filters cannot be normalized (if the sum of
 * its values is equal to 0).
 * If the filter cannot be normalized, it will be left unchanged.
 *
 * \param filter Filter to normalize.
 * \return 1 if it cannot be normalized, 0 otherwize.
 */
int NormalizeFilter(Filter *filter);

/**
 * \fn Color ApplyFilterOnSinglePixel(const Image *src, uint_fast32_t x, uint_fast32_t y, const Filter *filter)
 * \brief Apply a filter on the single pixel of an image.
 *
 * This function IS safe (pixels outside of the image will be
 * duplicates of the pixels at the sides of it).
 *
 * \param src Image we apply the filter on.
 * \param x X coordinate of the pixel of the image we apply the filter on.
 * \param y Y coordinate of the pixel of the image we apply the filter on.
 * \param filter Filter we apply on the image.
 * @return Result of the application of the filter on pixel (x,y) of the image.
 */
Color ApplyFilterOnSinglePixel(const Image *src, uint_fast32_t x, uint_fast32_t y, const Filter *filter);

/**
 * \fn void ApplyFilter(Image *dst, const Image *src, const Filter *filter)
 * \brief Apply filter on image.
 *
 * This function does not work in place.
 * This function is parallelized : number of threads is given by nbThreads
 * variable.
 *
 * \param dst Destination image.
 * \param src Source image.
 * \param filter Filter to apply.
 */
void ApplyFilter(Image *dst, const Image *src, const Filter *filter);

/**
 * \fn Action *LaunchApplyFilter(Image *dst, const Image *src, const Filter *filter)
 * \brief Launch apply filter action, but does not wait for termination.
 *
 * Action returned can be used to wait for termination or cancel filter applying.
 *
 * \param dst Destination image.
 * \param src Source image.
 * \param filter Filter to apply.
 * \return Corresponding newly-allocated action.
 */
Action *LaunchApplyFilter(Image *dst, const Image *src, const Filter *filter);

/**
 * \fn void FreeFilter(Filter filter)
 * \brief Free filter.
 *
 * \param filter Filter to be freed.
 */
void FreeFilter(Filter filter);

#ifdef __cplusplus
}
#endif

#endif
