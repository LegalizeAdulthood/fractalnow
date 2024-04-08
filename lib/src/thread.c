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

typedef struct StartThreadArg {
	pthread_mutex_t *startThreadCondMutex;
	pthread_cond_t *startThreadCond;
	pthread_mutex_t *allThreadsWaitingCondMutex;
	pthread_cond_t *allThreadsWaitingCond;
	uint_fast32_t N;
	uint_fast32_t *nbWaiting;
	void *(*startRoutine)(void *arg);
	void *arg;
	void **result;
} StartThreadArg;

void *StartThreadRoutine(void *arg);

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

void *DoNothingThreadRoutine(void *arg)
{
	(void)arg;

	return NULL;
}

Threads *CreateThreads(uint_fast32_t N)
{
	Threads *res = (Threads *)safeMalloc("threads", sizeof(Threads));

	res->thread = (pthread_t *)safeMalloc("pthreads", N*sizeof(pthread_t));
	res->startThreadArg = (StartThreadArg *)safeMalloc("startThreadArgs", N*sizeof(StartThreadArg));
	res->lastResult = (void **)safeMalloc("last results", N*sizeof(void *));
	res->N = N;
	safePThreadMutexInit(&res->startThreadCondMutex, NULL);
	safePThreadCondInit(&res->startThreadCond, NULL);
	safePThreadMutexInit(&res->allThreadsWaitingCondMutex, NULL);
	safePThreadCondInit(&res->allThreadsWaitingCond, NULL);
	safePThreadMutexLock(&res->allThreadsWaitingCondMutex);
	for (uint_fast32_t i = 0; i < N; ++i) {
		res->startThreadArg[i].startThreadCondMutex = &res->startThreadCondMutex;
		res->startThreadArg[i].startThreadCond = &res->startThreadCond;
		res->startThreadArg[i].allThreadsWaitingCondMutex = &res->allThreadsWaitingCondMutex;
		res->startThreadArg[i].allThreadsWaitingCond = &res->allThreadsWaitingCond;
		res->startThreadArg[i].N = N;
		res->startThreadArg[i].nbWaiting = &res->nbWaiting;
		res->startThreadArg[i].startRoutine = NULL;
		res->startThreadArg[i].arg = NULL;
		res->startThreadArg[i].result = &res->lastResult[i];
                safePThreadCreate(&(res->thread[i]),NULL,StartThreadRoutine,&res->startThreadArg[i]);
	}
	res->nbWaiting = 0;

	/* Wait till all threads are ready before returning. */
	safePThreadCondWait(&res->allThreadsWaitingCond, &res->allThreadsWaitingCondMutex);
	safePThreadMutexUnlock(&res->allThreadsWaitingCondMutex);

	return res;
}

void DestroyThreads(Threads *threads)
{
	uint_fast32_t nbWaiting = 0;
	safePThreadMutexLock(&threads->allThreadsWaitingCondMutex);
	nbWaiting = threads->nbWaiting;
	if (nbWaiting < threads->N) {
		safePThreadCondWait(&threads->allThreadsWaitingCond, 
					&threads->allThreadsWaitingCondMutex);
		nbWaiting = threads->nbWaiting;
	}
	safePThreadMutexUnlock(&threads->allThreadsWaitingCondMutex);

	safePThreadMutexLock(&threads->startThreadCondMutex);
	safePThreadMutexLock(&threads->allThreadsWaitingCondMutex);
	for (uint_fast32_t i = 0; i < threads->N; ++i) {
		threads->startThreadArg[i].startRoutine = NULL;
	}
	safePThreadCondBroadcast(&threads->startThreadCond);
	safePThreadMutexUnlock(&threads->allThreadsWaitingCondMutex);
	safePThreadMutexUnlock(&threads->startThreadCondMutex);

	void *status;
	for (uint_fast32_t i = 0; i < threads->N; ++i) {
		safePThreadJoin(threads->thread[i], &status);
		if (status != NULL) {
			fractal2D_message(stderr, T_QUIET, "Thread return value is not as \
expected.\n");
		}
	}

	free(threads->thread);
	free(threads->startThreadArg);
	free(threads->lastResult);
	free(threads);
}

inline Action *LaunchAction(const char *message, Threads *threads, uint_fast32_t nbThreadsNeeded,
				const void *args, size_t s_elem, void *(*routine)(void *),
				void (*freeArg)(void *))
{
	if (nbThreadsNeeded > threads->N) {
		fractal2D_error("More threads needed than available.\n");
	}
	if (nbThreadsNeeded == 0) {
		return DoNothingAction();
	}

	safePThreadMutexLock(&threads->allThreadsWaitingCondMutex);
	if (threads->nbWaiting < threads->N) {
		fractal2D_error("Trying to launch action while threads are still busy.\n");
	}
	safePThreadMutexUnlock(&threads->allThreadsWaitingCondMutex);

	Action *res = (Action *)safeMalloc("action", sizeof(Action));

	res->is_do_nothing = 0;
	res->done = 0;
	res->return_value = -1;
	res->cancel = 0;
	res->threadArgsHeaders = (ThreadArgHeader **)safeMalloc("internal thread args",
							nbThreadsNeeded * sizeof(ThreadArgHeader *));
	res->threads = threads;
	res->nbThreadsNeeded = nbThreadsNeeded;
	res->args = safeMalloc("thread args", nbThreadsNeeded * (s_elem + sizeof(ThreadArgHeader)));
	res->s_elem = s_elem;
	if (freeArg == NULL) {
		res->freeArg = freeArgDoNothing;
	} else {
		res->freeArg = freeArg;
	}
	size_t message_len = strlen(message);
	res->message = (char *)safeMalloc("message", (message_len+1)*sizeof(char));
	strcpy(res->message, message);

	uint8_t *res_args = (uint8_t *)res->args;
	uint8_t *c_args = (uint8_t *)args;
	ThreadArgHeader internalThreadArg;
	safePThreadMutexLock(&threads->startThreadCondMutex);
	safePThreadMutexLock(&threads->allThreadsWaitingCondMutex);
	for (uint_fast32_t i = 0; i < nbThreadsNeeded; ++i) {
		threads->startThreadArg[i].startRoutine = routine;
		threads->startThreadArg[i].arg = res_args;

		internalThreadArg.cancel = &res->cancel;
		internalThreadArg.progress = 0;
		res->threadArgsHeaders[i] = (ThreadArgHeader *)(res_args);
		*(res->threadArgsHeaders[i]) = internalThreadArg;

		memcpy(res_args+sizeof(ThreadArgHeader), c_args, s_elem);

		c_args += s_elem;
		res_args += s_elem+sizeof(ThreadArgHeader);
	}
	for (uint_fast32_t i = nbThreadsNeeded; i < threads->N; ++i) {
		threads->startThreadArg[i].startRoutine = DoNothingThreadRoutine;
		threads->startThreadArg[i].arg = NULL;
	}
	threads->nbWaiting = 0;

	fractal2D_message(stdout, T_NORMAL, "%s...\n", res->message);
	safePThreadCondBroadcast(&threads->startThreadCond);
	safePThreadMutexUnlock(&threads->startThreadCondMutex);
	safePThreadMutexUnlock(&threads->allThreadsWaitingCondMutex);

	return res;
}

void *StartThreadRoutine(void *arg)
{
	StartThreadArg *startThreadArg = (StartThreadArg *)arg;
	uint_fast32_t nbWaiting;
	do {
		safePThreadMutexLock(startThreadArg->startThreadCondMutex);
		safePThreadMutexLock(startThreadArg->allThreadsWaitingCondMutex);
		nbWaiting = *(startThreadArg->nbWaiting)+1;
		*(startThreadArg->nbWaiting) = nbWaiting;
		if (nbWaiting == startThreadArg->N) {
			safePThreadCondSignal(startThreadArg->allThreadsWaitingCond);
		}
		safePThreadMutexUnlock(startThreadArg->allThreadsWaitingCondMutex);

		safePThreadCondWait(startThreadArg->startThreadCond, startThreadArg->startThreadCondMutex);
		safePThreadMutexUnlock(startThreadArg->startThreadCondMutex);
		if (startThreadArg->startRoutine == NULL) {
			break;
		}
		*(startThreadArg->result) = startThreadArg->startRoutine(startThreadArg->arg);
	} while (1);

	return NULL;
}

void CancelAction(Action *action)
{
	action->cancel = 1;
}

void onActionFinished(Action *action)
{
	void *status;
	for (uint_fast32_t i = 0; i < action->threads->N; ++i) {
		status = action->threads->lastResult[i];

		if (status != NULL && status != PTHREAD_CANCELED) {
			fractal2D_message(stderr, T_QUIET, "Thread ended because of an error.\n");
		}
	}

	action->done = 1;
	action->return_value = action->cancel;
	fractal2D_message(stdout, T_NORMAL, "%s : %s.\n", action->message,
				(action->return_value == 0) ? "DONE" : "CANCELED");
}

int ActionIsFinished(Action *action)
{
	if (action->is_do_nothing || action->done) {
		return 1;
	}

	uint_fast32_t nbWaiting = 0;
	safePThreadMutexLock(&action->threads->allThreadsWaitingCondMutex);
	nbWaiting = action->threads->nbWaiting;
	safePThreadMutexUnlock(&action->threads->allThreadsWaitingCondMutex);

	int res = 0;
	if (nbWaiting == action->threads->N) {
		onActionFinished(action);
		res = 1;
	}

	return res;
}

inline int GetActionResult(Action *action)
{
	if (action->is_do_nothing) {
		return action->cancel;
	}
	if (action->done) {
		return action->return_value;
	}

	uint_fast32_t nbWaiting = 0;
	safePThreadMutexLock(&action->threads->allThreadsWaitingCondMutex);
	nbWaiting = action->threads->nbWaiting;
	if (nbWaiting < action->threads->N) {
		safePThreadCondWait(&action->threads->allThreadsWaitingCond,
					&action->threads->allThreadsWaitingCondMutex);
		nbWaiting = action->threads->nbWaiting;
	}
	safePThreadMutexUnlock(&action->threads->allThreadsWaitingCondMutex);

	onActionFinished(action);

	return action->return_value;
}

int LaunchActionBlocking(const char *message, Threads *threads, uint_fast32_t nbThreadsNeeded,
					const void *args, size_t s_elem, void *(*routine)(void *),
					void (*freeArg)(void *))
{
	Action *action = LaunchAction(message, threads, nbThreadsNeeded, args, s_elem, routine, freeArg);
	int res = GetActionResult(action);
	FreeAction(action);

	return res;
}

FLOAT GetActionProgress(const Action *action)
{
	FLOAT res;
	if (action->is_do_nothing || action->done) {
		res = 1;
	} else {
		res = 0;
		for (uint_fast32_t i = 0; i < action->nbThreadsNeeded; ++i) {
			res += action->threadArgsHeaders[i]->progress;
		}
		res /= 100 * action->nbThreadsNeeded;
	}

	return res;
}

inline void FreeAction(Action *action)
{
	if (action->is_do_nothing) {
		return;
	}
	if (!action->done) {
		fractal2D_error("Trying to free non-finished action.\n");
	}

	uint8_t *c_arg = (uint8_t *)(action->args) + sizeof(ThreadArgHeader);
	for (uint_fast32_t i = 0; i < action->nbThreadsNeeded; ++i) {
		action->freeArg(c_arg);
		c_arg += action->s_elem + sizeof(ThreadArgHeader);
	}

	free(action->threadArgsHeaders);
	free(action->args);
	free(action->message);
	free(action);
}

ThreadArgHeader *GetThreadArgHeader(const void *arg)
{
	return (ThreadArgHeader *)arg;
}

void *GetThreadArgBody(const void *arg)
{
	return ((uint8_t *)(arg)+sizeof(ThreadArgHeader));
}

