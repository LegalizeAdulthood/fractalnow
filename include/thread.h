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

#include <errno.h>
#include <pthread.h>
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
#define safePThreadCondWait(cond,mutex) {if((errno=pthread_cond_wait(cond,mutex))!=0){perror(THREAD_COND_WAIT_ERROR);pthread_exit(THREAD_ENDED_WITH_ERROR);}}
#define safePThreadCondSignal(cond) {if((errno=pthread_cond_signal(cond))!=0){perror(THREAD_COND_SIGNAL_ERROR);pthread_exit(THREAD_ENDED_WITH_ERROR);}}
#define safePThreadCondBroadcast(cond) {if((errno=pthread_cond_broadcast(cond))!=0){perror(THREAD_COND_BROADCAST_ERROR);pthread_exit(THREAD_ENDED_WITH_ERROR);}}
#define safePThreadCondDestroy(cond) {if((errno=pthread_cond_destroy(cond))!=0){perror(THREAD_COND_DESTROY_ERROR);pthread_exit(THREAD_ENDED_WITH_ERROR);}}

/**
 * \fn void LaunchThreadsAndWait(uint_fast32_t nbThreads, void *args, size_t s_elem, void *(*routine)(void *))
 * \brief Create several threads and wait for them to end.
 *
 * \param nbThreads Number of threads to create.
 * \param args Pointer to array of arguments for threads routines.
 * \param s_elem Size of one argument (in bytes).
 * \param routine Threads routine.
 */
void LaunchThreadsAndWait(uint_fast32_t nbThreads, void *args, size_t s_elem, void *(*routine)(void *));

#endif
