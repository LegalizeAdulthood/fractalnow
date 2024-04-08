/*
 *  float_table.h -- part of fractal2D
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
  * \file float_table.h
  * \brief Header file related to float table.
  * \author Marc Pegon
  */
 
#ifndef __FLOAT_TABLE_H__
#define __FLOAT_TABLE_H__

#include "floating_point.h"
#include <stdint.h>

/**
 * \struct s_FloatTable
 * \brief A table (2D array) of floating point numbers.
 */
typedef struct s_FloatTable {
	uint32_t width;
 /*!< Table width.*/
	uint32_t height;
 /*!< Table height.*/
	FLOAT *data;
 /*!< Table data.*/
} FloatTable;

/**
 * \fn void CreateUnitializedFloatTable(FloatTable *table, uint32_t width, uint32_t height)
 * \brief Create an unitialized table of floats.
 *
 * \param table Pointer to the (already allocated) FloatTable structure to create.
 * \param width Table width.
 * \param height Table height.
 */
void CreateUnitializedFloatTable(FloatTable *table, uint32_t width, uint32_t height);

/**
 * \fn FLOAT GetDataUnsafe(FloatTable *table, uint32_t x, uint32_t y)
 * \brief Get some particular data in float table.
 *
 * Get data at position (x,y) in float table.
 * This function is unsafe : it does not check whether or not
 * (x,y) is within range.
 *
 * \param table Table to get data from.
 * \param x X coordinate of data to get.
 * \param y Y coordinate of data to get.
 * \return Data at position (x,y) of float table.
 */
FLOAT GetDataUnsafe(FloatTable *table, uint32_t x, uint32_t y);

/**
 * \fn void SetDataUnsafe(FloatTable *table, uint32_t x, uint32_t y, FLOAT data)
 * \brief Set data in float table.
 *
 * Set data at position (x,y) in float table.
 * This function is unsafe : it does not check whether or not
 * (x,y) is within range.
 *
 * \param table Table to set data in.
 * \param x X coordinate of data to set.
 * \param y Y coordinate of data to set.
 * \param data Data to set in float table.
 */
void SetDataUnsafe(FloatTable *table, uint32_t x, uint32_t y, FLOAT data);

/**
 * \fn void ImportFloatTable(FloatTable *table, char *fileName)
 * \brief Import float table from file.
 *
 * \param table Float table to be initialized.
 * \param fileName Name of the file to read the float table from.
 */
void ImportFloatTable(FloatTable *table, char *fileName);

/**
 * \fn void ExportFloatTable(char *fileName, FloatTable *table)
 * \brief Export float table in file.
 *
 * Export float table as an array of floating numbers,
 * separated with spaces and preceded by its width and height.
 *
 * \param fileName Name of the file to write the float table in.
 * \param table Float table to be written.
 */
void ExportFloatTable(char *fileName, FloatTable *table);

/**
 * \fn void FreeFloatTable(FloatTable *table)
 * \brief Free float table.
 *
 * \param table Pointer to the float table structure to free.
 */
void FreeFloatTable(FloatTable *table);

#endif
