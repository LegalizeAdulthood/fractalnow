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
#include "floating_point.h"
#include "fractal.h"
#include "fractal2D.h"
#include "thread.h"
#include <stdio.h>
#include <inttypes.h>

void DisplayHelp()
{
	printf("fractal2D v%s - Generate 2D fractal images.\n\
Usage : fractal2D [-h] \n  \
or \n\
fractal2D [-q|-v] [-d] [-j <NbThreads>] -c <FractalFile> -r \
<RenderingFile> -o <OutPut> [-x <Width>|-y <Height>] [-a \
<AntiAliasingMethod> -s <AAMSize> [-p <AdaptiveAAMThreshold>]] \
[-i <QuadSize>] [-t <Threshold>]\n\n  \
-h : Prints this help.\n  \
-q : Quiet mode.\n  \
-v : Verbose mode.\n  "
#ifdef DEBUG
"-d : Debug mode.\n  "
#endif
"-j <NbThreads> : Specify the number of threads to compute image \
(%"PRIuFAST32" by default).\n  \
-f <FractalFile> : Specify the fractal file, used for \
computing fractal. See documentation for details on syntax.\n  \
-r <RenderingFile> : Specify the rendering file, used for \
rendering fractal. See documentation for details on syntax.\n  \
-o <Output> : Specify the output image file (raw PPM).\n  \
-x <Width> : Specify output image width. \n  \
-y <Height> : Specify output image height. \n  \
-a <AntiAliasingMethod> : Specify anti-aliasing method ; either \
none (by default), blur, oversampling (generate bigger image and \
downscale it), or adaptive (similar to oversampling but applied \
to some pixels only). \n  \
-s <AAMSize> : Specify size of anti-aliasing ; radius for blur \
(values in [2.5, 4] are generally satisfying), or scale factor \
for oversampling (3-5 is good for a high quality image), \
or adaptive anti-aliasing (must be integer in that last case).\
\n  \
-p <AAMThreshold> : Threshold for adaptive anti-aliasing (\
%"PRIFLOAT" by default, which is good for results generally \
similar to oversampling with same size factor). Value must be \
greater than 0.\n  \
-i <QuadSize> : Maximum size of quadrilateral for linear \
interpolation (%"PRIuFAST32" by default, which is good for no \
visible loss of quality). A value of 1 means no interpolation \
(all pixels are computed).\n\
Note that when oversampling factor is specified, this is \
used to compute the high resolution image, before downscaling.\n  \
-t <Threshold> : Dissimilarity threshold for quad interpolation \
(%"PRIFLOAT" by default, which is good for no \
visible loss of quality). A quadrilateral that shows too \
dissimilar values at its corners will be computed (as opposed \
to interpolated). Value must be greater than 0. Note that when \
oversampling factor is specified, this is used to compute \
the high resolution image, before downscaling.\n",
	fractal2D_VersionNumber(),
	DEFAULT_NB_THREADS,
	FLOAT_DIG, DEFAULT_ADAPTIVE_AAM_THRESHOLD,
	DEFAULT_QUAD_INTERPOLATION_SIZE,
	FLOAT_DIG, DEFAULT_COLOR_DISSIMILARITY_THRESHOLD);
}

