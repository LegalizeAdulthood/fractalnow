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
#include <float.h>
#include <inttypes.h>

#ifdef _ENABLE_MP_FLOATS
#include <stdio.h>
#include <mpfr.h>
#endif

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

int readDouble(FILE *file, double *dst)
{
	return fscanf(file,"%lf",dst);
}

#ifdef _ENABLE_MP_FLOATS
static inline int readMPFR(FILE *file, mpfr_t dst)
{
	return (mpfr_inp_str(dst, file, 10, MPFR_RNDN) == 0) ? -1 : 1;
}
#endif

int readBiggestFloat(FILE *file, BiggestFloat *dst)
{
#ifdef _ENABLE_MP_FLOATS
	return readMPFR(file, *dst);
#else // #ifndef _ENABLE_MP_FLOATS
	double tmp;
	int res = readDouble(file, &tmp);
	
	if (res > 0) {
		fromDoubleBiggestF(*dst, tmp);
	}

	return res;
#endif
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

int writeDouble(FILE *file, const double src, const char *suffix)
{
	/* We use %G instead of %lG because:
	 * -both are acceptable for printing doubles
	 *  (floats are actually promoted to doubles)
	 * -some implementations of printf do not accept
	 *  %lG (like Windows')
	 */
	return fprintf(file, "%.*G%s", DBL_DIG, src, suffix);
}

#ifdef _ENABLE_MP_FLOATS
static inline int writeMPFR(FILE *file, const mpfr_t src, const char *suffix)
{
	return mpfr_fprintf(file, "%RE%s", src, suffix);
}
#endif

int writeBiggestFloat(FILE *file, const BiggestFloat src, const char *suffix)
{
#ifdef _ENABLE_MP_FLOATS
	return writeMPFR(file, src, suffix);
#else // #ifndef _ENABLE_MP_FLOATS
	return writeDouble(file, src, suffix);
#endif
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

