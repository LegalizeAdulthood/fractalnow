/*
 *  queue.h -- part of fractal2D
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
  * \file queue.h
  * \brief Header file for a basic queue type.
  * \author Marc Pegon
  */

#ifndef __QUEUE_H__
#define __QUEUE_H__

#include <stdint.h>

/**
 * \struct s_Cell
 * \brief A queue cell
 */
typedef struct s_Cell {
	void *value;
 /*!< Pointer to the value of the cell.*/
	struct s_Cell *next;
 /*!< Pointer to the next cell in the queue.*/
} Cell;

/**
 * \struct s_Queue
 * \brief Basic queue structure.
 */
typedef struct s_Queue {
	Cell *head;
 /*!< Queue head.*/
	Cell *tail;
 /*!< Queue tail.*/
	uint32_t size;
 /*!< Queue size (number of cells).*/
} Queue;

/**
 * \fn void InitEmptyQueue(Queue *queue)
 * \brief Initialize an empty queue.
 *
 * \param queue Pointer to the queue to initialize.
 */
void InitEmptyQueue(Queue *queue);

/**
 * \fn int IsQueueEmpty(Queue *queue)
 * \brief Check whether the queue is empty or not.
 *
 * \param queue Pointer to the queue to check for emptiness.
 */
int IsQueueEmpty(Queue *queue);

/**
 * \fn void PushQueue(Queue *queue, void *value)
 * \brief Push an element at the end of the queue.
 *
 * \param queue Pointer to the queue in which to add the element.
 * \param value Value to add to the queue.
 */
void PushQueue(Queue *queue, void *value);

/**
 * \fn void *PopQueue(Queue *queue)
 * \brief Pop the queue head.
 *
 * This function is NOT safe : the queue must NOT be empty.
 *
 * \param queue Queue to pop the head from.
 * \return Return a pointer to the value in the head of the queue.
 */
void *PopQueue(Queue *queue);

#endif
