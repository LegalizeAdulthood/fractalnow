/*
 *  ppm.c -- part of fractal2D
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
 
#include "ppm.h"
#include "error.h"

void ExportPPM(char *fileName, Image *image)
{
	FILE *file;

	info(T_NORMAL, "Exporting PPM \'%s\'...\n", fileName);
	file = fopen(fileName,"w");
	if (!file) {
		open_error(fileName);
	}

	fprintf(file,"P6\n%u %u\n%d\n",image->width,image->height,255);

	Color *pixel = image->data;
	for (uint32_t i=0; i<image->height; i++) {
		for (uint32_t j=0; j<image->width; j++) {
			fprintf(file, "%c%c%c", pixel->r, pixel->g, pixel->b);
			pixel++;
		}
	}

	if (fclose(file)) {
		close_error(fileName);
	}

	info(T_NORMAL, "Exporting PPM \'%s\' : DONE.\n", fileName);
}

