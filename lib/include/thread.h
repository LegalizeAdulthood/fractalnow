/*
 *  thread.h -- part of FractalNow
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
  * \file thread.h
  * \brief Header file related to threads.
  * \author Marc Pegon
  */

#ifndef __THREAD_H__
#define __THREAD_H__

#ifdef __cplusplus
extern "C" {
#endif

#include "floating_point.h"
#include <errno.h>
#include <pthread.h>
#include <signal.h>
#include <stdint.h>

/**
 * \def DEFAULT_NB_THREADS
 * \brief Default number of threads.
 */
#define DEFAULT_NB_THREADS (uint_fast32_t)(4)

/**
 * \def DEFAULT_RECTANGLES_PER_THREAD
 * \brief Default number of rectangles per thread.
 *
 * Used for working on images. Each thread
 * may be assigned a number of rectangles
 * to draw.
 * This can be used to distribute homogeneously
 * the work between threads.
 * Although it can be disturbing if the user can
 * view the progress of the action in real-time
 * (small rectangles being drawn erratically),
 * which is why this is 1 by default.
 *
 * Not that a workaround to distribute the work
 * more homogeneously is simply to create more
 * threads than machine cores.
 */
#define DEFAULT_RECTANGLES_PER_THREAD (uint_fast32_t)(1)

// thread related
#define THREAD_CREATE_ERROR "Thread create error"
#define THREAD_CANCEL_ERROR "Thread cancel error"
#define THREAD_JOIN_ERROR "Thread join error"
#define THREAD_MUTEX_INIT_ERROR "Thread mutex init error"
#define THREAD_MUTEX_LOCK_ERROR "Thread mutex lock error"
#define THREAD_MUTEX_UNLOCK_ERROR "Thread mutex unlock error"
#define THREAD_MUTEX_DESTROY_ERROR "Thread mutex destroy error"
#define THREAD_COND_INIT_ERROR "Thread cond init error"
#define THREAD_COND_WAIT_ERROR "Thread cond wait error"
#define THREAD_COND_SIGNAL_ERROR "Thread cond signal error"
#define THREAD_COND_BROADCAST_ERROR "Thread cond broadcast error"
#define THREAD_COND_DESTROY_ERROR "Thread cond destroy error"

#define safePThreadCreate(thread,attributes,startRoutine,argument) {if((errno=pthread_create(thread,attributes,startRoutine,argument))!=0){perror(THREAD_CREATE_ERROR);exit(EXIT_FAILURE);}}
#define safePThreadJoin(thread,status); {if((errno=pthread_join(thread,status))!=0){perror(THREAD_JOIN_ERROR);exit(EXIT_FAILURE);}}
#define safePThreadMutexInit(mutex,attributes) {if((errno=pthread_mutex_init(mutex,attributes))!=0){perror(THREAD_MUTEX_INIT_ERROR);exit(EXIT_FAILURE);}}
#define safePThreadMutexLock(mutex) {if((errno=pthread_mutex_lock(mutex))!=0){perror(THREAD_MUTEX_LOCK_ERROR);exit(EXIT_FAILURE);}}
#define safePThreadMutexUnlock(mutex) {if((errno=pthread_mutex_unlock(mutex))!=0){perror(THREAD_MUTEX_UNLOCK_ERROR);perror(THREAD_MUTEX_UNLOCK_ERROR);exit(EXIT_FAILURE);}}
#define safePThreadMutexDestroy(mutex) {if((errno=pthread_mutex_destroy(mutex))!=0){perror(THREAD_MUTEX_DESTROY_ERROR);exit(EXIT_FAILURE);}}
#define safePThreadCondInit(cond,attributes) {if((errno=pthread_cond_init(cond,attributes))!=0){perror(THREAD_COND_INIT_ERROR);exit(EXIT_FAILURE);}}
#define safePThreadCancel(thread) {if((errno=pthread_cancel(thread))!=0){perror(THREAD_CANCEL_ERROR);exit(EXIT_FAILURE);}}
#define safePThreadCondWait(cond,mutex) {if((errno=pthread_cond_wait(cond,mutex))!=0){perror(THREAD_COND_WAIT_ERROR);exit(EXIT_FAILURE);}}
#define safePThreadCondSignal(cond) {if((errno=pthread_cond_signal(cond))!=0){perror(THREAD_COND_SIGNAL_ERROR);exit(EXIT_FAILURE);}}
#define safePThreadCondBroadcast(cond) {if((errno=pthread_cond_broadcast(cond))!=0){perror(THREAD_COND_BROADCAST_ERROR);exit(EXIT_FAILURE);}}
#define safePThreadCondDestroy(cond) {if((errno=pthread_cond_destroy(cond))!=0){perror(THREAD_COND_DESTROY_ERROR);exit(EXIT_FAILURE);}}

#ifdef NO_SPINLOCK
#define THREAD_SPIN_INIT_ERROR THREAD_MUTEX_INIT_ERROR
#define THREAD_SPIN_LOCK_ERROR THREAD_MUTEX_LOCK_ERROR
#define THREAD_SPIN_UNLOCK_ERROR THREAD_MUTEX_UNLOCK_ERROR
#define THREAD_SPIN_DESTROY_ERROR THREAD_MUTEX_DESTROY_ERROR

#define safePThreadSpinInit(mutex,attributes) safePThreadMutexInit(mutex,attributes)
#define safePThreadSpinLock(mutex) safePThreadMutexLock(mutex)
#define safePThreadSpinUnlock(mutex) safePThreadMutexUnlock(mutex)
#define safePThreadSpinDestroy(mutex) safePThreadMutexDestroy(mutex)

#define pthread_spinlock_t pthread_mutex_t
#define SPIN_INIT_ATTR NULL
#else
#define THREAD_SPIN_INIT_ERROR "Thread spin init error"
#define THREAD_SPIN_LOCK_ERROR "Thread spin lock error"
#define THREAD_SPIN_UNLOCK_ERROR "Thread spin unlock error"
#define THREAD_SPIN_DESTROY_ERROR "Thread spin destroy error"

#define safePThreadSpinInit(mutex,attributes) {if((errno=pthread_spin_init(mutex,attributes))!=0){perror(THREAD_SPIN_INIT_ERROR);exit(EXIT_FAILURE);}}
#define safePThreadSpinLock(mutex) {if((errno=pthread_spin_lock(mutex))!=0){perror(THREAD_SPIN_LOCK_ERROR);exit(EXIT_FAILURE);}}
#define safePThreadSpinUnlock(mutex) {if((errno=pthread_spin_unlock(mutex))!=0){perror(THREAD_SPIN_UNLOCK_ERROR);perror(THREAD_SPIN_UNLOCK_ERROR);exit(EXIT_FAILURE);}}
#define safePThreadSpinDestroy(mutex) {if((errno=pthread_spin_destroy(mutex))!=0){perror(THREAD_SPIN_DESTROY_ERROR);exit(EXIT_FAILURE);}}
#define SPIN_INIT_ATTR PTHREAD_PROCESS_PRIVATE
#endif

struct StartThreadArg;

/**
 * \struct Threads
 * \brief Threads structure.
 *
 * Threads should be created on program startup, and destroyed
 * on exit.\n
 * Once created, threads can be used to launch actions, such
 * as drawing fractals, applying filters on images, etc..
 */
/**
 * \typedef Threads
 * \brief Convenient typedef for struct Threads.
 */
typedef struct Threads {
	uint_fast32_t N;
 /*!< N Number of threads in this structure.*/
	uint_fast32_t nbReady;
 /*!< Number of threads ready to launch a new task.*/
	uint_fast32_t nbPaused;
 /*!< Number of threads currently paused (waiting resumeTaskCond to be signaled).*/
	pthread_mutex_t startThreadCondMutex;
 /*!< Mutex for starting threads condition.*/
	pthread_cond_t startThreadCond;
 /*!< Condition signaled when thread should start working on a new task.*/
	pthread_mutex_t threadsMutex;
 /*!< Main threads mutex (multiple use).*/
	pthread_cond_t allThreadsReadyCond; 
 /*!< Condition signaled when all threads are ready to launch a new task.*/
	pthread_cond_t allPausedCond;
 /*!< Condition signaled when all active threads have been paused.*/
	pthread_cond_t allResumedCond;
 /*!< Condition signaled when all previously paused threads have resumed.*/
	pthread_cond_t resumeTaskCond;
 /*!< Condition signaled when resume task is requested.*/
 	pthread_t *thread;
 /*!< Pthreads array.*/
	struct StartThreadArg *startThreadArg;
 /*!< Start thread argument.*/
	void **lastResult;
 /*!< Last result for each thread.*/
} Threads;

/**
 * \struct ThreadArgHeader
 * \brief Thread argument header.
 */
/**
 * \typedef ThreadArgHeader
 * \brief Convenient typedef for struct ThreadArgHeader.
 */
typedef struct ThreadArgHeader {
	uint_fast32_t threadId;
 /*!< Id of thread working on this argument.*/
	Threads *threads;
 /*!< Pointer to threads structure.*/
	uint_fast32_t *nbReady;
 /*!< Number of threads ready for next task, OR subtask (for composite tasks). This does not necessarily points to threads->nbWaiting.*/

	int *cancel;
 /*!< Used by thread to receive cancellation request.*/
	pthread_spinlock_t *cancelMutex;
 /*!< Mutex for cancel variable.*/
	int *pause;
 /*!< Used to pause thread.*/
	pthread_spinlock_t *pauseMutex;
 /*!< Mutex for pause variable.*/
	int progress;
 /*!< Should be kept up-to-date by thread to give an hint on its progress (value between 0 and 100).*/
	pthread_spinlock_t progressMutex;
 /*!< Mutex for progress variable.*/
} ThreadArgHeader;

/**
 * \struct StartThreadArg
 * \brief Structure used to start tasks (for internal use).
 */
/**
 * \typedef StartThreadArg
 * \brief Convenient typedef for struct StartThreadArg.
 */
typedef struct StartThreadArg {
	uint_fast32_t threadId;
 /*!< Thread id (between 0 and threads->N-1.*/
	Threads *threads;
 /*!< Threads structure.*/
	char *message;
 /*!< Pointer to threads structure (convenient to access threads common variables).*/
	void *(*startRoutine)(void *arg);
 /*!< Routine to be launched by thread (task routine).*/
	void *arg;
 /*!< Argument to be passed to thread routine.*/
	void **result;
 /*!< Where to put startRoutine return value.*/
} StartThreadArg;


/**
 * \fn Threads *CreateThreads(uint_fast32_t N)
 * \brief Create threads.
 *
 * \param N Number of threads to be created.
 * \return Newly-allocated threads structure.
 */
Threads *CreateThreads(uint_fast32_t N);

/**
 * \fn void DestroyThreads(Threads *threads)
 * \brief Destroy threads.
 *
 * Threads should not be busy.\n
 * Function is blocking, i.e. will wait for threads to
 * finish before destroying them.
 *
 * \param threads Threads structure to destroy and free.
 */
void DestroyThreads(Threads *threads);

/**
 * \fn ThreadArgHeader *GetThreadArgHeader(const void *arg)
 * \brief Get header part of thread argument.
 *
 * \param arg Argument passed to thread routine.
 * \return Header part of thread argument.
 */
ThreadArgHeader *GetThreadArgHeader(const void *arg);

/**
 * \fn void *GetThreadArgBody(const void *arg)
 * \brief Get body (user) part of thread argument.
 *
 * \param arg Argument passed to thread routine.
 * \return Body part of thread argument (i.e. pointer to argument passed by user when launching action).
 */
void *GetThreadArgBody(const void *arg);

/**
 * \fn int CancelTaskRequested(ThreadArgHeader *threadArgHeader)
 * \brief Query task cancelation request, through argument header.
 *
 * This function is thread-safe.
 *
 * \param threadArgHeader Thread argument header.
 * \return 1 if cancelation request has been made, 0 otherwise.
 */
int CancelTaskRequested(ThreadArgHeader *threadArgHeader);

/**
 * \fn int SetThreadProgress(ThreadArgHeader *threadArgHeader, int progress)
 * \brief Set thread progress, through argument header.
 *
 * This function is thread-safe.
 * Progress should be a value between 0 and 100 (unspecified behaviour
 * otherwise).
 *
 * \param threadArgHeader Thread argument header.
 * \param progress Task progress.
 */
void SetThreadProgress(ThreadArgHeader *threadArgHeader, int progress);

/**
 * \fn void HandlePauseRequest(ThreadArgHeader *threadArgHeader)
 * \brief Handle pause request.
 *
 * Blocks if task has been paused, until task is resumed.
 * \see task.c
 *
 * \param threadArgHeader Thread argument header.
 */
void HandlePauseRequest(ThreadArgHeader *threadArgHeader);

#ifdef __cplusplus
}
#endif

#endif
