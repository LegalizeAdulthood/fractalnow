/*
 *  gradient.h -- part of fractal2D
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
  * \file gradient.h
  * \brief Header file related to gradients.
  * \author Marc Pegon
  */

#ifndef __GRADIENT_H__
#define __GRADIENT_H__

#include "color.h"
#include <stdint.h>

/**
 * \struct s_Gradient
 * \brief Gradient structure.
 *
 * This structure is to represent a gradient, i.e. continous
 * transitions between colors.
 */
typedef struct s_Gradient {
	uint32_t size;
 /*!< Gradient size (total number of colors).*/
	Color *data;
 /*!< Gradient data.*/
} Gradient;

/**
 * \fn void GenerateGradient(Gradient *gradient, Color *color, uint32_t size, uint32_t N)
 * \brief Generate a gradient.
 *
 * Generate a gradient from some colors and the number of transitions
 * between two colors.
 * All the colors in given array must have the same number of bytes
 * per component. This is not checked by the function, hence it
 * is NOT safe.
 *
 * \param gradient Pointer to the (already allocated) gradient structure to initialize.
 * \param color Pointer to a colors array.
 * \param size Size of the colors array (previous argument).
 * \param N Number of transitions between two colors in the gradient.
 */
void GenerateGradient(Gradient *gradient, Color *color, uint32_t size, uint32_t N);

/**
 * \fn Color GetGradientColor(Gradient *gradient, uint32_t index)
 * \brief Get a color in the gradient.
 *
 * Get the color in gradient at position index mod gradient_size
 * 
 * \param gradient Pointer to the gradient to get the color from.
 * \param index (mod.) Index of the color to get.
 * \return Color in gradient at position (index mod gradient_size).
 */
Color GetGradientColor(Gradient *gradient, uint32_t index);

/**
 * \fn void FreeGradient(Gradient *gradient)
 * \brief Free gradient.
 *
 * \param gradient Gradient to free.
 */
void FreeGradient(Gradient *gradient);

#endif
