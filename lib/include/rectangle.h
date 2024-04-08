/*
 *  rectangle.h -- part of fractal2D
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
  * \file rectangle.h
  * \brief Header file for a basic rectangle type.
  * \author Marc Pegon
  */

#ifndef __RECTANGLE_H__
#define __RECTANGLE_H__

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>

/**
 * \struct Rectangle
 * \brief Basic rectangle type.
 */
/**
 * \typedef Rectangle
 * \brief Convenient typedef for struct Rectangle.
 */
typedef struct Rectangle {
	uint_fast32_t x1;
 /*!< X coordinate of the rectangle's top left corner.*/
	uint_fast32_t y1;
 /*!< Y coordinate of the rectangle's top left corner.*/
	uint_fast32_t x2;
 /*!< X coordinate of the rectangle's bottom right corner.*/
	uint_fast32_t y2;
 /*!< Y coordinate of the rectangle's bottom right corner.*/
} Rectangle;

/**
 * \fn void InitRectangle(Rectangle *rectangle, uint_fast32_t x1, uint_fast32_t y1, uint_fast32_t x2, uint_fast32_t y2)
 * \brief Initialize rectangle.
 *
 * x1 must be less than x2 and y1 must be less than y2
 * (undefined behaviour otherwise).
 *
 * \param rectangle Pointer to rectangle structure to initialize.
 * \param x1 X coordinate of the rectangle's top left corner.
 * \param y1 Y coordinate of the rectangle's top left corner.
 * \param x2 X coordinate of the rectangle's bottom right corner.
 * \param y2 Y coordinate of the rectangle's bottom right corner.
 */
void InitRectangle(Rectangle *rectangle, uint_fast32_t x1, uint_fast32_t y1, uint_fast32_t x2, uint_fast32_t y2);

/**
 * \fn Rectangle CopyRectangle(const Rectangle *rectangle)
 * \brief Copy rectangle.
 *
 * \param rectangle Pointer to rectangle to copy.
 * \return Copy of rectangle.
 */
Rectangle CopyRectangle(const Rectangle *rectangle);

/**
 * \fn int CutRectangleInHalf(Rectangle rectangle, Rectangle *out1, Rectangle *out2)
 * \brief Cut rectangle in half.
 *
 * Cut rectangle into two rectangles.
 * If the rectangle cannot be cut in half, out1 and out2 will be left unchanged.
 *
 * \param rectangle Rectangle to be cut in half (will not be modified).
 * \param out1 Pointer to put the first half of the cut rectangle.
 * \param out2 Pointer to put the second half of the cut rectangle.
 * \return 1 if rectangle cannot be cut in half, 0 otherwise.
 */
int CutRectangleInHalf(Rectangle rectangle, Rectangle *out1, Rectangle *out2);

/**
 * \fn void CutRectangleMaxSize(Rectangle src, uint_fast32_t size, Rectangle **out, uint_fast32_t *out_size)
 * \brief Cut rectangle into smaller rectangles.
 *
 * Cut rectangle into smaller rectangles. Each of these rectangles are smaller
 * (i.e. both width and height) than given size.
 * 
 * \param src Rectangle to be cut into smaller rectangles.
 * \param size Maximum size of the small rectangles produced.
 * \param out Pointer to a (not yet allocated) array of rectangles for the output.
 * \param out_size Pointer to an integer to store the number of small rectangles produced.
 */
void CutRectangleMaxSize(Rectangle src, uint_fast32_t size, Rectangle **out, uint_fast32_t *out_size);

/**
 * \fn int CutRectangleInN(Rectangle rectangle, uint_fast32_t N, Rectangle *out)
 * \brief Cut rectangle in N parts.
 *
 * Cut rectangle into N rectangles.
 * If the rectangle cannot be cut in N, the out array will be left unchanged.
 *
 * \param rectangle Rectangle to be cut in N.
 * \param N Rectangle is to be cut in N rectangles.
 * \param out Pointer to the (allocated) array in which to put the N cut rectangles.
 * \return 1 if rectangle cannot be cut in N, 0 otherwise.
 */
int CutRectangleInN(Rectangle rectangle, uint_fast32_t N, Rectangle *out);

#ifdef __cplusplus
}
#endif

#endif
