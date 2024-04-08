/*
 *  misc.c -- part of fractal2D
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

#include "misc.h"
#include "error.h"
#include <ctype.h>

void toLowerCase(char *str)
{
	for (; *str != '\0'; ++str) {
		*str = tolower(*str);
	}
}

void *safeMalloc(const char *name, uint_least64_t size)
{
	if (size > SIZE_MAX) {
		fractal2D_alloc_error(name);
	}
	void *res = malloc((size_t)size);
	if (res == NULL) {
		fractal2D_alloc_error(name);
	}

	return res;
}

void *safeCalloc(const char *name, uint_least64_t nmemb, uint_least64_t size)
{
	if (nmemb > SIZE_MAX || size > SIZE_MAX) {
		fractal2D_alloc_error(name);
	}
	void *res = calloc((size_t)nmemb, (size_t)size);
	if (res == NULL) {
		fractal2D_alloc_error(name);
	}

	return res;
}

int isInteger(FLOAT x) {
	FLOAT fptr, iptr;
	fptr = modfF(x, &iptr);

	return (fptr == 0);
}

