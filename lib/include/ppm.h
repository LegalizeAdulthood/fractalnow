/*
 *  ppm.h -- part of fractal2D
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
 
 /**
  * \file ppm.h
  * \brief Header file related to PPM (Portable Pixmap) handling.
  * \author Marc Pegon
  */

#ifndef __PPM_H__
#define __PPM_H__

#ifdef __cplusplus
extern "C" {
#endif

#include "image.h"
#include  <stdint.h>

/**
 * \fn void ExportPPM(char *fileName, Image *image)
 * \brief Export image as PPM;
 *
 * \param fileName Name of the PPM file to export the image in.
 * \param image Pointer to image structure to export.
 */
void ExportPPM(char *fileName, Image *image);

#ifdef __cplusplus
}
#endif

#endif
