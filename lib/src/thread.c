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

Action *DoNothingAction()
{
	Action *res = (Action *)safeMalloc("do-nothing action", sizeof(Action));
	res->is_do_nothing = 1;
	res->done = 1;
	res->cancel = 0;

	return res;
}

inline Action *LaunchThreads(const char *message, uint_fast32_t N, void *args, size_t s_elem,
			void *(*routine)(void *), void (*freeArg)(void *))
{
	if (N == 0) {
		return DoNothingAction();
	}
	Action *res = (Action *)safeMalloc("action", sizeof(Action));

	res->is_do_nothing = 0;
	res->done = 0;
	res->return_value = -1;
	size_t message_len = strlen(message);
	res->message = (char *)safeMalloc("message", (message_len+1)*sizeof(char));
	strcpy(res->message, message);

	res->cancel = 0;
        pthread_t *thread;
	thread = (pthread_t *)safeMalloc("threads", N * sizeof(pthread_t)); 
	res->args = safeMalloc("thread args", N * (s_elem + sizeof(sig_atomic_t *)));

	info(T_NORMAL, "%s...\n", res->message);

	uint8_t *res_args = (uint8_t *)res->args;
	uint8_t *c_args = (uint8_t *)args;
	for (uint_fast32_t i = 0; i < N; ++i) {
		*((sig_atomic_t **)res_args) = &res->cancel;
		memcpy(res_args+sizeof(sig_atomic_t *), c_args, s_elem);

                safePThreadCreate(&(thread[i]),NULL,routine,res_args);

		c_args += s_elem;
		res_args += s_elem+sizeof(sig_atomic_t *);
	}

	res->N = N;
	res->thread = thread;
	if (freeArg == NULL) {
		res->freeArg = freeArgDoNothing;
	} else {
		res->freeArg = freeArg;
	}
	res->s_elem = s_elem;
	
	return res;
}

void CancelAction(Action *action)
{
	action->cancel = 1;
}

inline int WaitForFinished(Action *action)
{
	if (action->is_do_nothing) {
		return action->cancel;
	}
	if (action->done) {
		return action->return_value;
	}

	void *status;
	uint8_t *c_arg = (uint8_t *)(action->args) + sizeof(sig_atomic_t *);
	for (uint_fast32_t i = 0; i < action->N; ++i) {
		safePThreadJoin(action->thread[i], &status);

		if (status != NULL && status != PTHREAD_CANCELED) {
			error("Thread ended because of an error.\n");
		}
		c_arg += action->s_elem + sizeof(sig_atomic_t *);
	}

	int res = action->cancel;
	info(T_NORMAL, "%s : %s.\n", action->message, (res == 0) ? "DONE" : "CANCELED");
	action->return_value = res;
	action->done = 1;

	return res;
}

int LaunchThreadsAndWaitForFinished(const char *message, uint_fast32_t N, void *args, size_t s_elem,
					void *(*routine)(void *), void (*freeArg)(void *))
{
	Action *action = LaunchThreads(message, N, args, s_elem, routine, freeArg);
	int res = WaitForFinished(action);
	FreeAction(*action);
	free(action);

	return res;
}

int CancelActionAndWaitForFinished(Action *action)
{
	CancelAction(action);
	return WaitForFinished(action);
}

inline void FreeAction(Action action)
{
	if (action.is_do_nothing) {
		return;
	}
	if (!action.done) {
		error("Trying to free non-finished action.\n");
	}

	uint8_t *c_arg = (uint8_t *)(action.args) + sizeof(sig_atomic_t *);
	for (uint_fast32_t i = 0; i < action.N; ++i) {
		action.freeArg(c_arg);
		c_arg += action.s_elem + sizeof(sig_atomic_t *);
	}

	free(action.thread);
	free(action.args);
	free(action.message);
}

