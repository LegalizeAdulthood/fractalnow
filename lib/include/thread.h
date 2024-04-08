/*
 *  thread.h -- part of fractal2D
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
#define safePThreadMutexUnlock(mutex) {if((errno=pthread_mutex_unlock(mutex))!=0){perror(THREAD_MUTEX_UNLOCK_ERROR);exit(EXIT_FAILURE);}}
#define safePThreadMutexDestroy(mutex) {if((errno=pthread_mutex_destroy(mutex))!=0){perror(THREAD_MUTEX_DESTROY_ERROR);exit(EXIT_FAILURE);}}
#define safePThreadCondInit(cond,attributes) {if((errno=pthread_cond_init(cond,attributes))!=0){perror(THREAD_COND_INIT_ERROR);exit(EXIT_FAILURE);}}
#define safePThreadCancel(thread) {if((errno=pthread_cancel(thread))!=0){perror(THREAD_CANCEL_ERROR);exit(EXIT_FAILURE);}}
#define safePThreadCondWait(cond,mutex) {if((errno=pthread_cond_wait(cond,mutex))!=0){perror(THREAD_COND_WAIT_ERROR);exit(EXIT_FAILURE);}}
#define safePThreadCondSignal(cond) {if((errno=pthread_cond_signal(cond))!=0){perror(THREAD_COND_SIGNAL_ERROR);exit(EXIT_FAILURE);}}
#define safePThreadCondBroadcast(cond) {if((errno=pthread_cond_broadcast(cond))!=0){perror(THREAD_COND_BROADCAST_ERROR);exit(EXIT_FAILURE);}}
#define safePThreadCondDestroy(cond) {if((errno=pthread_cond_destroy(cond))!=0){perror(THREAD_COND_DESTROY_ERROR);exit(EXIT_FAILURE);}}

struct StartThreadArg;

/**
 * \struct Threads
 * \brief Threads structure.
 *
 * Threads should be created on program startup, and destroyed
 * at exit.
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
	uint_fast32_t nbWaiting;
 /*!< Number of threads waiting to take an action (i.e. ready).*/
	pthread_mutex_t startThreadCondMutex;
 /*!< Mutex for starting threads condition.*/
	pthread_cond_t startThreadCond;
 /*!< Starting thread condition.*/
	pthread_mutex_t allThreadsWaitingCondMutex;
 /*!< Mutex for allThreadsWaiting condition.*/
	pthread_cond_t allThreadsWaitingCond; 
 /*!< Condition signaled when all threads are ready.*/
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
	sig_atomic_t *cancel;
 /*!< Used by thread to receive cancellation request.*/
	sig_atomic_t progress;
 /*!< Should be maintained up-to-date by thread to give an hint on its progress (value between 0 and 100).*/
} ThreadArgHeader;

/**
 * \struct Action
 * \brief Action created when launching simultaneous threads.
 *
 * There are two choices when launching threads :
 * - use a function that launches threads and then waits for
 *   them to finish
 * - use a function that launches threads and returns an 
 *   action structure
 * Through the action structure, it is possible to cancel
 * the threads (cancel action) or to wait for them to 
 * finish (get action result).
 *
 * Examples of use :
 * 1) Launch threads and wait for them to finish :
 * res = LaunchActionBlocking(message, threads, nbThreadsNeeded,
		args, s_elem, routine, freeArgs);
 *
 * 2) Launch threads, cancel action after some time, wait for it to
 *    finish.
 * action = LaunchAction(message, threads, nbThreadsNeeded, args,
		s_elem, routine, freeArgs); // launch threads
 * sleep(2);
 * CancelAction(action); // cancel action (send a request, actually)
 * res = GetActionResult(action); // wait for it to finish
 *
 * It is up to the thread routine to take into account cancellation
 * request and finish early.
 * The request is sent through the argument passed to the routine :
 * the first sizeof(ThreadArgHeader) bytes store a pointer to
 * a structure which contains data for cancellation request & progress.
 * The remaining part of the argument is the 'real' argument (of size
 * s_elem) that was passed to LaunchAction.
 */
/**
 * \typedef Action
 * \brief Convenient typedef for struct Action.
 */
typedef struct Action {
	int is_do_nothing;
 /*!< 1 if action is to do nothing (convenient).*/
	int done;
 /*!< 1 if action has already finished and threads have been joined.*/
	int return_value;
 /*!< Action return value (0 if finished normally, 1 if canceled).*/
	sig_atomic_t cancel;
 /*!< uint8_t used for cancellation request.*/
	struct ThreadArgHeader **threadArgsHeaders;
 /*!< Internal part of thread arguments (cancel and progress handling).*/
	struct Threads *threads;
 /*!< Pointer to threads to use for action.*/
	uint_fast32_t nbThreadsNeeded;
 /*!< Number of threads actually needed for action.*/
 	void *args;
 /*!< Array of args passed to thread routine.*/
	size_t s_elem;
 /*!< Size of each argument.*/
	void (*freeArg)(void *);
 /*!< Routine to free each arg.*/
	char *message;
 /*!< Action message to print at launch and when it finishes.*/
} Action;

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
 * Threads should not be busy.
 * Function is blocking, i.e. will wait for threads to
 * finish before destroying them.
 *
 * \param threads Threads structure to destroy and free.
 */
void DestroyThreads(Threads *threads);

/**
 * \fn Action *DoNothingAction()
 * \brief Returns a newly-allocated action that does nothing.
 *
 * Convenient for working on empty image for example.
 *
 * \return Do-nothing action.
 */
Action *DoNothingAction();

/**
 * \fn Action *LaunchAction(const char *message, Threads *threads, uint_fast32_t nbThreadsNeeded, const void *args, size_t s_elem, void *(*routine)(void *), void (*freeArg)(void *))
 * \brief Launch action (non-blocking).
 *
 * args is copied.
 * Note that arguments passed to the thread routine are not
 * exactly those passed to the function :
 * A ThreadArgHeader structure is added at the beginning of the
 * argument, which should be used to handle cancellation requests and
 * action progress.
 * The remaining bytes are the 'real' argument.
 * Free argument routine will be called for each argument when action
 * is freed (can be NULL if arguments contain no dynamically
 * allocated data).
 *
 * \param message Action message to print at launch and when action finishes.
 * \param threads Threads that will be used to execute action.
 * \param nbThreadsNeeded Number of threads needed to launch action (must be <= number of threads).
 * \param args Pointer to array of arguments for threads routines.
 * \param s_elem Size of one argument (in bytes).
 * \param routine Threads routine.
 * \param freeArg Routine to free each argument.
 * \return Corresponding newly-allocated action.
 */
Action *LaunchAction(const char *message, Threads *threads, uint_fast32_t nbThreadsNeeded,
			const void *args, size_t s_elem, void *(*routine)(void *),
			void (*freeArg)(void *));

/**
 * \fn int LaunchActionBlocking(const char *message, Threads *threads, uint_fast32_t nbThreadsNeeded, const void *args, size_t s_elem, void *(*routine)(void *), void (*freeArg)(void *))
 * \brief Launch action and wait for it to finish (blocking).
 *
 * \see LaunchAction
 *
 * \param message Action message to print at launch and when action finishes.
 * \param threads Threads that will be used to execute action.
 * \param nbThreadsNeeded Number of threads needed to launch action (must be <= number of threads).
 * \param args Pointer to array of arguments for threads routines.
 * \param s_elem Size of one argument (in bytes).
 * \param routine Threads routine.
 * \param freeArg Routine to free each argument.
 * \return Action return value (0 if finished normally, 1 if cancelled);
 */
int LaunchActionBlocking(const char *message, Threads *threads, uint_fast32_t nbThreadsNeeded,
				const void *args, size_t s_elem, void *(*routine)(void *),
				void (*freeArg)(void *));

/**
 * \fn int ActionIsFinished(Action *action)
 * \brief Test if action is finished.
 *
 * \param action Action to be tested.
 * \return 1 if action is finished, 0 otherwise.
 */
int ActionIsFinished(Action *action);

/**
 * \fn int GetActionResult(Action *action)
 * \brief Get action result (blocking, till action is finished).
 *
 * Calls free arguments routine on each argument to free resources.
 * Does nothing if WaitForFinished was already
 * called for this action (i.e. if threads were already joined).
 *
 * \param action Action to finish.
 * \return 0 if action finished normally, 1 if it was cancelled.
 */
int GetActionResult(Action *action);

/**
 * \fn void CancelAction(Action *action)
 * \brief Send cancellation request to action.
 *
 * It is up to the thread routine to take the request into
 * account. \see thread.h
 * Does nothing if action has already finished.
 *
 * \param action Action to send cancellation request to.
 */
void CancelAction(Action *action);

/**
 * \fn FLOAT GetActionProgress(const Action *action)
 * \brief Get action progress.
 *
 * Progress is a value between 0 (just begun) and 1 (done).
 *
 * \param action Action subject to request.
 * \return Action progress.
 */
FLOAT GetActionProgress(const Action *action);

/**
 * \fn void FreeAction(Action *action)
 * \brief Free action data.
 *
 * Action MUST have already finished : exit with error otherwise.
 *
 * \param action Action to be freed.
 */
void FreeAction(Action *action);

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

#ifdef __cplusplus
}
#endif

#endif
