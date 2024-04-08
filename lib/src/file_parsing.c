/*
 *  file_parsing.c -- part of fractal2D
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
 
#include "file_parsing.h"
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

int readFLOAT(FILE *file, FLOAT *dst)
{
	int res;

	res = fscanf(file,"%"SCNFLOAT,dst);

	return res;
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
		error("Invalid bytes per component.\n");
		break;
	}

	return res;
}

inline void safeReadString(FILE *file, const char *fileName, char *dst)
{
	if (readString(file, dst) < 1) {
		read_error(fileName);
	}
}

inline uint32_t safeReadUint32(FILE *file, const char *fileName)
{
	uint32_t res;

	if (readUint32(file, &res) < 1) {
		read_error(fileName);
	}
	return res;
}

inline FLOAT safeReadFLOAT(FILE *file, const char *fileName)
{
	FLOAT res;

	if (readFLOAT(file, &res) < 1) {
		read_error(fileName);
	}
	return res;
}

inline Color safeReadColor(FILE *file, const char *fileName, int_fast8_t bytesPerComponent)
{
	Color res;

	if (readColor(file, bytesPerComponent, &res) < 1) {
		read_error(fileName);
	}
	return res;
}

