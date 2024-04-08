/*
 *  file_io.h -- part of FractalNow
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
  * \file file_io.h
  * \brief Header file related to file IO.
  * \author Marc Pegon
  */

#ifndef __FILE_IO_H__
#define __FILE_IO_H__

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
 * \fn int readFLOATT(FILE *file, FLOATT *dst)
 * \brief Read FLOATT from file.
 *
 * \param file File from which to read FLOATT.
 * \param dst Destination FLOATT.
 * \return Number of elements successfully read or EOF (like fscanf).
 */
int readFLOATT(FILE *file, FLOATT *dst);

/**
 * \fn int readColor(FILE *file, int_fast8_t bytesPerComponent, Color *dst)
 * \brief Read color from file as a hexadecimal number.
 *
 * Exit program with error if bytesPerComponent is not 1 or 2.
 *
 * \param file File from which to read color.
 * \param bytesPerComponent Bytes per component for color to read (either 1 for RGB8 or 2 for RGB16).
 * \param dst Destination color.
 * \return Number of elements successfully read or EOF (like fscanf).
 */
int readColor(FILE *file, int_fast8_t bytesPerComponent, Color *dst);

/**
 * \fn int writeString(FILE *file, const char *src, const char *suffix)
 * \brief Write string into file.
 *
 * \param file File to write into.
 * \param src Source string.
 * \param suffix Suffix to add after source string.
 * \return Number of characters written, negative in case of error (like fprintf).
 */
int writeString(FILE *file, const char *src, const char *suffix);

/**
 * \fn int writeUint32(FILE *file, uint32_t src, const char *suffix)
 * \brief Write uint32 into file.
 *
 * \param file File to write into.
 * \param src Source uint32.
 * \param suffix Suffix to add after uint32.
 * \return Number of characters written, negative in case of error (like fprintf).
 */
int writeUint32(FILE *file, uint32_t src, const char *suffix);

/**
 * \fn int writeFLOATT(FILE *file, FLOATT src, const char *suffix)
 * \brief Write FLOATT into file.
 *
 * \param file File to write into.
 * \param src Source FLOATT.
 * \param suffix Suffix to add after FLOATT.
 * \return Number of characters written, negative in case of error (like fprintf).
 */
int writeFLOATT(FILE *file, FLOATT src, const char *suffix);

/**
 * \fn int writeColor(FILE *file, Color src, const char *suffix)
 * \brief Write color into file as a hexadecimal number.
 *
 * Exit program with error if color bytesPerComponent is not 1 or 2.

 * \param file File to write into.
 * \param src Source color.
 * \param suffix Suffix to add after source color.
 * \return Number of characters written, negative in case of error (like fprintf).
 */
int writeColor(FILE *file, Color src, const char *suffix);

#ifdef __cplusplus
}
#endif

#endif
