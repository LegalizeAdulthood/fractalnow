/*
 *  rendering_parameters.c -- part of fractal2D
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

#include "rendering_parameters.h"
#include "error.h"

void aux_ReadRenderingFile8(RenderingParameters *param, char *fileName, FILE *file)
{
	unsigned long spaceColor;
	if (fscanf(file,"%lx",&spaceColor) == EOF) {
		read_error(fileName);
	}
	param->spaceColor = ColorFromUint32((uint32_t)spaceColor);

	char tmp[256];
	if ( fscanf(file,"%s",tmp) == EOF ) {
		read_error(fileName);
	}
	param->multiplier = strtoF(tmp, NULL);

	if ( fscanf(file,"%s",tmp) == EOF ) {
		read_error(fileName);
	}
	uint32_t nbTransitions = (uint32_t)strtoll(tmp, NULL, 10);
	if (nbTransitions < 2) {
		error("Invalid rendering file : number of transitions must be > 1.\n");
	}

	unsigned long hex_color[256];
	uint32_t nbColors=0;
	while (fscanf(file,"%lx",&hex_color[nbColors]) != EOF) {
		nbColors++;
	}
	if (nbColors < 2) {
		error("Invalid rendering file : number of colors must be between 2 and 256.\n");
	}

	Color *color = (Color *)malloc(nbColors*sizeof(Color));
	if (color == NULL) {
		alloc_error("colors");
	}
	for (uint32_t i=0; i<nbColors; i++) {
		color[i]=ColorFromUint32((uint32_t)hex_color[i]);
	}

	GenerateGradient(&param->gradient, color, nbColors, nbTransitions);

	free(color);
}

void aux_ReadRenderingFile16(RenderingParameters *param, char *fileName, FILE *file)
{
	unsigned long long spaceColor;
	if (fscanf(file,"%llx",&spaceColor) == EOF) {
		read_error(fileName);
	}
	param->spaceColor = ColorFromUint64((uint64_t)spaceColor);

	char tmp[256];
	if ( fscanf(file,"%s",tmp) == EOF ) {
		read_error(fileName);
	}
	param->multiplier = strtoF(tmp, NULL);

	if ( fscanf(file,"%s",tmp) == EOF ) {
		read_error(fileName);
	}
	uint32_t nbTransitions = (uint32_t)strtoll(tmp, NULL, 10);
	if (nbTransitions < 2) {
		error("Invalid rendering file : number of transitions must be > 1.\n");
	}

	unsigned long long hex_color[256];
	uint32_t nbColors=0;
	while (fscanf(file,"%llx",&hex_color[nbColors]) != EOF) {
		nbColors++;
	}
	if (nbColors < 2) {
		error("Invalid rendering file : number of colors must be between 2 and 256.\n");
	}

	Color *color = (Color *)malloc(nbColors*sizeof(Color));
	if (color == NULL) {
		alloc_error("colors");
	}
	for (uint32_t i=0; i<nbColors; i++) {
		color[i]=ColorFromUint64((uint64_t)hex_color[i]);
	}

	GenerateGradient(&param->gradient, color, nbColors, nbTransitions);

	free(color);

}

void ReadRenderingFile(RenderingParameters *param, char *fileName)
{
	info(T_NORMAL, "Reading rendering file...\n");

	FILE *file;

	file=fopen(fileName,"r");
	if (!file) {
		open_error(fileName);
	}

	if ( fscanf(file,"%d",&param->bytesPerComponent) == EOF ) {
		read_error(fileName);
	}

	switch (param->bytesPerComponent) {
	case 1:
		aux_ReadRenderingFile8(param, fileName, file);
		break;
	case 2:
		aux_ReadRenderingFile16(param, fileName, file);
		break;
	default:
		error("Invalid rendering file ! bytes per components must be 1 or 2.\n");
		break;
	}

	if (fclose(file)) {
		close_error(fileName);
	}

	info(T_NORMAL, "Reading rendering file : DONE\n");
}

void FreeRenderingParameters(RenderingParameters *param)
{
	FreeGradient(&param->gradient);
}
