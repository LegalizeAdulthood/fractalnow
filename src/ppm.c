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

void aux_ExportPPM8(char *fileName, Image *image) {
	FILE *file;

	file = fopen(fileName,"w");
	if (!file) {
		open_error(fileName);
	}

	fprintf(file,"P6\n%lu %lu\n%lu\n",(unsigned long)image->width,
					(unsigned long)image->height,
					(unsigned long)255);

	Color *pixel = image->data;
	for (uint32_t i=0; i<image->height; i++) {
		for (uint32_t j=0; j<image->width; j++) {
			fprintf(file, "%c%c%c", (uint8_t)pixel->r,
				(uint8_t)pixel->g, (uint8_t)pixel->b);
			pixel++;
		}
	}

	if (fclose(file)) {
		close_error(fileName);
	}

}

void aux_ExportPPM16(char *fileName, Image *image) {
	FILE *file;

	file = fopen(fileName,"w");
	if (!file) {
		open_error(fileName);
	}

	fprintf(file,"P6\n%lu %lu\n%lu\n",(unsigned long)image->width,
					(unsigned long)image->height,
					(unsigned long)65535);

	Color *pixel = image->data;
	for (uint32_t i=0; i<image->height; i++) {
		for (uint32_t j=0; j<image->width; j++) {
			fprintf(file, "%c%c%c%c%c%c",
				(uint8_t)(pixel->r >> 8),(uint8_t)(pixel->r & 0xFF),
				(uint8_t)(pixel->g >> 8),(uint8_t)(pixel->g & 0xFF),
				(uint8_t)(pixel->b >> 8),(uint8_t)(pixel->b & 0xFF));
			pixel++;
		}
	}

	if (fclose(file)) {
		close_error(fileName);
	}

}

void ExportPPM(char *fileName, Image *image)
{
	info(T_NORMAL, "Exporting PPM \'%s\'...\n", fileName);

	switch (image->bytesPerComponent) {
	case 1:
		aux_ExportPPM8(fileName, image);
		break;
	case 2:
		aux_ExportPPM16(fileName, image);
		break;
	default:
		error("Invalid image bytes per component.\n");
		break;
	}

	info(T_NORMAL, "Exporting PPM \'%s\' : DONE.\n", fileName);
}

