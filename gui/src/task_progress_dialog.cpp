/*
 *  task_progress_dialog.cpp -- part of FractalNow
 *
 *  Copyright (c) 2012 Marc Pegon <pe.marc@free.fr>
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
 
#include <QApplication>
#include <QFuture>
#include <QProgressDialog>
#include <QtConcurrentRun>
#include "task_progress_dialog.h"

int TaskProgressDialog::progress(Task *task, QString labelText, QString cancelButtonText, QWidget *parent)
{
	QProgressDialog progress(labelText, cancelButtonText, 0, 1000, parent);
	progress.setWindowModality(Qt::WindowModal);
	progress.setWindowTitle(labelText);
	progress.setMinimumDuration(0);
	progress.setValue(0);

	while (!TaskIsFinished(task)) {
		if (progress.wasCanceled()) {
			CancelTask(task);
			break;
		} else {
			progress.setValue(GetTaskProgress(task) * 1000);
		}

		QApplication::processEvents();
	}
	progress.setValue(1000);

	return GetTaskResult(task);
}

