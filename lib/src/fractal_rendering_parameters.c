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

inline void InitRenderingParameters(RenderingParameters *param, uint_fast8_t bytesPerComponent, Color spaceColor,
				CountingFunction countingFunction, ColoringMethod coloringMethod,
				AddendFunction addendFunction, uint_fast32_t stripeDensity,
				InterpolationMethod interpolationMethod, TransferFunction transferFunction,
				FLOAT multiplier, FLOAT offset, Gradient gradient)
{
	param->bytesPerComponent = bytesPerComponent;
	param->spaceColor = spaceColor;
	param->countingFunction = countingFunction;
	param->coloringMethod = coloringMethod;
	param->addendFunction = addendFunction;
	param->stripeDensity = stripeDensity;
	param->interpolationMethod = interpolationMethod;
	param->transferFunction = transferFunction;
	param->multiplier = multiplier;
	param->offset = offset;
	param->gradient = gradient;
}

void ReadRenderingFile(RenderingParameters *param, const char *fileName)
{
	info(T_NORMAL, "Reading rendering file...\n");

	FILE *file;

	file=fopen(fileName,"r");
	if (!file) {
		open_error(fileName);
	}
	
	uint_fast8_t bytesPerComponent;
	Color spaceColor;
	CountingFunction countingFunction;
	ColoringMethod coloringMethod;
	AddendFunction addendFunction;
	uint_fast32_t stripeDensity;
	InterpolationMethod interpolationMethod;
	TransferFunction transferFunction;
	FLOAT multiplier;
	FLOAT offset;
	Gradient gradient;

	uint_fast32_t bytesPerComponent32 = safeReadUint32(file, fileName);
	if (bytesPerComponent32 == 1 || bytesPerComponent32 == 2) {
		bytesPerComponent = (uint_fast8_t)bytesPerComponent32;
	} else {
		error("Invalid rendering file ! bytes per components must be 1 or 2.\n");
	}

	spaceColor = safeReadColor(file, fileName, bytesPerComponent);

	char str[256];
	safeReadString(file, fileName, str);
	countingFunction = GetCountingFunction(str);

	safeReadString(file, fileName, str);
	coloringMethod = GetColoringMethod(str);

	/* Just to be sure these are initialized, because they are always needed (and thus read). */
	interpolationMethod = IM_NONE;
	addendFunction = AF_TRIANGLEINEQUALITY;
	stripeDensity = 0;

	if (coloringMethod == CM_AVERAGE) {
		safeReadString(file, fileName, str);
		addendFunction = GetAddendFunction(str);
		if (addendFunction == AF_STRIPE) {
			stripeDensity = safeReadUint32(file, fileName);
		}
		safeReadString(file, fileName, str);
		interpolationMethod = GetInterpolationMethod(str);
	}

	safeReadString(file, fileName, str);
	transferFunction = GetTransferFunction(str);

	multiplier = safeReadFLOAT(file, fileName);
	offset = safeReadFLOAT(file, fileName);

	uint_fast32_t nbTransitions = safeReadUint32(file, fileName);
	if (nbTransitions < 2) {
		error("Invalid rendering file : number of transitions must be > 1.\n");
	}

	Color color[256];
	uint_fast32_t nbColors=0;
	int res;
	while ((res = readColor(file, bytesPerComponent, &color[nbColors])) != EOF) {
		if (res < 1) {
			read_error(fileName);
		}
		nbColors++;
	}

	if (nbColors < 2) {
		error("Invalid rendering file : number of colors must be between 2 and 256.\n");
	}

	GenerateGradient(&gradient, color, nbColors, nbTransitions);

	InitRenderingParameters(param, bytesPerComponent, spaceColor, countingFunction, coloringMethod,
				addendFunction, stripeDensity, interpolationMethod, transferFunction,
				multiplier, offset, gradient);

	if (fclose(file)) {
		close_error(fileName);
	}

	info(T_NORMAL, "Reading rendering file : DONE\n");
}

void FreeRenderingParameters(RenderingParameters *param)
{
	FreeGradient(&param->gradient);
}
