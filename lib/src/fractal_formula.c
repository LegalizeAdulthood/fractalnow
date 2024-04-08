/*
 *  fractal_formula.c -- part of FractalNow
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

const char *fractalFormulaStr[] = {
	(const char *)"mandelbrot",
	(const char *)"mandelbrotp",
	(const char *)"julia",
	(const char *)"juliap",
	(const char *)"burningship",
	(const char *)"juliaburningship",
	(const char *)"mandelbar",
	(const char *)"juliabar",
	(const char *)"rudy"
};

const char *fractalFormulaDescStr[] = {
	(const char *)"Mandelbrot (z^2+c)",
	(const char *)"Multibrot (z^p+c))",
	(const char *)"Julia (z^2+c)",
	(const char *)"Multi Julia (z^p+c)",
	(const char *)"Burning ship ((|Re(z)|+|Im(z)|)^p+c)",
	(const char *)"Julia B.ship ((|Re(z)|+|Im(z)|)^p+c)",
	(const char *)"Mandelbar (conjugate(z)^p+c)",
	(const char *)"Juliabar (conjugate(z)^p+c)",
	(const char *)"Rudy (z^p + c*z + d)"
};

const uint_fast32_t nbFractalFormulas = sizeof(fractalFormulaStr) / sizeof(const char *);

int GetFractalFormula(FractalFormula *fractalFormula, const char *str)
{
	int res = 0;
	size_t len = strlen(str);

	if (len > 255) {
		FractalNow_werror("Unknown fractal formula \'%s\'.\n", str);
	}

	char FFStr[256];
	strcpy(FFStr, str);
	toLowerCase(FFStr);

	uint_fast32_t i;
	for (i = 0; i < nbFractalFormulas; ++i) {
		if (strcmp(FFStr, fractalFormulaStr[i]) == 0) {
			*fractalFormula = (FractalFormula)i;
			break;
		}
	}

	if (i == nbFractalFormulas) {
		FractalNow_werror("Unknown fractal formula \'%s\'.\n", str);
	}

	end:
	return res;
}

