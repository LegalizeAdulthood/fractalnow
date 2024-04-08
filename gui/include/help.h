/*
 *  help.h -- part of fractal2D
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
 
 /**
  * \file help.h
  * \brief Header file introducing Help class.
  *
  * \author Marc Pegon
  */

#ifndef __HELP_H__
#define __HELP_H__

#include <cstdio>

 //! To print help on the command line.
class Help
{
	public:
	 //! Print program help.
	static void Print();
};

#define invalid_use_error(...) \
        if (debug) { \
            printf("[%s: %s, l.%d] ", __FILE__, __func__, __LINE__); \
        } \
        printf("Invalid use : "); \
	printf(__VA_ARGS__); \
	Help::Print(); \
        exit(EXIT_FAILURE)

#endif
