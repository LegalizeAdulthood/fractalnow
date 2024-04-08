/*
 *  uirectangle.h -- part of FractalNow
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
  * \file uirectangle.h
  * \brief Header file related to UIUIRectangle structure.
  * \author Marc Pegon
  */

#ifndef __UIRECTANGLE_H__
#define __UIRECTANGLE_H__

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * \struct UIRectangle
 * \brief Simple rectangle structure made of unsigned integers.
 */
/**
 * \typedef UIRectangle
 * \brief Convenient typedef for struct UIRectangle.
 */
typedef struct UIRectangle {
	uint_fast32_t x1;
 /*!< X coordinate of the rectangle's top left corner.*/
	uint_fast32_t y1;
 /*!< Y coordinate of the rectangle's top left corner.*/
	uint_fast32_t x2;
 /*!< X coordinate of the rectangle's bottom right corner.*/
	uint_fast32_t y2;
 /*!< Y coordinate of the rectangle's bottom right corner.*/
} UIRectangle;

/**
 * \fn void InitUIRectangle(UIRectangle *rectangle, uint_fast32_t x1, uint_fast32_t y1, uint_fast32_t x2, uint_fast32_t y2)
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
void InitUIRectangle(UIRectangle *rectangle, uint_fast32_t x1, uint_fast32_t y1, uint_fast32_t x2, uint_fast32_t y2);

/**
 * \fn UIRectangle CopyUIRectangle(const UIRectangle *rectangle)
 * \brief Copy rectangle.
 *
 * \param rectangle Pointer to rectangle to copy.
 * \return Copy of rectangle.
 */
UIRectangle CopyUIRectangle(const UIRectangle *rectangle);

/**
 * \fn int CutUIRectangleInHalf(UIRectangle rectangle, UIRectangle *out1, UIRectangle *out2)
 * \brief Cut rectangle in half.
 *
 * Cut rectangle into two rectangles.\n
 * If the rectangle cannot be cut in half, out1 and out2 will be left unchanged.
 *
 * \param rectangle Rectangle to be cut in half (will not be modified).
 * \param out1 Pointer to put the first half of the cut rectangle.
 * \param out2 Pointer to put the second half of the cut rectangle.
 * \return 1 if rectangle cannot be cut in half, 0 otherwise.
 */
int CutUIRectangleInHalf(UIRectangle rectangle, UIRectangle *out1, UIRectangle *out2);

/**
 * \fn void CutUIRectangleMaxSize(UIRectangle src, uint_fast32_t size, UIRectangle **out, uint_fast32_t *out_size)
 * \brief Cut rectangle into smaller rectangles.
 *
 * Cut rectangle into smaller rectangles. Each of these rectangles is smaller
 * (i.e. both width and height) than given size.
 * 
 * \param src Rectangle to be cut into smaller rectangles.
 * \param size Maximum size of the small rectangles produced.
 * \param out Pointer to a (not yet allocated) array of rectangles for the output.
 * \param out_size Pointer to an integer to store the number of small rectangles produced.
 */
void CutUIRectangleMaxSize(UIRectangle src, uint_fast32_t size, UIRectangle **out, uint_fast32_t *out_size);

/**
 * \fn int CutUIRectangleInN(UIRectangle rectangle, uint_fast32_t N, UIRectangle *out)
 * \brief Cut rectangle in N parts.
 *
 * Cut rectangle into N rectangles.\n
 * If the rectangle cannot be cut in N, the out array will be left unchanged.
 *
 * \param rectangle Rectangle to be cut in N.
 * \param N Rectangle is to be cut in N rectangles.
 * \param out Pointer to the (allocated) array in which to put the N cut rectangles.
 * \return 1 if rectangle cannot be cut in N, 0 otherwise.
 */
int CutUIRectangleInN(UIRectangle rectangle, uint_fast32_t N, UIRectangle *out);

#ifdef __cplusplus
}
#endif

#endif
