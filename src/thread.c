/*
 *  thread.c -- part of fractal2D
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
 
#include "thread.h"
#include "error.h"
#include "misc.h"
#include <stdlib.h>

uint_fast32_t nbThreads = DEFAULT_NB_THREADS;

void LaunchThreadsAndWait(uint_fast32_t N, void *args, size_t s_elem, void *(*routine)(void *))
{
        pthread_t *thread;
	thread = (pthread_t *)safeMalloc("threads", N * sizeof(pthread_t)); 

	uint8_t *c_arg = (uint8_t *)args;
	for (uint_fast32_t i = 0; i < N; ++i, c_arg+=s_elem) {
                safePThreadCreate(&(thread[i]),NULL,routine,c_arg);
	}

	void *status;
	for (uint_fast32_t i = 0; i < N; ++i) {
		safePThreadJoin(thread[i], &status);

		if (status != NULL) {
			error("Thread ended because of an error.\n");
		}
	}

	free(thread);
}
