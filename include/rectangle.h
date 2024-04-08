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

#include <stdint.h>

/**
 * \struct s_Rectangle
 * \brief Basic rectangle type.
 */
typedef struct s_Rectangle {
	uint32_t x1;
 /*!< X coordinate of the rectangle's top left corner.*/
	uint32_t y1;
 /*!< Y coordinate of the rectangle's top left corner.*/
	uint32_t x2;
 /*!< X coordinate of the rectangle's bottom right corner.*/
	uint32_t y2;
 /*!< Y coordinate of the rectangle's bottom right corner.*/
} Rectangle;

/**
 * \fn void InitRectangle(Rectangle *rectangle, uint32_t x1, uint32_t y1, uint32_t x2, uint32_t y2)
 * \brief Initialize rectangle.
 *
 * \param rectangle Pointer to rectangle structure to initialize.
 * \param x1 X coordinate of the rectangle's top left corner.
 * \param y1 Y coordinate of the rectangle's top left corner.
 * \param x2 X coordinate of the rectangle's bottom right corner.
 * \param y2 Y coordinate of the rectangle's bottom right corner.
 */
void InitRectangle(Rectangle *rectangle, uint32_t x1, uint32_t y1, uint32_t x2, uint32_t y2);

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
 * \fn int CutRectangleInN(Rectangle rectangle, uint32_t N, Rectangle *out)
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
int CutRectangleInN(Rectangle rectangle, uint32_t N, Rectangle *out);

#endif
