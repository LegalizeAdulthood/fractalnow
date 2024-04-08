/*
 *  thread.c -- part of FractalNow
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
#include "fractalnow.h"
#include "misc.h"
#include <stdlib.h>
#include <string.h>

#ifdef _ENABLE_MP_FLOATS
#include <stdio.h>
#include <mpfr.h>
#endif

void *StartThreadRoutine(void *arg);

Threads *CreateThreads(uint_fast32_t N)
{
	Threads *res = (Threads *)safeMalloc("threads", sizeof(Threads));

	res->thread = (pthread_t *)safeMalloc("pthreads", N*sizeof(pthread_t));
	res->startThreadArg = (StartThreadArg *)safeMalloc("startThreadArgs", N*sizeof(StartThreadArg));
	res->lastResult = (void **)safeMalloc("last results", N*sizeof(void *));
	res->N = N;
	safePThreadMutexInit(&res->startThreadCondMutex, NULL);
	safePThreadCondInit(&res->startThreadCond, NULL);
	safePThreadMutexInit(&res->threadsMutex, NULL);
	safePThreadCondInit(&res->allThreadsReadyCond, NULL);
	safePThreadMutexLock(&res->threadsMutex);
	safePThreadCondInit(&res->allPausedCond, NULL);
	safePThreadCondInit(&res->allResumedCond, NULL);
	safePThreadCondInit(&res->resumeTaskCond, NULL);
	res->nbReady = 0;
	res->nbPaused = 0;
	for (uint_fast32_t i = 0; i < N; ++i) {
		res->startThreadArg[i].threadId = i;
		res->startThreadArg[i].threads = res;
		res->startThreadArg[i].message = NULL;
		res->startThreadArg[i].startRoutine = NULL;
		res->startThreadArg[i].arg = NULL;
		res->startThreadArg[i].result = &res->lastResult[i];
		safePThreadCreate(&(res->thread[i]),NULL,StartThreadRoutine,&res->startThreadArg[i]);
	}

	/* Wait till all threads are ready before returning. */
	safePThreadCondWait(&res->allThreadsReadyCond, &res->threadsMutex);
	safePThreadMutexUnlock(&res->threadsMutex);

	return res;
}

void *StartThreadRoutine(void *arg)
{
#ifdef _ENABLE_MP_FLOATS
	mpfr_set_default_prec((mpfr_prec_t)fractalnow_mp_precision);
#endif

	StartThreadArg *startThreadArg = (StartThreadArg *)arg;
	Threads *threads = startThreadArg->threads;
	do {
		safePThreadMutexLock(&threads->startThreadCondMutex);
		safePThreadMutexLock(&threads->threadsMutex);
		++threads->nbReady;
		if (threads->nbReady+threads->nbPaused == threads->N) {
			safePThreadCondSignal(&threads->allPausedCond);
		}
		if (startThreadArg->message != NULL) {
			FractalNow_message(stdout, T_VERBOSE, "%s [%"PRIuFAST32"] : %s.\n",
				startThreadArg->message, startThreadArg->threadId,
				*startThreadArg->result == NULL ?
				"DONE" : "CANCELED");
		}
		if (threads->nbReady == threads->N) {
			safePThreadCondSignal(&threads->allThreadsReadyCond);
		}
		safePThreadMutexUnlock(&threads->threadsMutex);

		safePThreadCondWait(&threads->startThreadCond, &threads->startThreadCondMutex);
		safePThreadMutexUnlock(&threads->startThreadCondMutex);
		if (startThreadArg->startRoutine == NULL) {
			break;
		}
		if (startThreadArg->message != NULL) {
			FractalNow_message(stdout, T_VERBOSE, "%s [%"PRIuFAST32"]...\n",
					startThreadArg->message, startThreadArg->threadId);
		}
		*(startThreadArg->result) = startThreadArg->startRoutine(startThreadArg->arg);
	} while (1);

#ifdef _ENABLE_MP_FLOATS
	mpfr_free_cache();
#endif

	return NULL;
}

void DestroyThreads(Threads *threads)
{
	uint_fast32_t nbReady = 0;
	safePThreadMutexLock(&threads->threadsMutex);
	nbReady = threads->nbReady;
	if (nbReady < threads->N) {
		safePThreadCondWait(&threads->allThreadsReadyCond, 
					&threads->threadsMutex);
		nbReady = threads->nbReady;
	}
	safePThreadMutexUnlock(&threads->threadsMutex);

	safePThreadMutexLock(&threads->startThreadCondMutex);
	for (uint_fast32_t i = 0; i < threads->N; ++i) {
		threads->startThreadArg[i].startRoutine = NULL;
	}
	safePThreadCondBroadcast(&threads->startThreadCond);
	safePThreadMutexUnlock(&threads->startThreadCondMutex);

	void *status;
	for (uint_fast32_t i = 0; i < threads->N; ++i) {
		safePThreadJoin(threads->thread[i], &status);
		if (status != NULL) {
			FractalNow_message(stderr, T_QUIET, "Thread return value is not as \
expected.\n");
		}
	}

	safePThreadMutexDestroy(&threads->startThreadCondMutex);
	safePThreadMutexDestroy(&threads->threadsMutex);
	safePThreadCondDestroy(&threads->startThreadCond);
	safePThreadCondDestroy(&threads->allThreadsReadyCond);
	safePThreadCondDestroy(&threads->allPausedCond);
	safePThreadCondDestroy(&threads->allResumedCond);
	safePThreadCondDestroy(&threads->resumeTaskCond);
	free(threads->thread);
	free(threads->startThreadArg);
	free(threads->lastResult);
	free(threads);
}

ThreadArgHeader *GetThreadArgHeader(const void *arg)
{
	return *((ThreadArgHeader **)arg);
}

void *GetThreadArgBody(const void *arg)
{
	return *((void **)(((uint8_t *)arg)+sizeof(ThreadArgHeader *)));
}

static inline int ReadIntVarSafe(int *variable, pthread_spinlock_t *mutex)
{
	int res;
	safePThreadSpinLock(mutex);
	res = *variable;
	safePThreadSpinUnlock(mutex);
	return res;
}

static inline void WriteIntVarSafe(int *variable, int value, pthread_spinlock_t *mutex)
{
	safePThreadSpinLock(mutex);
	*variable = value;
	safePThreadSpinUnlock(mutex);
}

int CancelTaskRequested(ThreadArgHeader *threadArgHeader)
{
	return ReadIntVarSafe(threadArgHeader->cancel, threadArgHeader->cancelMutex);
}

void SetThreadProgress(ThreadArgHeader *threadArgHeader, int progress)
{
	WriteIntVarSafe(&threadArgHeader->progress, progress, &threadArgHeader->progressMutex);
}

void HandlePauseRequest(ThreadArgHeader *threadArgHeader)
{
	int pause = ReadIntVarSafe(threadArgHeader->pause, threadArgHeader->pauseMutex);
	if (pause) {
		uint_fast32_t nbThreadsActive;
		Threads *threads = threadArgHeader->threads;

		safePThreadMutexLock(&threads->threadsMutex);

		++threads->nbPaused;
		nbThreadsActive = threads->N-(*threadArgHeader->nbReady);
		if (threads->nbPaused == nbThreadsActive) {
			safePThreadCondSignal(&threads->allPausedCond);
		}

		safePThreadCondWait(&threads->resumeTaskCond, &threads->threadsMutex);

		--threads->nbPaused;
		if (threads->nbPaused == 0) {
			safePThreadCondSignal(&threads->allResumedCond);
		}
		safePThreadMutexUnlock(&threads->threadsMutex);
	}
}

