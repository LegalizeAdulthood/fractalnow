/*
 *  gradient.c -- part of fractal2D
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
 
#include "error.h"
#include "gradient.h"

void aux_GenerateGradient(Gradient *gradient, uint32_t ind_tab, Color C1, Color C2, uint32_t N)
{
	Color *pixel = gradient->data+ind_tab;
	for (uint32_t i=0;i<N;i++) {
		pixel->r = (C1.r*(N-1-i) + C2.r*i) / (N-1);
		pixel->g = (C1.g*(N-1-i) + C2.g*i) / (N-1);
		pixel->b = (C1.b*(N-1-i) + C2.b*i) / (N-1);
		++pixel;
	}
}

void GenerateGradient(Gradient *gradient, Color *color, uint32_t size, uint32_t N)
{
	info(T_NORMAL,"Generating gradient...\n");
	
	gradient->data = malloc(N*(size-1)*sizeof(Color));
	if (gradient->data == NULL) {
		alloc_error("gradient");
	}

	for (uint32_t i=0; i<size-1; i++) {
		aux_GenerateGradient(gradient, i*N, color[i], color[i+1],N);
	}

	gradient->size = (size-1)*N;

	info(T_NORMAL,"Generating gradient : DONE.\n");
}

inline Color GetGradientColor(Gradient *gradient, uint32_t index)
{
	return gradient->data[index % gradient->size];
}

void FreeGradient(Gradient *gradient)
{
	free(gradient->data);
}
