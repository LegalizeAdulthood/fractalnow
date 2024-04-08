/*
 *  fractal_formula.c -- part of fractal2D
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
 
#include "fractal_formula.h"
#include "error.h"
#include "misc.h"
#include <string.h>

const char *FractalFormulaStr[] = {
	(char *)"mandelbrot",
	(char *)"mandelbrotp",
	(char *)"julia",
	(char *)"juliap",
	(char *)"rudy"
};

const char *FractalFormulaDescStr[] = {
	(char *)"Mandelbrot (z^2+c)",
	(char *)"Mandelbrot P (z^p+c))",
	(char *)"Julia (z^2+c)",
	(char *)"Julia P (z^p+c)",
	(char *)"Rudy (z^p + c*z + d)"
};

uint_fast32_t nbFractalFormulas = sizeof(FractalFormulaStr) / sizeof(char *);

FractalFormula GetFractalFormula(const char *str)
{
	size_t len = strlen(str);

	if (len > 255) {
		error("Unknown fractal formula \'%s\'.\n", str);
	}

	FractalFormula res;
	char FFStr[256];
	strcpy(FFStr, str);
	toLowerCase(FFStr);

	uint_fast32_t i;
	for (i = 0; i < nbFractalFormulas; ++i) {
		if (strcmp(FFStr, FractalFormulaStr[i]) == 0) {
			res = (FractalFormula)i;
			break;
		}
	}

	if (i == nbFractalFormulas) {
		error("Unknown fractal formula \'%s\'.\n", str);
	}

	return res;
}

