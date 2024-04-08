/*
 *  error.h -- part of FractalNow
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

#include <stdlib.h>
#include <stdio.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * \enum e_TraceLevel
 * \brief Trace level type.
 *
 * Users should understand the difference between application current
 * trace level, and messages trace levels.\n
 * Using FractalNow_message macro, a message is printed if and only
 * application current trace level is greater (>=) than message trace
 * level.
 *
 * For example :
 * - Messages with T_NORMAL trace level will be printed if application
 *   current trace level is T_NORMAL or T_VERBOSE.
 * - Messages with T_VERBOSE trace level will be printed only if
 *   application current trace level is T_VERBOSE.
 *
 * Note that error messages' trace level is T_QUIET, i.e. are always
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
 * \var FractalNow_debug
 * \brief Specifies whether or not debug messages should be printed.
 *
 * 1 if debug messages should be printed (for errors), 0 otherwise.\n
 * This variable has an effect only if library was built in debug mode.
 *
 * \see debug_enabled()
 */
extern int FractalNow_debug;

/**
 * \var FractalNow_traceLevel
 * \brief Specifies application current trace level.
 */
extern TraceLevel FractalNow_traceLevel;

#ifdef DEBUG
#define FractalNow_debug_prefix(output) \
    if (FractalNow_debug) { \
	fprintf(output,"[%s: %s, l.%d] ", __FILE__, __func__, __LINE__); \
    }
#else
#define FractalNow_debug_prefix(output) (void)NULL;
#endif

/**
 * \def FractalNow_message(output, msg_trace,...)
 * \brief Print message depending on trace level.
 *
 * Message is printed if and only application current trace level
 * is greater (>=) than message trace level.
 */
#define FractalNow_message(output, msg_trace,...) \
        if (msg_trace <= FractalNow_traceLevel) { \
            FractalNow_debug_prefix(output); \
            fprintf(output, __VA_ARGS__); \
        }

#define FractalNow_errmsg(...) \
	FractalNow_message(stderr, T_QUIET, __VA_ARGS__);

#define FractalNow_error(...) \
	FractalNow_errmsg(__VA_ARGS__);\
        exit(EXIT_FAILURE)

#define FractalNow_werror(...) \
	FractalNow_message(stderr, T_QUIET, __VA_ARGS__);\
	res = 1; \
	goto end;

// Common error messages
#define alloc_error_msg(s) "Error occured when allocated memory for %s.\n", s
#define existence_error_msg(fileName) "Error: \'%s\' does not exist.\n", fileName
#define open_error_msg(fileName) "Error occured when opening file \'%s\'.\n", fileName
#define read_error_msg(fileName) "Error occured when reading file \'%s\'.\n", fileName
#define write_error_msg(fileName) "Error occured when writing in file \'%s\'.\n", fileName
#define close_error_msg(fileName) "Error occured when closing file \'%s\'.\n", fileName

#define FractalNow_alloc_errmsg(s) FractalNow_errmsg(alloc_error_msg(s))
#define FractalNow_existence_errmsg(fileName) FractalNow_errmsg(existence_error_msg(fileName))
#define FractalNow_open_errmsg(fileName) FractalNow_errmsg(open_error_msg(fileName))
#define FractalNow_read_errmsg(fileName) FractalNow_errmsg(read_error_msg(fileName))
#define FractalNow_write_errmsg(fileName) FractalNow_errmsg(write_error_msg(fileName))
#define FractalNow_close_errmsg(fileName) FractalNow_errmsg(close_error_msg(fileName))

// Common errors
#define FractalNow_alloc_error(s) FractalNow_error(alloc_error_msg(s))
#define FractalNow_existence_error(fileName) FractalNow_error(existence_error_msg(fileName))
#define FractalNow_open_error(fileName) FractalNow_error(open_error_msg(fileName))
#define FractalNow_read_error(fileName) FractalNow_error(read_error_msg(fileName))
#define FractalNow_write_error(fileName) FractalNow_error(write_error_msg(fileName))
#define FractalNow_close_error(fileName) FractalNow_error(close_error_msg(fileName))

// Weak error
#define FractalNow_alloc_werror(s) FractalNow_werror(alloc_error_msg(s))
#define FractalNow_existence_werror(fileName) FractalNow_werror(existence_error_msg(fileName))
#define FractalNow_open_werror(fileName) FractalNow_werror(open_error_msg(fileName))
#define FractalNow_read_werror(fileName) FractalNow_werror(read_error_msg(fileName))
#define FractalNow_write_werror(fileName) FractalNow_werror(write_error_msg(fileName))
#define FractalNow_close_werror(fileName) FractalNow_werror(close_error_msg(fileName))

#ifdef __cplusplus
}
#endif

#endif
