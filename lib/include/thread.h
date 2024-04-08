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

#include <errno.h>
#include <pthread.h>
#include <signal.h>
#include <stdint.h>
#include <unistd.h>

/**
 * \def DEFAULT_NB_THREADS
 * \brief Default number of threads.
 */
#define DEFAULT_NB_THREADS (uint_fast32_t)(16)

/**
 * \var nbThreads.
 * \brief Number of threads to be used by parallelized functions.
 *
 * Unless changed explicitly, this variable will be equal to
 * DEFAULT_NB_THREADS.
 */
extern uint_fast32_t nbThreads;

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
#define safePThreadMutexInit(mutex,attributes) {if((errno=pthread_mutex_init(mutex,attributes))!=0){perror(THREAD_MUTEX_INIT_ERROR);pthread_exit(THREAD_ENDED_WITH_ERROR);}}
#define safePThreadMutexLock(mutex) {if((errno=pthread_mutex_lock(mutex))!=0){perror(THREAD_MUTEX_LOCK_ERROR);pthread_exit(THREAD_ENDED_WITH_ERROR);}}
#define safePThreadMutexUnlock(mutex) {if((errno=pthread_mutex_unlock(mutex))!=0){perror(THREAD_MUTEX_UNLOCK_ERROR);pthread_exit(THREAD_ENDED_WITH_ERROR);}}
#define safePThreadMutexDestroy(mutex) {if((errno=pthread_mutex_destroy(mutex))!=0){perror(THREAD_MUTEX_DESTROY_ERROR);exit(EXIT_FAILURE);}}
#define safePThreadCondInit(cond,attributes) {if((errno=pthread_cond_init(cond,attributes))!=0){perror(THREAD_COND_INIT_ERROR);exit(EXIT_FAILURE);}}
#define safePThreadCancel(thread) {if((errno=pthread_cancel(thread))!=0){perror(THREAD_CANCEL_ERROR);exit(EXIT_FAILURE);}}
#define safePThreadCondWait(cond,mutex) {if((errno=pthread_cond_wait(cond,mutex))!=0){perror(THREAD_COND_WAIT_ERROR);pthread_exit(THREAD_ENDED_WITH_ERROR);}}
#define safePThreadCondSignal(cond) {if((errno=pthread_cond_signal(cond))!=0){perror(THREAD_COND_SIGNAL_ERROR);pthread_exit(THREAD_ENDED_WITH_ERROR);}}
#define safePThreadCondBroadcast(cond) {if((errno=pthread_cond_broadcast(cond))!=0){perror(THREAD_COND_BROADCAST_ERROR);pthread_exit(THREAD_ENDED_WITH_ERROR);}}
#define safePThreadCondDestroy(cond) {if((errno=pthread_cond_destroy(cond))!=0){perror(THREAD_COND_DESTROY_ERROR);pthread_exit(THREAD_ENDED_WITH_ERROR);}}

/**
 * \struct s_Action
 * \brief Action created when launching simultaneous threads.
 *
 * There are two choices when launching threads :
 * - use a function that launches threads and then waits for
 *   them to terminate
 * - use a function that launches threads and returns an 
 *   action structure
 * Through the action structure, it is possible to cancel
 * the threads (cancel action) or to wait for them to 
 * terminate (wait for action termination).
 *
 * Note that when an action is returned by the LaunchThreads function,
 * sooner or later the function WaitForActionTermination
 * (or CancelAndWaitForActionTermination) must be called (on this
 * action structure) in order to free resources.
 * Before returning, WaitForActionTermination (and other function
 * that wait for termination) frees each argument it has copied
 * using the specified freeArg routine.
 *
 * Examples of use :
 * 1) Launch threads and wait for them to terminate :
 * res = LaunchThreadsAndWaitForTermination(N, args, s_elem, routine);
 *
 * 2) Launch threads, cancel action after some time, wait for it to
 *    terminate.
 * action = LaunchThreads(N, args, s_elem, routine); // launch threads
 * sleep(2);
 * CancelAction(action); // cancel action (send a request, actually)
 * res = WaitForActionToTerminate(action); // wait for it to terminate
 *
 * It is up to the thread routine to take into account cancelling
 * cancelling request and terminate earlier.
 * The request is sent through the argument passed to the routine :
 * the first sizeof(sig_atomic_t *) bytes store a pointer to
 * a sig_atomic_t which changes to 1 when a cancelation request is made.
 * The remaining part of the argument is the 'real' argument (of size
 * s_elem) that was passed to LaunchThreads.
 *   
 */
typedef struct s_Action {
	int is_do_nothing;
 /*!< 1 if action is to do nothing (convenient).*/
	int done;
 /*!< 1 if action has already terminated and threads have been joined.*/
	int return_value;
 /*!< Action return value (0 if finished normally, 1 if canceled).*/
	sig_atomic_t cancel;
 /*!< uint8_t used for cancelation request.*/
 	uint_fast32_t N;
 /*!< Number of threads.*/
 	pthread_t *thread;
 /*!< Threads array.*/
 	void *args;
 /*!< Array of args passed to thread routine.*/
	size_t s_elem;
 /*!< Size of each argument.*/
	void (*freeArg)(void *);
 /*!< Routine to free each arg.*/
	char *message;
 /*!< Action message to print at launch and termination.*/
} Action;

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
 * \fn Action *LaunchThreads(const char *message, uint_fast32_t N, void *args, size_t s_elem, void *(*routine)(void *), void (*freeArg)(void *))
 * \brief Create several threads..
 *
 * args is copied.
 * Note that arguments passed to the thread routine are not
 * exactly those passed to the function :
 * A pointer to a sig_atomic_t is added at the beginning of the
 * argument, which should be used to handle cancelation requests.
 * The sig_atomic_t pointed by it is changed to 1 if a cancelation
 * request was made.
 * The remaining bytes are the 'real' argument.
 * Free argument routine will be called for each argument on
 * termination to free its data (can be NULL if arguments contain
 * no dynamically allocated data).
 *
 * \param message Action message to print at launch and termination.
 * \param N Number of threads to create.
 * \param args Pointer to array of arguments for threads routines.
 * \param s_elem Size of one argument (in bytes).
 * \param routine Threads routine.
 * \param freeArg Routine to free each argument.
 * \return Corresponding newly-allocated action.
 */
Action *LaunchThreads(const char *message, uint_fast32_t N, void *args, size_t s_elem,
			void *(*routine)(void *), void (*freeArg)(void *));

/**
 * \fn int WaitForActionTermination(Action *action)
 * \brief Wait for action to terminate (normally of after cancelation).
 *
 * Does nothing if WaitForActionTermination was already
 * called for this action (i.e. if threads were already joined).
 *
 * \param action Action to wait for termination.
 * \return 0 if all threads terminated normally, 1 otherwise (at least one canceled).
 */
int WaitForActionTermination(Action *action);

/**
 * \fn int LaunchThreadsAndWaitForTermination(const char *message, uint_fast32_t N, void *args, size_t s_elem, void *(*routine)(void *), void (*freeArg)(void *))
 * \brief Create several threads and wait for them to end.
 *
 * args array is copied. If it was dynamically allocated by
 * caller, do not forget to free it (it can be freed directly
 * after launching threads, since it is copied).
 * Note that arguments passed to the thread routine are not
 * exactly those passed to the function :
 * A pointer to a sig_atomic_t is added at the beginning of the
 * argument, which should be used to handle cancel requests.
 * The sig_atomic_t pointed by it is changed to 1 if a cancel
 * request was made.
 * The remaining bytes are the 'real' argument.
 * Free argument routine will be called for each argument on
 * termination.
 *
 * \param message Message to be printed at threads launch and termination.
 * \param N Number of threads to create.
 * \param args Pointer to array of arguments for thread routines.
 * \param s_elem Size of one argument (in bytes).
 * \param routine Threads routine.
 * \param freeArg Routine to free each argument.
 * \return 0 if all threads terminated normally, 1 otherwise (at least one canceled).
 */
int LaunchThreadsAndWaitForTermination(const char *message, uint_fast32_t N, void *args, size_t s_elem,
					void *(*routine)(void *), void (*freeArg)(void *));

/**
 * \fn void CancelAction(Action *action)
 * \brief Send cancelation request to action.
 *
 * It is up to the thread routine to take the request into
 * account. \see thread.h
 * Does nothing if threads have already finished
 * (whether WaitForActionTermination has already been
 * called or not).
 *
 * \param action Action to send cancelation request to.
 */
void CancelAction(Action *action);

/**
 * \fn int CancelActionAndWaitForTermination(Action *action)
 * \brief Send cancelation request to action and wait for it to terminate.
 *
 * \param action Action to be canceled.
 * \return 0 if all threads terminated normally, 1 otherwise (at least one canceled).
 */
int CancelActionAndWaitForTermination(Action *action);

/**
 * \fn void FreeAction(Action action)
 * \brief Free action data.
 *
 * Action MUST have already terminated (i.e. WaitForActionTermination must 
 * already have been called) : exit with error otherwise.
 *
 * \param action Action to be freed.
 */
void FreeAction(Action action);

#ifdef __cplusplus
}
#endif

#endif
