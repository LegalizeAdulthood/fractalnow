/*
 *  color.h -- part of FractalNow
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

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include "floating_point.h"

/**
 * \def GET_R8(x)
 * \brief Get red component of uint32_t color.
 */
#define GET_R8(x) ((x >> 16) & 0xFF)

/**
 * \def GET_G8(x)
 * \brief Get green component of uint32_t color.
 */
#define GET_G8(x) ((x >> 8) & 0xFF)

/**
 * \def GET_B8(x)
 * \brief Get blue component of uint32_t color.
 */
#define GET_B8(x) (x & 0xFF)

/**
 * \def RGB8_TO_UINT32(r,g,b)
 * \brief Create uint32_t color from 8-bits red, green, and blue components.
 *
 * alpha is set to 0xFF by default.
 */
#define RGB8_TO_UINT32(r,g,b) (0xFF000000+((uint32_t)(r)<<16)+((uint32_t)(g)<<8)+(uint32_t)b)

/**
 * \def ARGB8_TO_UINT32(a,r,g,b)
 * \brief Create uint32_t color from 8-bits alpha, red, green, and blue components.
 */
#define ARGB8_TO_UINT32(a,r,g,b) (((uint32_t)(a)<<24)+((uint32_t)(r)<<16)+((uint32_t)(g)<<8)+(uint32_t)b)

/**
 * \def GET_R16(x)
 * \brief Get red component of uint64_t color.
 */
#define GET_R16(x) ((x >> 32) & 0xFFFF)

/**
 * \def GET_G16(x)
 * \brief Get green component of uint64_t color.
 */
#define GET_G16(x) ((x >> 16) & 0xFFFF)

/**
 * \def GET_B16(x)
 * \brief Get blue component of uint64_t color.
 */
#define GET_B16(x) (x & 0xFFFF)

/**
 * \def RGB16_TO_UINT64(r,g,b)
 * \brief Create uint64_t color from 16-bits red, green, and blue components.
 *
 * alpha is set to 0xFFFF by default.
 */
#define RGB16_TO_UINT64(r,g,b) (0xFFFF000000000000+((uint64_t)(r)<<32)+((uint64_t)(g)<<16)+(uint64_t)b)

/**
 * \def ARGB16_TO_UINT64(r,g,b)
 * \brief Create uint64_t color from 16-bits alpha, red, green, and blue components.
 */
#define ARGB16_TO_UINT64(a,r,g,b) (((uint64_t)(a)<<48)+((uint64_t)(r)<<32)+((uint64_t)(g)<<16)+(uint64_t)b)

/**
 * \struct Color
 * \brief Simple RGB color structure.
 */
/**
 * \typedef Color
 * \brief Convenient typedef for struct Color.
 */
typedef struct Color {
	uint_fast8_t bytesPerComponent; /*!< Either 1 (RGB8) or 2 (RGB16).*/
	uint_fast16_t r; /*!< Red component.*/
	uint_fast16_t g; /*!< Green component.*/
	uint_fast16_t b; /*!< Blue component.*/
} Color;

/**
 * \fn Color ColorFromRGB(uint8_t bytesPerComponent, uint16_t r, uint16_t g, uint16_t b)
 * \brief Create color from RGB values.
 *
 * bytesPerComponent must be 1 or 2.
 *
 * \param bytesPerComponent Bytes per component.
 * \param r Red value.
 * \param g Green value.
 * \param b Blue value.
 * \return Corresponding color structure.
 */
Color ColorFromRGB(uint8_t bytesPerComponent, uint16_t r, uint16_t g, uint16_t b);

/**
 * \fn Color ColorFromUint32(uint32_t color)
 * \brief Convert a uint32_t color to Color structure.
 *
 * Creates a RGB8 Color structure.
 * alpha component (most significant 8 bits) is ignored.
 *
 * \param color Color to convert.
 * \return Converted color.
 */
Color ColorFromUint32(uint32_t color);

/**
 * \fn Color ColorFromUint64(uint64_t color)
 * \brief Convert a uint64_t color to Color structure.
 *
 * Creates a RGB16 Color structure.
 * alpha component (most significant 16 bits) is ignored.
 *
 * \param color Color to convert.
 * \return Converted color.
 */
Color ColorFromUint64(uint64_t color);

/**
 * \fn Color Color16(Color color)
 * \brief Convert color to 16 bits color.
 *
 * Simple copy if color bytes per component is already 2.
 *
 * \param color Color to be converted.
 * \return 16 bits color.
 */
Color Color16(Color color);

/**
 * \fn Color Color8(Color color)
 * \brief Convert color to 8 bits color.
 *
 * Simple copy if color bytes per component is already 1.
 *
 * \param color Color to be converted.
 * \return 8 bits color.
 */
Color Color8(Color color);

/**
 * \fn int CompareColors(const Color color1, const Color color2)
 * \brief Compare two colors.
 *
 * \param color1 First color.
 * \param color2 Second color.
 * \return 0 if colors are the same, 1 otherwise.
 */
int CompareColors(const Color color1, const Color color2);

/**
 * \fn Color MixColors(Color C1, FLOATT weight1, Color C2, FLOATT weight2)
 * \brief Mix two weighted colors.
 *
 * Both colors must have same number of bytes per component
 * (undefined behaviour otherwise).
 *
 * \param C1 First Color.
 * \param weight1 Weight given to first color for mixing.
 * \param C2 Second Color.
 * \param weight2 Weight given to second color for mixing.
 * \return Result of the mixing of colors C1 and C2 according to given weights.
 */
Color MixColors(Color C1, FLOATT weight1, Color C2, FLOATT weight2);

/**
 * \fn FLOATT ColorManhattanDistance(Color C1, Color C2)
 * \brief Compute manhattan distance between two colors.
 *
 * Distance is normalized (i.e. between 0 and 1).\n
 * Both colors must have same number of bytes per component
 * (undefined behaviour otherwise).
 *
 * \param C1 First color.
 * \param C2 Second color.
 * \return Manhattan distance between colors C1 and C2.
 */
FLOATT ColorManhattanDistance(Color C1, Color C2);

/**
 * \fn FLOATT QuadAvgDissimilarity(const Color C[4])
 * \brief Compute average dissimilarity of a quadrilateral given its corner colors.
 *
 * Result is normalized (between 0 and 1).\n
 * Colors must have same number of bytes per component
 * (undefined behaviour otherwise).
 *
 * \param C Point to colors at the four corners.
 * \return Quadrilateral average anhattan dissimilarity.
 */
FLOATT QuadAvgDissimilarity(const Color C[4]);

/**
 * \fn Color QuadLinearInterpolation(const Color C[4], FLOATT x, FLOATT y)
 * \brief Interpolate linearly some color of a quadrilateral.
 *
 * Interpolate color at point (x,y) according to its corner colors.\n
 * Coordinates x and y are relative (undefined behaviour otherwise) :
 * (0,0 is the top left corner, (1,0) the top right, etc...\n
 * Colors must have same number of bytes per component
 * (undefined behaviour otherwise).
 *
 * \param C Quadrilateral corner colors.
 * \param x X (relative) coordinate of quad point to interpolate.
 * \param y Y (relative) coordinate of quad point to interpolate.
 * \return Linearly interpolated color at point (x,y) of quadrilateral.
 */
Color QuadLinearInterpolation(const Color C[4], FLOATT x, FLOATT y);

#ifdef __cplusplus
}
#endif

#endif
