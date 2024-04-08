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
 
 /**
  * \file error.h
  * \brief Header file containing error macros and stuff.
  * \author Marc Pegon
  */

#ifndef __ERROR_H__
#define __ERROR_H__

#ifdef __cplusplus
extern "C" {
#endif

#include <stdlib.h>
#include <stdio.h>

/**
 * \enum e_TraceLevel
 * \brief Trace level type.
 *
 * Users should understand the difference between application current
 * trace level, and messages trace levels.
 * Using fractal2D_message macro, a message is printed if and only
 * application current trace level is greater (>=) than message trace
 * level.
 *
 * For example :
 * - Messages with T_NORMAL trace level will be printed if application
 *   current trace level is T_NORMAL or T_VERBOSE.
 * - Messages with T_VERBOSE trace level will be printed only if
 *   application current trace level is T_VERBOSE.
 *
 * Note that error messages trace level is T_QUIET, i.e. are always
 * printed, except when application current trace level is
 * T_QUIET_ERROR.
 */
/**
 * \typedef TraceLevel
 * \brief Convenient typedef for enum TraceLevel.
 */
typedef enum e_TraceLevel {
	T_QUIET_ERROR = 0,
 /*<! Quiet errors trace level.*/
	T_QUIET,
 /*<! Quiet trace level.*/
	T_NORMAL,
 /*<! Normal trace level.*/
	T_VERBOSE
 /*<! Verbose trace level.*/
} TraceLevel;

/**
 * \var fractal2D_debug
 * \brief Specifies whether or not debug messages should be printed.
 *
 * 1 if debug messages should be printed (for errors), 0 otherwise.
 * This variable has an effect only if library was built in debug mode.
 *
 * \see debug_enabled()
 */
extern int fractal2D_debug;

/**
 * \var fractal2D_traceLevel
 * \brief Specifies application current trace level.
 */
extern TraceLevel fractal2D_traceLevel;

#ifdef DEBUG
#define fractal2D_debug_prefix(output) \
    if (fractal2D_debug) { \
	fprintf(output,"[%s: %s, l.%d] ", __FILE__, __func__, __LINE__); \
    }
#else
#define fractal2D_debug_prefix(output) (void)NULL;
#endif

/**
 * \def fractal2D_message(output, msg_trace,...)
 * \brief Print message depending on trace level.
 *
 * Message is printed if and only application current trace level
 * is greater (>=) than message trace level.
 */
#define fractal2D_message(output, msg_trace,...) \
        if (msg_trace <= fractal2D_traceLevel) { \
            fractal2D_debug_prefix(output); \
            fprintf(output, __VA_ARGS__); \
        }

#define fractal2D_errmsg(...) \
	fractal2D_message(stderr, T_QUIET, __VA_ARGS__);

#define fractal2D_error(...) \
	fractal2D_errmsg(__VA_ARGS__);\
        exit(EXIT_FAILURE)

#define fractal2D_werror(...) \
	fractal2D_message(stderr, T_QUIET, __VA_ARGS__);\
	res = 1; \
	goto end;

// Common error messages
#define alloc_error_msg(s) "Error occured when allocated memory for %s.\n", s
#define existence_error_msg(fileName) "Error: \'%s\' does not exist.\n", fileName
#define open_error_msg(fileName) "Error occured when opening file \'%s\'.\n", fileName
#define read_error_msg(fileName) "Error occured when reading file \'%s\'.\n", fileName
#define write_error_msg(fileName) "Error occured when writing in file \'%s\'.\n", fileName
#define close_error_msg(fileName) "Error occured when closing file \'%s\'.\n", fileName

#define fractal2D_alloc_errmsg(s) fractal2D_errmsg(alloc_error_msg(s))
#define fractal2D_existence_errmsg(fileName) fractal2D_errmsg(existence_error_msg(fileName))
#define fractal2D_open_errmsg(fileName) fractal2D_errmsg(open_error_msg(fileName))
#define fractal2D_read_errmsg(fileName) fractal2D_errmsg(read_error_msg(fileName))
#define fractal2D_write_errmsg(fileName) fractal2D_errmsg(write_error_msg(fileName))
#define fractal2D_close_errmsg(fileName) fractal2D_errmsg(close_error_msg(fileName))

// Common errors
#define fractal2D_alloc_error(s) fractal2D_error(alloc_error_msg(s))
#define fractal2D_existence_error(fileName) fractal2D_error(existence_error_msg(fileName))
#define fractal2D_open_error(fileName) fractal2D_error(open_error_msg(fileName))
#define fractal2D_read_error(fileName) fractal2D_error(read_error_msg(fileName))
#define fractal2D_write_error(fileName) fractal2D_error(write_error_msg(fileName))
#define fractal2D_close_error(fileName) fractal2D_error(close_error_msg(fileName))

// Weak error
#define fractal2D_alloc_werror(s) fractal2D_werror(alloc_error_msg(s))
#define fractal2D_existence_werror(fileName) fractal2D_werror(existence_error_msg(fileName))
#define fractal2D_open_werror(fileName) fractal2D_werror(open_error_msg(fileName))
#define fractal2D_read_werror(fileName) fractal2D_werror(read_error_msg(fileName))
#define fractal2D_write_werror(fileName) fractal2D_werror(write_error_msg(fileName))
#define fractal2D_close_werror(fileName) fractal2D_werror(close_error_msg(fileName))

#ifdef __cplusplus
}
#endif

#endif
