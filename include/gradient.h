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
 
#ifndef __GRADIENT_H__
#define __GRADIENT_H__

#include "color.h"
#include <stdint.h>

typedef struct s_Gradient {
	Color *data;
	uint32_t size;
} Gradient;

void GenerateGradient(Gradient *gradient, Color *color, uint32_t size, uint32_t N);
Color GetGradientColor(Gradient *gradient, uint32_t index);
void FreeGradient(Gradient *gradient);

#endif
