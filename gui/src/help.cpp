/*
 *  help.cpp -- part of FractalNow
 *
 *  Copyright (c) 2012 Marc Pegon <pe.marc@free.fr>
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
#include "main.h"

#include <float.h>
#include <inttypes.h>

void Help::Print()
{
	printf("qfractalnow v%s - Explore fractals and generate \
fractal images.\n\n\
Usage : qfractalnow [OPTIONS]\n\n\
OPTIONS:\n\
  -h                       Prints this help.\n\
  -q                       Quiet mode.\n\
  -v                       Verbose mode.\n"
#ifdef DEBUG
"  -d                       Debug mode.\n"
#endif
"  -j <NbThreads>           Specify number of threads \
(%"PRIuFAST32" by default).\n\
  -c <ConfigFile>          Specify configuration file, \
overriding default configuration.\n\
  -f <FractalFile>         Specify fractal file, overriding \
parameters from default configuration or configuration file.\n\
  -r <RenderingFile>       Specify rendering file, overriding \
parameters from default configuration or configuration file.\n\
  -g <GradientFile>        Specify gradient file, overriding \
gradient from default configuration, configuration file, or \
rendering file.\n\
  -x <Width>               Specify image width.\n\
  -y <Height>              Specify image height.\n\
  -l <FloatType>           Specify float precision:\n\
                               single        Single precision.\n\
                               double        Double precision.\n"
#ifdef _ENABLE_LDOUBLE_FLOATS
"                               ldouble       Long double \
precision.\n"
#endif
#ifdef _ENABLE_MP_FLOATS
"                               mp            Multiple \
precision.\n\
  -L <MPPrecision>         Specify precision for Multiple \
Precision (MP) floats (%"PRId64" by default).\n"
#endif
"  -m <MinAAMSize>          Specify minimum size of adaptive \
anti-aliasing (%"PRIuFAST32" by default).\n\
                           Must be an integer strictly greater \
than 1.\n\
  -M <MaxAAMSize>          Specify maximum size of adaptive \
anti-aliasing (%"PRIuFAST32" by default).\n\
                           Must be an integer strictly greater \
than 1 and MinAAMSize.\n\
  -n <AAMSizeIteration>    Anti-aliasing size iteration.\n\
                           Anti-aliasing size will increase by \
steps of AAMSizeIteration from MinAAMSize to MaxAAMSize.\n\
  -p <AAMThreshold>        Threshold for adaptive \
anti-aliasing (%G by default).\n\
  -i <QuadSize>            Maximum size of quadrilaterals for \
linear interpolation.\n\
                           %"PRIuFAST32" by default, which is \
good for no visible loss of quality.\n\
                           1 means no interpolation (all \
pixels are computed).\n\
  -t <Threshold>           Dissimilarity threshold for quad \
interpolation.\n\
                           %G by default, which is \
good for no visible loss of quality.\n\
                           A quadrilateral that shows too \
dissimilar values at its corners will be computed, \
as opposed to interpolated.\n",
	FractalNow_VersionNumber(),
	DEFAULT_NB_THREADS,
#ifdef _ENABLE_MP_FLOATS
	DEFAULT_MP_PRECISION,
#endif
	DEFAULT_MIN_ANTIALIASING_SIZE,
	DEFAULT_MAX_ANTIALIASING_SIZE,
	DEFAULT_ADAPTIVE_AAM_THRESHOLD,
	DEFAULT_QUAD_INTERPOLATION_SIZE,
	DEFAULT_COLOR_DISSIMILARITY_THRESHOLD);
}

