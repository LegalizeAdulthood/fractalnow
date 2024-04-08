/*
 *  color.h -- part of fractal2D
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
  * \file color.h
  * \brief Header file related to colors.
  * \author Marc Pegon
  */

#ifndef __COLOR_H__
#define __COLOR_H__

#include <stdint.h>
#include "floating_point.h"

#define GET_R8(x) ((x & 0x00FF0000)>>16)
#define GET_G8(x) ((x & 0x0000FF00)>>8)
#define GET_B8(x) (x & 0x000000FF)

#define GET_R16(x) ((x & 0x0000FFFF00000000)>>32)
#define GET_G16(x) ((x & 0x00000000FFFF0000)>>16)
#define GET_B16(x) (x & 0x000000000000FFFF)

/**
 * \struct s_Color
 * \brief RGB8 color.
 */
typedef struct s_Color {
	int bytesPerComponent; /*!< Either 1 (RGB8) or 2 (RGB16).*/
	uint16_t r; /*!< Red component.*/
	uint16_t g; /*!< Green component.*/
	uint16_t b; /*!< Blue component.*/
} Color;

/**
 * \fn Color ColorFromUint32(uint32_t color)
 * \brief Convert a uint32_t color to Color structure.
 *
 * Create an RGB8 Color structure.
 *
 * \param color Color to convert.
 * \return Converted color.
 */
Color ColorFromUint32(uint32_t color);

/**
 * \fn Color ColorFromUint32(uint32_t color)
 * \brief Convert a uint64_t color to Color structure.
 *
 * Create an RGB16 Color structure.
 *
 * \param color Color to convert.
 * \return Converted color.
 */
Color ColorFromUint64(uint64_t color);

/**
 * \fn Color MixColors(Color C1, FLOAT weight1, Color C2, FLOAT weight2)
 * \brief Mix two weighted colors.
 *
 * Both colors must have same number of bytes per component,
 * and the function does not check for that : hence it is
 * NOT safe.
 *
 * \param C1 First Color.
 * \param weight1 Weight given to first color for mixing.
 * \param C2 Second Color.
 * \param weight2 Weight given to second color for mixing.
 * \return Result of the mixing of colors C1 and C2 according to given weights.
 */
Color MixColors(Color C1, FLOAT weight1, Color C2, FLOAT weight2);

#endif
