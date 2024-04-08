/*
 *  file_io.c -- part of FractalNow
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
 
#include "file_io.h"
#include "error.h"
#include <inttypes.h>

int readString(FILE *file, char *dst)
{
	return fscanf(file, "%s", dst);
}

int readUint32(FILE *file, uint32_t *dst)
{
	int res;
	int64_t read;

	if ((res = fscanf(file,"%"SCNd64,&read)) < 1) {
		return res;
	}
	if (read < 0) {
		return 0;
	}

	*dst = (uint32_t)read;

	return res;
}

int readFLOATT(FILE *file, FLOATT *dst)
{
	return fscanf(file,"%"SCNFLOATT,dst);
}

int readColor(FILE *file, int_fast8_t bytesPerComponent, Color *dst)
{
	uint32_t color32;
	uint64_t color64;
	int res;

	switch (bytesPerComponent) {
	case 1:
		if ((res = fscanf(file,"%"SCNx32,&color32)) < 1) {
			break;
		}
		*dst = ColorFromUint32(color32);
		break;
	case 2:
		if ((res = fscanf(file,"%"SCNx64,&color64)) < 1) {
			break;
		}
		*dst = ColorFromUint64((uint64_t)color64);
		break;
	default:
		FractalNow_error("Invalid bytes per component.\n");
		break;
	}

	return res;
}

int writeString(FILE *file, const char *src, const char *suffix)
{
	return fprintf(file, "%s%s", src, suffix);
}

int writeUint32(FILE *file, uint32_t src, const char *suffix)
{
	return fprintf(file, "%"PRIu32"%s", src, suffix);
}

int writeFLOATT(FILE *file, FLOATT src, const char *suffix)
{
	return fprintf(file, "%"PRIFLOATT"%s", FLOATT_DIG, src, suffix);
}

int writeColor(FILE *file, Color src, const char *suffix)
{
	uint32_t color32;
	uint64_t color64;
	int res;

	switch (src.bytesPerComponent) {
	case 1:
		color32 = ARGB8_TO_UINT32(0, src.r, src.g, src.b);

		res = fprintf(file, "0x%"PRIx32"%s", color32, suffix);
		break;
	case 2:
		color64 = ARGB16_TO_UINT64(0, src.r, src.g, src.b);

		res = fprintf(file, "0x%"PRIx64"%s", color64, suffix);
		break;
	default:
		FractalNow_error("Invalid bytes per component.\n");
		break;
	}

	return res;
}

