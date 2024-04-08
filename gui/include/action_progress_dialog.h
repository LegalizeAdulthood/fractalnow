/*
 *  action_progress_dialog.h -- part of fractal2D
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
 
 /**
  * \file action_progress_dialog.h
  * \brief Header file introducing ActionProgressDialog class.
  *
  * \author Marc Pegon
  */

#ifndef __ACTION_PROGRESS_DIALOG_H__
#define __ACTION_PROGRESS_DIALOG_H__

#include "thread.h"
#include <QString>

class ActionProgressDialog
{
	public:
	static int progress(Action *action, QString labelText, QString cancelButtonText, QWidget *parent=0);
};

#endif
