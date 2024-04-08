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
#include <string.h>

uint_fast32_t nbThreads = DEFAULT_NB_THREADS;

void freeArgDoNothing(void *arg)
{
	(void)arg;
}

inline Action LaunchThreads(uint_fast32_t N, void *args, size_t s_elem, void *(*routine)(void *),
				void (*freeArg)(void *))
{
	Action res;
        pthread_t *thread;

	res.cancel = (sig_atomic_t *)safeMalloc("cancel", sizeof(sig_atomic_t));
	*(res.cancel) = 0;
	thread = (pthread_t *)safeMalloc("threads", N * sizeof(pthread_t)); 
	res.args = safeMalloc("thread args", N * (s_elem + sizeof(sig_atomic_t *)));

	uint8_t *res_args = (uint8_t *)res.args;
	uint8_t *c_args = (uint8_t *)args;
	for (uint_fast32_t i = 0; i < N; ++i) {
		*((sig_atomic_t **)res_args) = res.cancel;
		memcpy(res_args+sizeof(sig_atomic_t *), c_args, s_elem);

                safePThreadCreate(&(thread[i]),NULL,routine,res_args);

		c_args += s_elem;
		res_args += s_elem+sizeof(sig_atomic_t *);
	}

	res.N = N;
	res.thread = thread;
	if (freeArg == NULL) {
		res.freeArg = freeArgDoNothing;
	} else {
		res.freeArg = freeArg;
	}
	res.s_elem = s_elem;
	
	return res;
}

void CancelAction(Action action)
{
	*(action.cancel) = 1;
}

inline int WaitForActionTermination(Action action)
{
	void *status;
	int res = 0;
	uint8_t *c_arg = (uint8_t *)(action.args) + sizeof(sig_atomic_t *);
	for (uint_fast32_t i = 0; i < action.N; ++i) {
		safePThreadJoin(action.thread[i], &status);

		if (status != NULL && status != PTHREAD_CANCELED) {
			error("Thread ended because of an error.\n");
		}
		res |= (status == PTHREAD_CANCELED);
		action.freeArg(c_arg);
		c_arg += action.s_elem + sizeof(sig_atomic_t *);
	}

	free(action.thread);
	free(action.args);
	free(action.cancel);

	return res;
}

int LaunchThreadsAndWaitForTermination(uint_fast32_t N, void *args, size_t s_elem,
					void *(*routine)(void *), void (*freeArg)(void *))
{
	Action action = LaunchThreads(N, args, s_elem, routine, freeArg);
	return WaitForActionTermination(action);
}

int CancelActionAndWaitForTermination(Action action)
{
	CancelAction(action);
	return WaitForActionTermination(action);
}

