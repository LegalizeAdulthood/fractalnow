/*
 *  float_table.c -- part of fractal2D
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

#include "float_table.h"
#include "error.h"
#include <stdlib.h>

void CreateUnitializedFloatTable(FloatTable *table, uint32_t width, uint32_t height)
{
	table->data = (FLOAT *)malloc(width*height*sizeof(FLOAT));
	if (table->data == NULL) {
		alloc_error("float table");
	}
	table->width = width;
	table->height = height;
}

inline FLOAT GetDataUnsafe(FloatTable *table, uint32_t x, uint32_t y) {
	return *(table->data+y*table->width+x);
}

inline void SetDataUnsafe(FloatTable *table, uint32_t x, uint32_t y, FLOAT data)
{
	*(table->data+y*table->width+x)=data;
}

void FLOAT2Bytes(uint8_t dest[10], FLOAT f)
{
	int exp;
	FLOAT significand = frexpF(f, &exp);

	uint16_t u_exp = (uint16_t)(exp + 32767);
	uint64_t mantissa = (significand+1.)*powF(2, 32);

	int i = 0;
	for (; i < 2; ++i, u_exp >>= 8) {
		dest[i] = (uint8_t)(u_exp&0xFF);
	}
	for (; i < 10; ++i, mantissa >>= 8) {
		dest[i] = (uint8_t)(mantissa&0xFF);
	}
}

void Bytes2FLOAT(FLOAT *dest, uint8_t bytes[10])
{
	uint16_t u_exp = 0;
	uint64_t mantissa = 0;

	int i = 9;
	for (; i >= 2; --i) {
		mantissa <<= 8;
		mantissa += bytes[i];
	}

	for (; i >= 0; --i) {
		u_exp <<= 8;
		u_exp += bytes[i];
	}

	int exp = (int32_t)(u_exp) - 32767;
	FLOAT significand = mantissa*powF(2, -32) - 1.;

	*dest = ldexpF(significand, exp);
}

void BytesToUint32(uint32_t *dest, uint8_t bytes[4])
{
	*dest = bytes[0] + (bytes[1]<<8) + (bytes[2]<<16) + (bytes[3] << 24);
}

void Uint32ToBytes(uint8_t bytes[4], uint32_t src)
{
	for (int i = 0; i < 4; ++i, src >>= 8) {
		bytes[i] = (uint8_t)(src & 0xFF);
	}
}

void ImportFloatTable(FloatTable *table, char *fileName)
{
	FILE *file;

	info(T_NORMAL, "Importing float table \'%s\'...\n", fileName);
	file = fopen(fileName,"rb");
	if (!file) {
		open_error(fileName);
	}

	uint8_t tmp[4];
	if (fread(tmp, 1, 4, file) < 4) {
		read_error(fileName);
	}
	BytesToUint32(&table->width, tmp);

	if (fread(tmp, 1, 4, file) < 4) {
		read_error(fileName);
	}
	BytesToUint32(&table->height, tmp);

	CreateUnitializedFloatTable(table, table->width, table->height);
	FLOAT *data = table->data;

	uint8_t bytes[10];
	for (uint32_t i=0; i<table->height; i++) {
		for (uint32_t j=0; j<table->width; j++) {
			if (fread(bytes, 1, 10, file) < 10) {
				read_error(fileName);
			}
			Bytes2FLOAT(data, bytes);

			data++;
		}
	}

	if (fclose(file)) {
		close_error(fileName);
	}

	info(T_NORMAL, "Importing float table \'%s\' : DONE.\n", fileName);
}

void ExportFloatTable(char *fileName, FloatTable *table)
{
	FILE *file;

	info(T_NORMAL, "Exporting float table \'%s\'...\n", fileName);
	file = fopen(fileName,"wb");
	if (!file) {
		open_error(fileName);
	}

	uint8_t tmp[4];
	Uint32ToBytes(tmp, table->width);
	if (fwrite(tmp, 1, 4, file) < 4) {
		write_error(fileName);
	} 

	Uint32ToBytes(tmp, table->height);
	if (fwrite(tmp, 1, 4, file) < 4) {
		write_error(fileName);
	} 

	FLOAT *data = table->data;
	uint8_t bytes[10];
	for (uint32_t i=0; i<table->height; i++) {
		for (uint32_t j=0; j<table->width; j++) {
			FLOAT2Bytes(bytes, *data);
			if (fwrite(bytes, 1, 10, file) < 10) {
				write_error(fileName);
			}
			
			data++;
		}
	}

	if (fclose(file)) {
		close_error(fileName);
	}

	info(T_NORMAL, "Exporting float table \'%s\' : DONE.\n", fileName);
}

void FreeFloatTable(FloatTable *table)
{
	free(table->data);
}

