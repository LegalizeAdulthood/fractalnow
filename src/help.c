/*
 *  help.c -- part of fractal2D
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
 
#include "help.h"
#include "fractal.h"
#include "thread.h"
#include <stdio.h>

void DisplayHelp()
{
	printf("\nDraw some Mandelbrot subset :\n\
Usage : fractal2D [-h] \n  \
or \n\
fractal2D [-q|-v] [-d] [-j <NbThreads>] -c <Config> -o <OutPut> \
[-x <Width>|-y <Height>] [-s <OSFactor>|-b <BlurRadius>] \
[-i <QuadSize>] [-t <Threshold>]\n  \
-h : Prints this help.\n  \
-q : Quiet mode.\n  \
-v : Verbose mode.\n  \
-d : Debug mode.\n  \
-j <NbThreads> : Specify the number of threads to compute image \
(%u by default).\n  \
-c <Config> : Specify the config file for drawing fractal. \
See config file syntax below.\n  \
-o <Output> : Specify the output image file (raw PPM).\n  \
-x <Width> : Specify output image width. \n  \
-y <Height> : Specify output image height. \n  \
-s <OSFactor> : Specify factor for oversampling (excellent \
but costly antialiasing method). For a very high quality image, 5 \
is a good value.\n  \
-b <BlurRadius> : Specify radius for gaussian blur (fast \
antialiasing method). Values in [2.5, 4] are generally sufficient.\n  \
-i <QuadSize> : Maximum size of quadrilateral for linear  \
interpolation (%u by default, which is good for no visible loss of \
quality). A value of 1 means no interpolation(all pixels are computed).\n\
Note that when oversampling factor is specified, this is used to compute \
the high resolution image, before downscaling.\n  \
-t <Threshold> : Dissimilarity threshold for colors (%lG by default, \
which is good for no visible loss of quality). \
A quadrilateral that shows too dissimilar colors at its corners will \
be computed (as opposed to interpolated). Value must be between \
0 and 1.\n\
Note that when oversampling factor is specified, this is used to compute \
the high resolution image, before downscaling.\n  \
Configuration file syntax :\n<CENTER_X> | <CENTER_Y> | <SPAN_X> | <SPAN_Y> | <ESCAPE_RADIUS> | <NB_ITER_MAX> | <COLOR_SCALING> | <SPACE_COLOR(hexa)> | <NB_TRANSITIONS> | <Color1(hexa)> | .. | <ColorN(hexa)>\n where '|' designates separators (tabs, blank spaces)\n",
	DEFAULT_NB_THREADS, DEFAULT_QUAD_INTERPOLATION_SIZE,
	DEFAULT_COLOR_DISSIMILARITY_THRESHOLD);
}
