/*
 *  file_parsing.h -- part of fractal2D
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
 
 /**
  * \file file_parsing.h
  * \brief Header file related to file parsing.
  * \author Marc Pegon
  */

#ifndef __FILE_PARSING_H__
#define __FILE_PARSING_H__

#ifdef __cplusplus
extern "C" {
#endif

#include "color.h"
#include <stdint.h>
#include <stdio.h>

/**
 * \fn int readString(FILE *file, char *dst)
 * \brief Read string from file.
 *
 * \param file File from which to read string.
 * \param dst Destination string.
 * \return Number of elements successfully read or EOF (like fscanf).
 */
int readString(FILE *file, char *dst);

/**
 * \fn int readUint32(FILE *file, uint32_t *dst)
 * \brief Read uint32 from file.
 *
 * \param file File from which to read uint32.
 * \param dst Destination uint32.
 * \return Number of elements successfully read or EOF (like fscanf).
 */
int readUint32(FILE *file, uint32_t *dst);

/**
 * \fn int readFLOAT(FILE *file, FLOAT *dst)
 * \brief Read FLOAT from file.
 *
 * \param file File from which to read FLOAT.
 * \param dst Destination FLOAT.
 * \return Number of elements successfully read or EOF (like fscanf).
 */
int readFLOAT(FILE *file, FLOAT *dst);

/**
 * \fn int readColor(FILE *file, int_fast8_t bytesPerComponent, Color *dst)
 * \brief Read color from file as a hexadecimal number.
 *
 * \param file File from which to read color.
 * \param bytesPerComponent Bytes per component for color to read (either 1 for RGB8 or 2 for RGB16).
 * \param dst Destination color.
 * \return Number of elements successfully read or EOF (like fscanf).
 */
int readColor(FILE *file, int_fast8_t bytesPerComponent, Color *dst);

/**
 * \fn void safeReadString(FILE *file, const char *fileName, char *dst)
 * \brief Safely read string from file.
 *
 * Exit with error in case of read failure.
 *
 * \param file File from which to read string.
 * \param fileName File name to print in case of failure.
 * \param dst Destination string.
 */
void safeReadString(FILE *file, const char *fileName, char *dst);

/**
 * \fn uint32_t safeReadUint32(FILE *file, const char *fileName)
 * \brief Safely read uint32 from file.
 *
 * Exit with error in case of read failure.
 *
 * \param file File from which to read uint32.
 * \param fileName File name to print in case of failure.
 * \return uint32 read.
 */
uint32_t safeReadUint32(FILE *file, const char *fileName);

/**
 * \fn FLOAT safeReadFLOAT(FILE *file, const char *fileName)
 * \brief Safely read FLOAT from file.
 *
 * Exit with error in case of read failure.
 *
 * \param file File from which to read FLOAT.
 * \param fileName File name to print in case of failure.
 * \return FLOAT read.
 */
FLOAT safeReadFLOAT(FILE *file, const char *fileName);

/**
 * \fn Color safeReadColor(FILE *file, const char *fileName, int_fast8_t bytesPerComponent)
 * \brief Safely read color from file as a hexadecimal number.
 *
 * Exit with error in case of read failure.
 *
 * \param file File from which to read color.
 * \param fileName File name to print in case of failure.
 * \param bytesPerComponent Bytes per component for color to read (either 1 for RGB8 or 2 for RGB16).
 * \return Color read.
 */
Color safeReadColor(FILE *file, const char *fileName, int_fast8_t bytesPerComponent);

#ifdef __cplusplus
}
#endif

#endif
