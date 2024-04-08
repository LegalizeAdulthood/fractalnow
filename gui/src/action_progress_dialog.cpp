/*
 *  action_progress_dialog.cpp -- part of fractal2D
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
 
#include <QFuture>
#include <QProgressDialog>
#include <QtConcurrentRun>
#include "action_progress_dialog.h"

int ActionProgressDialog::progress(Action *action, QString labelText, QString cancelButtonText, QWidget *parent)
{
	QFuture<int> future(QtConcurrent::run(WaitForFinished, action));

	QProgressDialog progress(labelText, cancelButtonText, 0, 1000, parent);
	progress.setWindowModality(Qt::WindowModal);
	progress.setMinimumDuration(500);

	while (!future.isFinished()) {
		progress.setValue(GetActionProgress(action) * 1000);

		if (progress.wasCanceled()) {
			CancelAction(action);
			break;
		}
	}
	progress.setValue(1000);

	return future.result();
}

