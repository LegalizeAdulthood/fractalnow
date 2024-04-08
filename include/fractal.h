/*
 *  fractal.h -- part of fractal2D
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
 
#ifndef __FRACTAL_H__
#define __FRACTAL_H__

#include "gradient.h"
#include "image.h"
#include "rectangle.h"
#include "thread.h"
#include <stdint.h>

#define DEFAULT_QUAD_INTERPOLATION_SIZE 10
#define DEFAULT_COLOR_DISSIMILARITY_THRESHOLD 1.95E-2

typedef struct s_Fractal {
	double x1, x2;
	double y1, y2;
	double escapeRadius;
	uint32_t maxIter;
} Fractal;

void InitFractal(Fractal *fractal, double x1, double y1, double x2, double y2,
		double escapeRadius, uint32_t maxIter);
void InitFractal2(Fractal *fractal, double centerX, double centerY, double spanX,
		double spanY, double escapeRadius, uint32_t maxIter);

Color ComputeFractalPixel(Fractal *fractal, Gradient *gradient, Color spaceColor,
			double multiplier, uint32_t width, uint32_t height,
			uint32_t x, uint32_t y);

void DrawFractalFast(Image *image, Fractal *fractal, Gradient *gradient, Color spaceColor,
		double multiplier, uint32_t quadInterpolationSize, double interpolationThreshold);
void DrawFractal(Image *image, Fractal *fractal, Gradient *gradient, Color spaceColor,
		double multiplier);
#endif
