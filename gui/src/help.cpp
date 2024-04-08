/*
 *  help.cpp -- part of fractal2D
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
#include "fractal2D.h"
#include "floating_point.h"
#include "main.h"
#include "thread.h"
#include <inttypes.h>

void Help::Print()
{
	printf("qfractal2D v%s - Explore \
2D fractals.\n\
Usage : qfractal2D [-h] \n  \
or \n\
qfractal2D [-v] [-d] [-j <NbThreads>] [-c <FractalFile>] [-r \
<RenderingFile>] [-x <Width> | -y <Height>] [-m <MinAAMSize> \
-M <MaxAAMSize>] \n\n  \
-h : Prints this help.\n  \
-v : Verbose mode.\n  \
-d : Debug mode.\n  \
-j <NbThreads> : Specify the number of threads to compute image \
(%"PRIuFAST32" by default).\n  \
-f <FractalFile> : Specify the fractal config file, used for \
computing fractal. See documentation for details on syntax.\n  \
-r <RenderingFile> : Specify the rendering file, used for \
rendering fractal. See documentation for details on syntax.\n  \
-x <Width> : Specify output image width (%"PRIuFAST32" by \
default, or set to keep fractal space ratio if height is \
specified). \n  \
-y <Height> : Specify output image height (by default set \
to keep fractal space ratio). \n  \
-m <MinAAMSize> : Specify minimum size of adaptive \
anti-aliasing (%"PRIuFAST32" by default). Must be an \
integer strictly greater than 1.\n  \
-M <MinAAMSize> : Specify maximum size of adaptive \
anti-aliasing (%"PRIuFAST32" by default). Must be an \
integer strictly greater than 1.\n",
	fractal2D_VersionNumber(),
	DEFAULT_NB_THREADS,
	DEFAULT_FRACTAL_IMAGE_WIDTH,
	DEFAULT_MIN_ANTIALIASING_SIZE,
	DEFAULT_MAX_ANTIALIASING_SIZE);
}

