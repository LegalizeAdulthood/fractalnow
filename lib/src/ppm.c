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
#include <inttypes.h>

void aux_ExportPPM8(char *fileName, Image *image) {
	FILE *file;

	file = fopen(fileName,"w");
	if (!file) {
		open_error(fileName);
	}

	fprintf(file,"P6\n%"PRIuFAST32" %"PRIuFAST32"\n%"PRIu8"\n",
		image->width, image->height, (uint8_t)UINT8_MAX);

	if (image->width != 0 && image->height != 0) {
		uint8_t *bytes = ImageToBytesArray(image);
		fwrite(bytes, 3, image->width*image->height, file);
		free(bytes);
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

	fprintf(file,"P6\n%"PRIuFAST32" %"PRIuFAST32"\n%"PRIu16"\n",
		image->width, image->height, (uint16_t)UINT16_MAX);

	if (image->width != 0 && image->height != 0) {
		uint8_t *bytes = ImageToBytesArray(image);
		fwrite(bytes, 6, image->width*image->height, file);
		free(bytes);
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

