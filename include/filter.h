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
 
#ifndef __FILTER_H__
#define __FILTER_H__

#include <stdint.h>
#include "image.h"

typedef struct s_Filter
{
	uint32_t sx,sy;
	uint32_t cx,cy; // center of the filter if not n*n with n odd
	double *data;
} Filter;

void InitFilter(Filter *filter, uint32_t sx, uint32_t sy, uint32_t cx, uint32_t cy, double *data);
void InitFilter2(Filter *filter, uint32_t sx, uint32_t sy, double *data);
void CreateHorizontalGaussianFilter(Filter *filter, double sigma);
void CreateHorizontalGaussianFilter2(Filter *filter, double radius);
void CreateVerticalGaussianFilter(Filter *filter, double sigma);
void CreateVerticalGaussianFilter2(Filter *filter, double radius);
void CreateGaussianFilter(Filter *filter, double sigma);
void CreateGaussianFilter2(Filter *filter, double radius);
void FreeFilter(Filter *filter);

double GetFilterValue(Filter *filter, uint32_t x, uint32_t y);
void MultiplyFilterByScalar(Filter *filter, double scalar);
void NormalizeFilter(Filter *filter);
Color ApplyFilterOnSinglePixel(Image *src, uint32_t x, uint32_t y, Filter *filter);
void ApplyFilter(Image *dst, Image *src, Filter *filter);

#endif
