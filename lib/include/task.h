/*
 *  task.h -- part of FractalNow
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
  * \file task.h
  * \brief Header file related to tasks.
  * \author Marc Pegon
  */

#ifndef __TASKS_H__
#define __TASKS_H__

#ifdef __cplusplus
extern "C" {
#endif

#include "thread.h"
#include <signal.h>

struct Task;
/**
 * \struct CompositeTaskArguments
 * \brief Arguments structure for composite tasks thread routine.
 *
 * This is for internal use only.
 */
/**
 * \typedef CompositeTaskArguments
 * \brief Convenient typedef for struct CompositeTaskArguments.
 */
typedef struct CompositeTaskArguments {
	struct Task *thisTask;
 /*!< Pointer to the composite task being run.*/
} CompositeTaskArguments;

/**
 * \struct Task
 * \brief Structure to control tasks execution by threads.
 *
 * Typically, executing a task by threads is done in 3
 * steps:
 *   -create task
 *   -launch task
 *   -wait for it to finish
 *
 * Through the task structure, it is possible to cancel
 * a task, to pause it and then resume it, as well to wait for
 * it to finish (get task result).
 *
 * Some examples of use :\n
 * 1) Launch task and wait for it to finish :\n
 * task = CreateTask(message, threads, nbThreadsNeeded,
 *		args, s_elem, routine, freeArgs);\n
 * LaunchTask(task, threads);\n
 * res = GetTaskResult(task);
 *
 * 2) Launch threads, cancel task after some time, wait for it to
 *    finish.\n
 * task = CreateTask(message, threads, nbThreadsNeeded, args,
		s_elem, routine, freeArgs);\n
 * sleep(2);\n
 * CancelTask(task); // (send cancelation request)\n
 * res = GetTaskResult(task); // wait for it to finish\n
 *
 * It is up to the thread routine to take into account cancellation
 * request and finish earlier, via the CancelRequested function.
 *
 * Remark: note that although the functions described here deal
 * with threads, they are not thread-safe (like all the functions
 * in this library btw), i.e. the functions here should not be run
 * concurrently by different threads (although, if necessary, it
 * would be quite easy to make them thread-safe, by adding a mutex
 * for each function).
 */
/**
 * \typedef Task
 * \brief Convenient typedef for struct Task.
 */
typedef struct Task {
	int isComposite;
 /*!< 1 if task consists of a sequence of subtasks.*/
	int launchPrepared;
 /*!< 1 if launch has been prepared (for composite tasks).*/
	int hasBeenLaunched;
 /*!< 1 if created task has already been launched.*/
	int done;
 /*!< 1 if task has already finished and threads have been joined.*/
	int returnValue;
 /*!< Task return value (0 if finished normally, 1 if canceled).*/
	int cancel;
 /*!< Integer used for cancellation request.*/
	pthread_spinlock_t cancelMutex;
 /*!< Mutex for cancel variable.*/
	int pause;
 /*!< Integer used to pause task.*/
	pthread_spinlock_t pauseMutex;
 /*!< Mutex for pause variable.*/

 /* For composite tasks: */
	CompositeTaskArguments compositeTaskArguments;
 /*!< Arguments for composite tasks.*/
	uint_fast32_t nbSubTasks;
 /*!< Number of subtasks (for composite task only).*/
	struct Task **subTasks;
 /*!< Subtasks (for composite task only).*/
	uint_fast32_t compositeTaskNbReady;
 /*!< Number of threads ready to launch a new subtask.*/
	int stopLaunchingSubTasks;
 /*!< Instructs composite task routine to stop launching subtasks (cancel requested).*/
	pthread_cond_t compositeTaskAllThreadsReadyCond;
 /*!< Signaled when all threads are ready to launch new subtask.*/

 /* For non-composite tasks: */
	struct ThreadArgHeader *threadArgsHeaders;
 /*!< Internal part of thread arguments (cancel and progress handling).*/
	struct Threads *threads;
 /*!< Pointer to threads to use for task.*/
	uint_fast32_t nbThreadsNeeded;
 /*!< Number of threads actually needed for task.*/
	void *initialArgs;
 /*!< Copy of initial arguments (as passed to CreateTask).*/
 	void *args;
 /*!< Array of args passed to thread routine (with headers).*/
	size_t s_elem;
 /*!< Size of each argument.*/
	void *(*threadsRoutine)(void *);
 /*!< Task routine (to be executed by each thread).*/
	void (*freeArg)(void *);
 /*!< Routine to free each arg.*/
	char *message;
 /*!< Task message to print at launch and when it finishes.*/
} Task;

/**
 * \fn Task *DoNothingTask()
 * \brief Returns a newly-allocated task that does nothing.
 *
 * Convenient for working on empty image for example.
 *
 * \return Do-nothing task.
 */
Task *DoNothingTask();

/**
 * \fn Task *CreateTask(const char message[], uint_fast32_t nbThreadsNeeded, const void *args, size_t s_elem, void *(*routine)(void *), void (*freeArg)(void *))
 * \brief Create task.
 *
 * args is copied.\n
 * Note that arguments passed to the thread routine are not
 * exactly those passed to the function :\n
 * A ThreadArgHeader structure is added at the beginning of the
 * argument, which should be used to handle cancellation requests and
 * task progress.\n
 * The remaining bytes are the 'real' argument.\n
 * Free argument routine will be called for each argument when task
 * is free'd (can be NULL if arguments contain no dynamically
 * allocated data).
 *
 * \param message Task message to print at launch and when task finishes.
 * \param nbThreadsNeeded Number of threads needed to launch task.
 * \param args Pointer to array of arguments for threads routines.
 * \param s_elem Size of one argument (in bytes).
 * \param routine Threads routine.
 * \param freeArg Routine to free each argument.
 * \return Corresponding newly-allocated task.
 */
Task *CreateTask(const char message[], uint_fast32_t nbThreadsNeeded,
			const void *args, size_t s_elem, void *(*routine)(void *),
			void (*freeArg)(void *));

/**
 * \fn Task *CreateCompositeTask(const char message[], uint_fast32_t nbSubTasks, Task *subTasks[])
 * \brief Create composite task.
 *
 * Create a "virtual" task made of subtasks.\n
 * Array of subtasks pointers will be copied (though not the tasks
 * themselves).
 *
 * \param message Task message to print at launch and when task finishes.
 * \param nbSubTasks Number of subtasks.
 * \param subTasks Array of subtasks.
 * \return Newly-allocated composite task.
 */
Task *CreateCompositeTask(const char message[], uint_fast32_t nbSubTasks, Task *subTasks[]);

/**
 * \fn void LaunchTask(Task *task, Threads *threads)
 * \brief Launch task.
 *
 * Exit with error if task has already been launched.\n
 * Exit with error if number of threads needed for task
 * is greater than number of threads.\n
 * Threads should *not* be busy when calling this
 * function (unspecified behaviour otherwise).
 *
 * \param task Task to be launched.
 * \param threads Threads that will execute task.
 */
void LaunchTask(Task *task, Threads *threads);

/**
 * \fn int ExecuteTaskBlocking(Task *task, Threads *threads)
 * \brief Execute task (blocking).
 *
 * Launch task and wait for it finish before returning.
 * \see LaunchTask
 *
 * \param task Task to be executed.
 * \param threads Threads that will be used to execute task.
 * \return Task return value (0 if finished normally, 1 if cancelled);
 */
int ExecuteTaskBlocking(Task *task, Threads *threads);

/**
 * \fn int TaskIsFinished(Task *task)
 * \brief Test if task is finished.
 *
 * A not-yet-launched task is considered not finished
 * (0 returned).
 *
 * \param task Task to be tested.
 * \return 1 if task is finished, 0 otherwise.
 */
int TaskIsFinished(Task *task);

/**
 * \fn int GetTaskResult(Task *task)
 * \brief Get task result.
 *
 * Exit with error if task has not been launched yet.\n
 * If task has already been launched but is not finished,\n
 * the function will block until it is finished.\n
 * Hence make sure that task is not paused when calling\n
 * this function (deadlock).
 *
 * \param task Task to finish.
 * \return 0 if task finished normally, 1 if it was cancelled, -1 if it has not yet been launched.
 */
int GetTaskResult(Task *task);

/**
 * \fn void CancelTask(Task *task)
 * \brief Send cancelation request to task.
 *
 * Note that the function returns immediately after
 * sending the cancelation request.
 * You can call GetTaskResult after CancelTask to
 * make sure it is effectively canceled.\n
 * It is up to the task routine to take the request into
 * account, via the CancelRequested function.\n
 * \see CancelRequested
 * Does nothing if task has already finished.\n
 *
 * \param task Task to send cancellation request to.
 */
void CancelTask(Task *task);

/** \fn void PauseTask(Task *task)
 * \brief Pause task.
 *
 * Function returns after task has effectively been paused
 * (unlike CancelAction which only sends a request).\n
 * You can use this function to access data being modified
 * by task safely.\n
 * Does nothing if task is already paused, has not been launched
 * yet, or has already finished.
 *
 * \brief task Task to be paused.
 */
void PauseTask(Task *task);

/** \fn void ResumeTask(Task *task)
 * \brief Resume task.
 *
 * Resume task after it has been paused.\n
 * Does nothing if task has not been launched yet,
 * or has already finished.
 */
void ResumeTask(Task *task);

/**
 * \fn FLOATT GetTaskProgress(const Task *task)
 * \brief Get task progress.
 *
 * Progress is a value between 0 (just begun) and 1 (done).\n
 * Returns 0 if task has not been launched yet.
 *
 * \param task Task subject to request.
 * \return Task progress.
 */
FLOATT GetTaskProgress(const Task *task);

/**
 * \fn void FreeTask(Task *task)
 * \brief Free task data.
 *
 * If task has been launched, it *must* have already finished:
 * exit with error otherwise.\n
 * Note: a prior call to GetTaskResult will ensure that task
 * has finished, since it waits for action to finish.
 *
 * \param task Task to be free'd.
 */
void FreeTask(Task *task);

#ifdef __cplusplus
}
#endif

#endif
