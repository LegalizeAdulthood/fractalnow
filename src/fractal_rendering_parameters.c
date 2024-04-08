/*
 *  fractal_rendering_parameters.c -- part of fractal2D
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

#include "fractal_rendering_parameters.h"
#include "error.h"
#include "file_parsing.h"
#include "misc.h"

void ReadRenderingFile(RenderingParameters *param, char *fileName)
{
	info(T_NORMAL, "Reading rendering file...\n");

	FILE *file;

	file=fopen(fileName,"r");
	if (!file) {
		open_error(fileName);
	}

	uint_fast32_t bytesPerComponent = safeReadUint32(file, fileName);
	if (bytesPerComponent == 1 || bytesPerComponent == 2) {
		param->bytesPerComponent = (uint_fast8_t)bytesPerComponent;
	} else {
		error("Invalid rendering file ! bytes per components must be 1 or 2.\n");
	}

	param->spaceColor = safeReadColor(file, fileName, param->bytesPerComponent);

	char str[256];
	safeReadString(file, fileName, str);
	param->iterationCountFunction = GetIterationCountFunction(str);

	safeReadString(file, fileName, str);
	param->coloringMethod = GetColoringMethod(str);

	if (param->coloringMethod == CM_AVERAGE) {
		safeReadString(file, fileName, str);
		param->addendFunction = GetAddendFunction(str);
		safeReadString(file, fileName, str);
		param->interpolationFunction = GetInterpolationFunction(str);
	} else {
		param->interpolationFunction = NULL;
		param->addendFunction = NULL;
	}

	safeReadString(file, fileName, str);
	param->transferFunction = GetTransferFunction(str);

	param->multiplier = safeReadFLOAT(file, fileName);

	uint_fast32_t nbTransitions = safeReadUint32(file, fileName);
	if (nbTransitions < 2) {
		error("Invalid rendering file : number of transitions must be > 1.\n");
	}

	Color color[256];
	uint_fast32_t nbColors=0;
	int res;
	while ((res = readColor(file, param->bytesPerComponent, &color[nbColors])) != EOF) {
		if (res < 1) {
			read_error(fileName);
		}
		nbColors++;
	}

	if (nbColors < 2) {
		error("Invalid rendering file : number of colors must be between 2 and 256.\n");
	}

	GenerateGradient(&param->gradient, color, nbColors, nbTransitions);
	if (fclose(file)) {
		close_error(fileName);
	}

	info(T_NORMAL, "Reading rendering file : DONE\n");
}

void FreeRenderingParameters(RenderingParameters *param)
{
	FreeGradient(&param->gradient);
}
