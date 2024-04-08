/*
 *  help.c -- part of FractalNow
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
#include "fractalnow.h"
#include <float.h>
#include <inttypes.h>
#include <stdio.h>

void DisplayHelp()
{
	printf("fractalnow v%s - Generate fractal pictures (raw \
PPM format).\n\n\
Usage : fractalnow -h\n\
  or\n\
fractalnow [OPTIONS] -c <Config> [-x <Width>|-y <Height>] -o \
<Output>\n\
  or\n\
fractalnow [OPTIONS] -f <Fractal> -r <Rendering> [-x <Width>\
|-y <Height>] -o <Output>\n\
\n\
OPTIONS:\n"
#ifdef DEBUG
"  -d                       Debug mode.\n"
#endif
"  -j <NbThreads>           Specify number of threads \
(%"PRIuFAST32" by default).\n\
  -g <GradientFile>        Specify gradient file, overriding \
gradient from configuration/rendering file.\n\
  -x <Width>               Specify output image width.\n\
  -y <Height>              Specify output image height.\n\
  -l <FloatType>           Specify float type:\n\
                               single        Single precision.\n\
                               double        Double precision.\n\
                               ldouble       Long double \
precision.\n\
                               mp            Multiple \
precision.\n\
  -L <MPPrecision>         Specify precision for Multiple \
Precision (MP) floats (%ld by default).\n\
  -a <AntiAliasingMethod>  Specify anti-aliasing method:\n\
                               none          By default.\n\
			       blur          Gaussian blur.\n\
			       oversampling\n\
			       adaptive      Smart oversampling.\
\n\
  -s <AAMSize>             Specify size for anti-aliasing:\n\
                               Radius for blur (values in \
[2.5, 4] are generally good).\n\
                               Scale factor for oversampling ([3\
, 5] is good for a high quality image).\n\
                               Scale factor for adaptive (\
integers between 3-5 are good for a high quality image).\n\
  -p <AAMThreshold>        Threshold for adaptive \
anti-aliasing (%.*lf by default).\n\
  -i <QuadSize>            Maximum size of quadrilaterals for \
linear interpolation.\n\
                           %"PRIuFAST32" by default, which is \
good for no visible loss of quality.\n\
                           1 means no interpolation (all \
pixels are computed).\n\
  -t <Threshold>           Dissimilarity threshold for quad \
interpolation.\n\
                           %.*lf by default, which is \
good for no visible loss of quality.\n\
                           A quadrilateral that shows too \
dissimilar values at its corners will be computed, \
as opposed to interpolated.\n",
	FractalNow_VersionNumber(),
	DEFAULT_NB_THREADS,
	(long int)DEFAULT_MPFR_PRECISION,
	DBL_DIG, DEFAULT_ADAPTIVE_AAM_THRESHOLD,
	DEFAULT_QUAD_INTERPOLATION_SIZE,
	DBL_DIG, DEFAULT_COLOR_DISSIMILARITY_THRESHOLD);
}

