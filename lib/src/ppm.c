/*
 *  ppm.c -- part of FractalNow
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

int aux_ExportPPM8(const char *fileName, const Image *image)
{
	FILE *file;
	int res = 0;

	file = fopen(fileName,"wb");
	if (!file) {
		FractalNow_open_werror(fileName);
	}

	fprintf(file,"P6\n%"PRIuFAST32" %"PRIuFAST32"\n%"PRIu8"\n",
		image->width, image->height, (uint8_t)UINT8_MAX);

	if (image->width != 0 && image->height != 0) {
		uint8_t *bytes = ImageToBytesArray(image);
		fwrite(bytes, 3, image->width*image->height, file);
		free(bytes);
	}

	end:
	if (file && fclose(file)) {
		FractalNow_close_errmsg(fileName);
		res = 1;
	}

	return res;
}

int aux_ExportPPM16(const char *fileName, const Image *image)
{
	FILE *file;
	int res = 0;

	file = fopen(fileName,"wb");
	if (!file) {
		FractalNow_open_werror(fileName);
	}

	fprintf(file,"P6\n%"PRIuFAST32" %"PRIuFAST32"\n%"PRIu16"\n",
		image->width, image->height, (uint16_t)UINT16_MAX);

	if (image->width != 0 && image->height != 0) {
		uint8_t *bytes = ImageToBytesArray(image);
		fwrite(bytes, 6, image->width*image->height, file);
		free(bytes);
	}

	end:
	if (file && fclose(file)) {
		FractalNow_close_errmsg(fileName);
		res = 1;
	}

	return res;

}

int ExportPPM(const char *fileName, const Image *image)
{
	int res = 0;

	FractalNow_message(stdout, T_NORMAL, "Exporting PPM \'%s\'...\n", fileName);

	switch (image->bytesPerComponent) {
	case 1:
		res = aux_ExportPPM8(fileName, image);
		break;
	case 2:
		res = aux_ExportPPM16(fileName, image);
		break;
	default:
		FractalNow_error("Invalid image bytes per component.\n");
		break;
	}

	FractalNow_message(stdout, T_NORMAL, "Exporting PPM \'%s\' : %s.\n", fileName,
				(res == 0) ? "DONE" : "FAILED");

	return res;
}

