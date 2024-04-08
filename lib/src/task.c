/*
 *  task.c -- part of FractalNow
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

#include "task.h"
#include "error.h"
#include "misc.h"
#include <string.h>

#define ARG_SIZE (size_t)(sizeof(ThreadArgHeader *)+sizeof(void *))

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

void *DoNothingThreadRoutine(void *arg)
{
	ThreadArgHeader *c_arg = GetThreadArgHeader(arg);
	HandlePauseRequest(c_arg);

	return (CancelTaskRequested(c_arg) ? PTHREAD_CANCELED : NULL);
}

void FreeArgDoNothing(void *arg)
{
	UNUSED(arg);
}

#ifdef DEBUG
char DoNothingMessage[] = "Doing nothing";
#else
char *DoNothingMessage = NULL;
#endif

Task *DoNothingTask()
{

	return CreateTask(DoNothingMessage, 0, NULL, 0, DoNothingThreadRoutine, FreeArgDoNothing);
}

static inline Task *aux_CreateTask(const char message[], uint_fast32_t nbThreadsNeeded,
					size_t s_elem, void *(*routine)(void *),
					void (*freeArg)(void *))
{
	Task *res = (Task *)safeMalloc("task", sizeof(Task));

	res->launchPrepared = 0;
	res->hasBeenLaunched = 0;
	res->done = 0;
	res->returnValue = -1;
	res->nbThreadsNeeded = nbThreadsNeeded;
	res->initialArgs = NULL;
	safePThreadSpinInit(&res->cancelMutex, SPIN_INIT_ATTR);
	WriteIntVarSafe(&res->cancel, 0, &res->cancelMutex);
	safePThreadSpinInit(&res->pauseMutex, SPIN_INIT_ATTR);
	WriteIntVarSafe(&res->pause, 0, &res->pauseMutex);

	res->isComposite = 0;
	res->compositeTaskArguments.thisTask = NULL;
	res->nbSubTasks = 0;
	res->subTasks = NULL;
	res->compositeTaskNbReady = 0;
	res->stopLaunchingSubTasks = 0;
	safePThreadCondInit(&res->compositeTaskAllThreadsReadyCond, NULL);

	res->s_elem = s_elem;
	if (freeArg == NULL) {
		res->freeArg = FreeArgDoNothing;
	} else {
		res->freeArg = freeArg;
	}

	if (message == NULL) {
		res->message = NULL;
	} else {
		res->message = (char *)safeMalloc("message", (strlen(message)+1)*sizeof(char));
		strcpy(res->message, message);
	}
	res->threadsRoutine = routine;

	return res;
}

inline Task *CreateTask(const char message[], uint_fast32_t nbThreadsNeeded,
				const void *args, size_t s_elem, void *(*routine)(void *),
				void (*freeArg)(void *))
{
	Task *res = aux_CreateTask(message, nbThreadsNeeded, s_elem, routine, freeArg);

	if (res->s_elem > 0) {
		res->initialArgs = safeMalloc("thread args", nbThreadsNeeded * s_elem);
		memcpy(res->initialArgs, args, nbThreadsNeeded * s_elem);
	}

	return res;
}

void *CompositeTaskRoutine(void *arg);

Task *CreateCompositeTask(const char message[], uint_fast32_t nbSubTasks, Task *subTasks[])
{
	if (nbSubTasks == 0) {
		FractalNow_error("Number of subtasks must be > 0 when creating composite task.\n");
	}

	uint_fast32_t nbThreadsNeeded = subTasks[0]->nbThreadsNeeded;
	for (uint_fast32_t i = 1; i < nbSubTasks; ++i) {
		if (subTasks[i]->nbThreadsNeeded > nbThreadsNeeded) {
			nbThreadsNeeded = subTasks[i]->nbThreadsNeeded;
		}
	}

	Task *res = aux_CreateTask(message, nbThreadsNeeded, sizeof(CompositeTaskArguments),
					CompositeTaskRoutine, NULL);

	res->isComposite = 1;
	res->compositeTaskArguments.thisTask = res;

	res->nbSubTasks = nbSubTasks;
	res->subTasks = (Task **)safeMalloc("copy subtasks", nbSubTasks*sizeof(Task *));
	memcpy(res->subTasks, subTasks, nbSubTasks*sizeof(Task *));

	return res;
}

/* nbReady override for tasks that are subtasks. */
static void PrepareLaunchTask(Task *task, Threads *threads, uint_fast32_t *nbReadyOverride)
{
	task->threads = threads;
	task->threadArgsHeaders = (ThreadArgHeader *)safeMalloc("thread arg headers",
							threads->N * sizeof(ThreadArgHeader));
	task->args = (void *)safeMalloc("thread args", threads->N * ARG_SIZE);
	
	uint8_t *task_args = (uint8_t *)task->args;
	uint8_t *c_args = (uint8_t *)task->initialArgs;
	ThreadArgHeader *threadArgHeader = task->threadArgsHeaders;
	for (uint_fast32_t i = 0; i < threads->N; ++i) {
		threadArgHeader->threadId = i;
		threadArgHeader->threads = threads;
		if (nbReadyOverride != NULL) {
			threadArgHeader->nbReady = nbReadyOverride;
		} else {
			threadArgHeader->nbReady = &threads->nbReady;
		}

		threadArgHeader->cancel = &task->cancel;
		threadArgHeader->cancelMutex = &task->cancelMutex;
		threadArgHeader->pause = &task->pause;
		threadArgHeader->pauseMutex = &task->pauseMutex;
		threadArgHeader->progress = 0;
		safePThreadSpinInit(&threadArgHeader->progressMutex, SPIN_INIT_ATTR);

		*((ThreadArgHeader **)task_args) = threadArgHeader;
		if (task->isComposite) {
			*((void **)(task_args + sizeof(ThreadArgHeader *))) =
				&task->compositeTaskArguments;
		} else {
			if (i < task->nbThreadsNeeded) {
				*((void **)(task_args + sizeof(ThreadArgHeader *))) = c_args;
				c_args += task->s_elem;
			} else {
				*((void **)(task_args + sizeof(ThreadArgHeader *))) = NULL;
			}
		}

		task_args += ARG_SIZE;
		++threadArgHeader;
	}
	if (task->isComposite) {
		for (uint_fast32_t i = 0; i < task->nbSubTasks; ++i) {
			PrepareLaunchTask(task->subTasks[i], threads, &task->compositeTaskNbReady);
		}
	}
	task->launchPrepared = 1;
}

void LaunchTask(Task *task, Threads *threads)
{
	if (task->hasBeenLaunched) {
		FractalNow_error("Trying to launch task that has already been launched.\n");
	}
	safePThreadMutexLock(&threads->threadsMutex);
	if (threads->nbReady < threads->N) {
		FractalNow_error("Trying to launch task while threads are still busy.\n");
	}
	safePThreadMutexUnlock(&threads->threadsMutex);

	PrepareLaunchTask(task, threads, NULL);

	uint8_t *task_args = (uint8_t *)task->args;
	safePThreadMutexLock(&threads->startThreadCondMutex);
	for (uint_fast32_t i = 0; i < threads->N; ++i) {
		threads->startThreadArg[i].message = task->message;
		if (task->isComposite || i < task->nbThreadsNeeded) {
			threads->startThreadArg[i].startRoutine = task->threadsRoutine;
		} else {
			threads->startThreadArg[i].startRoutine = DoNothingThreadRoutine;
		}
		threads->startThreadArg[i].arg = task_args;

		task_args += ARG_SIZE;
	}
	/* Reinit nbReady counter. */
	threads->nbReady = 0;
	threads->nbPaused = 0;
	task->hasBeenLaunched = 1;
	if (task->message != NULL) {
		FractalNow_message(stdout, T_NORMAL, "%s...\n", task->message);
	}
	safePThreadCondBroadcast(&threads->startThreadCond);
	safePThreadMutexUnlock(&threads->startThreadCondMutex);
}

void CancelTask(Task *task)
{
	if (task->isComposite) {
		for (uint_fast32_t i = 0; i < task->nbSubTasks; ++i) {
			CancelTask(task->subTasks[i]);
		}
	}

	WriteIntVarSafe(&task->cancel, 1, &task->cancelMutex);
}

static inline void aux_PauseTask(Task *task)
{
	if (task->isComposite) {
		WriteIntVarSafe(&task->pause, 1, &task->pauseMutex);
		for (uint_fast32_t i = 0; i < task->nbSubTasks; ++i) {
			aux_PauseTask(task->subTasks[i]);
		}
	} else {
		WriteIntVarSafe(&task->pause, 1, &task->pauseMutex);
	}
}

void PauseTask(Task *task)
{
	if (!task->hasBeenLaunched || task->done) {
		return;
	}
	int pause = ReadIntVarSafe(&task->pause, &task->pauseMutex);
	if (pause) {
		return;
	}
	Threads *threads = task->threads;
	safePThreadMutexLock(&threads->threadsMutex);
	if (threads->nbReady < threads->N) {
		threads->nbPaused = 0;
		aux_PauseTask(task);
		safePThreadCondWait(&threads->allPausedCond, &threads->threadsMutex);
	}
	safePThreadMutexUnlock(&threads->threadsMutex);
}

static inline void aux_ResumeTask(Task *task)
{
	if (task->isComposite) {
		for (uint_fast32_t i = 0; i < task->nbSubTasks; ++i) {
			aux_ResumeTask(task->subTasks[i]);
		}
		WriteIntVarSafe(&task->pause, 0, &task->pauseMutex);
	} else {
		WriteIntVarSafe(&task->pause, 0, &task->pauseMutex);
	}
}

void ResumeTask(Task *task)
{
	if (!task->hasBeenLaunched || task->done) {
		return;
	}

	Threads *threads = task->threads;

	safePThreadMutexLock(&task->threads->threadsMutex);
	aux_ResumeTask(task);

	if (threads->nbPaused > 0) {
		safePThreadCondBroadcast(&threads->resumeTaskCond);
		safePThreadCondWait(&threads->allResumedCond, &threads->threadsMutex);
	}
	safePThreadMutexUnlock(&threads->threadsMutex);
}

void onTaskFinished(Task *task)
{
	void *status;
	for (uint_fast32_t i = 0; i < task->threads->N; ++i) {
		status = task->threads->lastResult[i];

		if (status != NULL && status != PTHREAD_CANCELED) {
			FractalNow_message(stderr, T_QUIET, "Thread ended because of an error.\n");
		}
	}

	task->done = 1;
	task->returnValue = ReadIntVarSafe(&task->cancel, &task->cancelMutex);
	if (task->message != NULL) {
		FractalNow_message(stdout, T_NORMAL, "%s : %s.\n", task->message,
					(task->returnValue == 0) ? "DONE" : "CANCELED");
	}
}

int TaskIsFinished(Task *task)
{
	if (task->done) {
		return 1;
	}
	if (!task->hasBeenLaunched) {
		return 0;
	}

	uint_fast32_t nbReady = 0;
	safePThreadMutexLock(&task->threads->threadsMutex);
	nbReady = task->threads->nbReady;
	safePThreadMutexUnlock(&task->threads->threadsMutex);

	int res = 0;
	if (nbReady == task->threads->N) {
		onTaskFinished(task);
		res = 1;
	}

	return res;
}

inline int GetTaskResult(Task *task)
{
	if (task->done) {
		return task->returnValue;
	}
	if (!task->hasBeenLaunched) {
		FractalNow_error("Cannot get result of not-yet-launched task.\n");
	}

	uint_fast32_t nbReady = 0;
	safePThreadMutexLock(&task->threads->threadsMutex);
	nbReady = task->threads->nbReady;
	if (nbReady < task->threads->N) {
		safePThreadCondWait(&task->threads->allThreadsReadyCond,
					&task->threads->threadsMutex);
		nbReady = task->threads->nbReady;
	}
	safePThreadMutexUnlock(&task->threads->threadsMutex);

	onTaskFinished(task);

	return task->returnValue;
}

int ExecuteTaskBlocking(Task *task, Threads *threads)
{
	LaunchTask(task, threads);
	int res = GetTaskResult(task);
	FreeTask(task);

	return res;
}

double GetTaskProgress(const Task *task)
{
	double res;
	if (!task->hasBeenLaunched) {
		res = 0;
	} else if (task->done) {
		res = 1;
	} else if (!task->isComposite) {
		res = 0;
		for (uint_fast32_t i = 0; i < task->nbThreadsNeeded; ++i) {
			res += ReadIntVarSafe(&task->threadArgsHeaders[i].progress, 
						&task->threadArgsHeaders[i].progressMutex);
		}
		res /= 100 * task->nbThreadsNeeded;
	} else {
		res = 0;
		for (uint_fast32_t i = 0; i < task->nbSubTasks; ++i) {
			res += GetTaskProgress(task->subTasks[i]);
		}
		res /= task->nbSubTasks;
	}

	return res;
}

inline void FreeTask(Task *task)
{
	if (task->hasBeenLaunched && !task->done) {
		FractalNow_error("Trying to free non-finished task.\n");
	}

	uint8_t *c_arg = (uint8_t *)task->initialArgs;
	for (uint_fast32_t i = 0; i < task->nbThreadsNeeded; ++i) {
		task->freeArg(c_arg);
		c_arg += task->s_elem;
	}
	if (task->initialArgs != NULL) {
		free(task->initialArgs);
	}
	if (task->launchPrepared) {
		Threads *threads = task->threads;
		for (uint_fast32_t i = 0; i < threads->N; ++i) {
			safePThreadSpinDestroy(&task->threadArgsHeaders[i].progressMutex);
		}
		free(task->threadArgsHeaders);
		free(task->args);
	}
	if (task->message != NULL) {
		free(task->message);
	}

	if (task->isComposite) {
		for (uint_fast32_t i = 0; i < task->nbSubTasks; ++i) {
			FreeTask(task->subTasks[i]);
		}
		free(task->subTasks);
	}

	safePThreadSpinDestroy(&task->cancelMutex);
	safePThreadSpinDestroy(&task->pauseMutex);
	safePThreadCondDestroy(&task->compositeTaskAllThreadsReadyCond);
	free(task);
}

static inline void CompositeTaskRendezVous(ThreadArgHeader *threadArgHeader, Task *thisTask,
						Task *finishedTask, Task *newTask, uint_fast32_t i)
{
	Threads *threads = threadArgHeader->threads;
	safePThreadMutexLock(&threads->threadsMutex);
	++thisTask->compositeTaskNbReady;

	if (threads->nbPaused > 0 && thisTask->compositeTaskNbReady+threads->nbPaused == threads->N) {
		safePThreadCondSignal(&threads->allPausedCond);
	}

	if (finishedTask != NULL && finishedTask->message != NULL) {
		FractalNow_message(stdout, T_VERBOSE, "%s [%"PRIuFAST32"] : %s.\n",
			finishedTask->message, threadArgHeader->threadId,
			threads->lastResult[threadArgHeader->threadId] == NULL ?
			"DONE" : "CANCELED");
	}
	if (thisTask->compositeTaskNbReady == threads->N) {
		if (finishedTask != NULL) {
			onTaskFinished(finishedTask);
		}
		if (newTask != NULL) {
			if (CancelTaskRequested(threadArgHeader)) {
				thisTask->stopLaunchingSubTasks = 1;
			} else {
				threadArgHeader->progress = 100 * i / thisTask->nbSubTasks;
				thisTask->compositeTaskNbReady = 0;
				newTask->hasBeenLaunched = 1;
				if (newTask->message != NULL) {
					FractalNow_message(stdout, T_NORMAL, "%s...\n", newTask->message);
				}
			}
		} else {
			threadArgHeader->progress = 100;
		}
		safePThreadCondBroadcast(&thisTask->compositeTaskAllThreadsReadyCond);
	} else {
		safePThreadCondWait(&thisTask->compositeTaskAllThreadsReadyCond, &threads->threadsMutex);
	}
	safePThreadMutexUnlock(&threads->threadsMutex);
}

void *CompositeTaskRoutine(void *arg)
{
	ThreadArgHeader *threadArgHeader = GetThreadArgHeader(arg);
	CompositeTaskArguments *c_arg = (CompositeTaskArguments *)GetThreadArgBody(arg);

	Task *thisTask = c_arg->thisTask;
	Task *currentTask = NULL;
	Task *previousTask = NULL;
	void *result;
	int canceled = 0;

	uint_fast32_t i;
	for (i = 0; i < thisTask->nbSubTasks; ++i) {
		previousTask = currentTask;
		currentTask = thisTask->subTasks[i];

		CompositeTaskRendezVous(threadArgHeader, thisTask, previousTask, currentTask, i);
		if (thisTask->stopLaunchingSubTasks) {
			canceled = 1;
			break;
		} else {
			if (currentTask->message != NULL) {
				FractalNow_message(stdout, T_VERBOSE, "%s [%"PRIuFAST32"]...\n",
						currentTask->message, threadArgHeader->threadId);
			}
			if (currentTask->isComposite || threadArgHeader->threadId <
					currentTask->nbThreadsNeeded) {
				result = currentTask->threadsRoutine(((uint8_t *)currentTask->args)+
									threadArgHeader->threadId*ARG_SIZE);
			} else {
				result = DoNothingThreadRoutine(((uint8_t *)currentTask->args)+
								threadArgHeader->threadId*ARG_SIZE);
			}
			currentTask->threads->lastResult[threadArgHeader->threadId] = result;
		}
	}
	if (!canceled) {
		CompositeTaskRendezVous(threadArgHeader, thisTask, currentTask, NULL, i);
	}

	return (CancelTaskRequested(threadArgHeader) ? PTHREAD_CANCELED : NULL);
}

