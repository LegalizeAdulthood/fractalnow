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
	printf("fractalnow v%s - Generate fractal images.\n\
Usage : fractalnow [-h] \n\
  or \n\
fractalnow [-q|-v] [-d] [-j <NbThreads>] \n\
          [-c <ConfigFile>|-f <FractalFile> -r \
<RenderingFile>] [-g <GradientFile]\n\
          -o <OutPut> [-x <Width>|-y <Height>]\n\
	  -l <FloatType> [-L <MPFloatPrecision>]\n\
	  [-a <AntiAliasingMethod> -s <AAMSize> [-p \
<AdaptiveAAMThreshold>]] [-i <QuadSize>] [-t <Threshold>]\n\n\
  -h                       Prints this help.\n\
  -q                       Quiet mode.\n\
  -v                       Verbose mode.\n"
#ifdef DEBUG
"  -d                       Debug mode.\n"
#endif
"  -j <NbThreads>           Specify number of threads \
(%"PRIuFAST32" by default).\n\
  -c <ConfigFile>          Specify configuration file. See \
documentation for details on configuration files.\n\
  -f <FractalFile>         Specify fractal file. See \
documentation for details on fractal files.\n\
                           If a configuration file was already \
specified,\n\
                           fractal file will override \
overlapping parameters.\n\
  -r <RenderingFile>       Specify rendering file. See \
documentation for details on rendering files.\n\
                           If a configuration file was already \
specified,\n\
                           rendering file will override the \
overlapping parameters.\n\
  -g <GradientFile>        Specify gradient file. See \
documentatin for details on gradient file.\n\
                           If a configuration file and/or a \
rendering file were already specified,\n\
                           gradient file will override the \
overlapping parameters.\n\
  -o <Output>              Specify output image file (raw PPM).\
\n\
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
dissimilar values at its corners will be computed, \n\
                           as opposed to interpolated.\n",
	FractalNow_VersionNumber(),
	DEFAULT_NB_THREADS,
	(long int)DEFAULT_MPFR_PRECISION,
	DBL_DIG, DEFAULT_ADAPTIVE_AAM_THRESHOLD,
	DEFAULT_QUAD_INTERPOLATION_SIZE,
	DBL_DIG, DEFAULT_COLOR_DISSIMILARITY_THRESHOLD);
}

