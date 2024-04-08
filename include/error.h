/*
 *  error.h -- part of fractal2D
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
 
#ifndef __ERROR_H__
#define __ERROR_H__

#include <stdlib.h>
#include <stdio.h>

enum {T_QUIET = 0, T_NORMAL, T_VERBOSE};

int debug;
int traceLevel;

// added line (void)NULL to force the user to put a ";" after calling error
#define info(msg_trace,...) \
        if (msg_trace <= traceLevel) { \
            if (debug) { \
                printf("[%s: %s, l.%d] ", __FILE__, __func__, __LINE__); \
            } \
            printf(__VA_ARGS__); \
        }

#define error(...) \
        if (debug) { \
            printf("[%s: %s, l.%d] ", __FILE__, __func__, __LINE__); \
        } \
        printf(__VA_ARGS__); \
        exit(EXIT_FAILURE)

// common errors
#define invalid_use_error(...) \
        if (debug) { \
            printf("[%s: %s, l.%d] ", __FILE__, __func__, __LINE__); \
        } \
        printf("Invalid use : "); \
	printf(__VA_ARGS__); \
	DisplayHelp(); \
        exit(EXIT_FAILURE)

#define read_error(fileName) error("Error occured when reading file \'%s\'.\n", fileName)
#define write_error(fileName) error("Error occured when writing in file \'%s\'.\n", fileName)
#define open_error(fileName) error("Error occured when opening file \'%s\'.\n", fileName)
#define close_error(fileName) error("Error occured when closing file \'%s\'.\n", fileName)
#define alloc_error(s) error("Error occured when allocating memory for %s.\n", s)


#endif
