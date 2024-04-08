/*
 *  queue.c -- part of fractal2D
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
 
#include "queue.h"
#include "error.h"

void InitEmptyQueue(Queue *queue)
{
	queue->head = NULL;
	queue->tail = NULL;
	queue->size = 0;
}

inline int IsQueueEmpty(Queue *queue)
{
	return (queue->size == 0);
}

void PushQueue(Queue *queue, void *value)
{
	Cell *cell = (Cell *)malloc(sizeof(Cell));
	if (cell == NULL) {
		alloc_error("cell");
	}
	cell->value = value;
	cell->next = NULL;

	if (queue->size == 0) {
		queue->head = cell;
		queue->tail = cell;
	} else {
		queue->tail->next = cell;
		queue->tail = cell;
	}
	++queue->size;
}

void *PopQueue(Queue *queue)
{
	void *value = queue->head->value;
	Cell *oldHead = queue->head;
	queue->head = oldHead->next;
	--queue->size;
	if (queue->size == 0) {
		queue->tail = NULL;
	}

	free(oldHead);

	return value;
}

