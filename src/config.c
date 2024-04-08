/*
 *  config.c -- part of fractal2D
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
 
#include "config.h"
#include "error.h"
#include <stdio.h>

void ReadConfigFile(Config *config, char *fileName) {
	FILE *file;
	char tmp[8][256];
	uint32_t hex_color[256];

	file=fopen(fileName,"r");
	if (!file) {
		open_error(fileName);
	}

	for (uint32_t i=0; i<7; i++) {
		if ( fscanf(file,"%s",tmp[i]) == EOF ) {
			read_error(fileName);
		}
	}
	uint32_t spaceColor;
	if ( fscanf(file,"%x",&spaceColor) == EOF ) {
		read_error(fileName);
	}
	config->spaceColor = ColorFromUint32(spaceColor);
	if ( fscanf(file,"%s",tmp[7]) == EOF ) {
		read_error(fileName);
	}
	
	uint32_t i=0;
	while (fscanf(file,"%x",&hex_color[i]) != EOF) {
		i++;
	}

	config->nbColors = i;
	if (config->nbColors < 2) {
		error("Invalid config file : number of colors must be between 2 and 256.\n");
	}
	config->color = malloc(config->nbColors*sizeof(Color));
	if (config->color == NULL) {
		alloc_error("colors");
	}
	for (i=0; i<config->nbColors; i++) {
		config->color[i]=ColorFromUint32(hex_color[i]);
	}

	config->centerX = atof(tmp[0]);
	config->centerY = atof(tmp[1]);
	config->spanX = atof(tmp[2]);
	if (config->spanX <= 0) {
		error("Invalid config file : spanX must be > 0.\n");
	}
	config->spanY = atof(tmp[3]);
	if (config->spanY <= 0) {
		error("Invalid config file : spanY must be > 0.\n");
	}
	config->escapeRadius = atof(tmp[4]);
	if (config->escapeRadius <= 0) {
		error("Invalid config file : escape radius must be > 0.\n");
	}
	config->nbIterMax = atoi(tmp[5]);
	config->multiplier = atof(tmp[6]);
	config->multiplier *= config->multiplier;
	if (config->multiplier <= 0) {
		error("Invalid config file : multiplier must be > 0.\n");
	}
	config->nbTransitions = atoi(tmp[7]);
	if (config->nbTransitions < 2) {
		error("Invalid config file : number of transitions must be > 1.\n");
	}

	if (fclose(file)) {
		close_error(fileName);
	}
}

void FreeConfig(Config *config)
{
	free(config->color);
}
