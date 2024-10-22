/*
 *  export_fractal_image_dialog.h -- part of FractalNow
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
  * \file export_fractal_image_dialog.h
  * \brief Header file introducing ExportFractalImageDialog class.
  *
  * \author Marc Pegon
  */

#ifndef __EXPORT_FRACTAL_IMAGE_DIALOG_H__
#define __EXPORT_FRACTAL_IMAGE_DIALOG_H__

#include <QDialog>
#include <QComboBox>
#include <QDialogButtonBox>
#include <QDoubleSpinBox>
#include <QLineEdit>
#include <QRadioButton>
#include <QSpinBox>

#include "fractalnow.h"

class ExportFractalImageDialog : public QDialog
{
	Q_OBJECT

	public:
	ExportFractalImageDialog(const FractalConfig &config,
					uint_fast32_t nbThreads,
					QString imageDir = QString(),
					QWidget *parent = 0, Qt::WindowFlags f = 0);
	void resetFractalConfig(const FractalConfig &config);
	void setFloatPrecision(FloatPrecision floatPrecision);
	QString exportedFile();
	~ExportFractalImageDialog();

	private:
	void TaskProgressBar(Task *task, QString labelText, QString cancelButtonText);

	enum AntiAliasingMethod {
		AAM_NONE = 0,
		AAM_GAUSSIANBLUR,
		AAM_OVERSAMPLING,
		AAM_ADAPTIVE
	};
	AntiAliasingMethod getAntiAliasingMethod() const;

	QString m_exportedFile;
	QString imageDir;
	FractalConfig config;
	Fractal &fractal;
	RenderingParameters &render;
	Threads *threads;
	FloatPrecision floatPrecision;
	QComboBox *colorDepthBox;
	QLineEdit *outputFileEdit;
	QRadioButton *noAAMButton;
	QRadioButton *gaussianBlurAAMButton;
	QDoubleSpinBox *blurRadiusBox;
	QRadioButton *adaptiveAAMButton;
	QSpinBox *adaptiveSizeBox;
	QRadioButton *oversamplingAAMButton;
	QDoubleSpinBox *oversamplingSizeBox;
	QSpinBox *imageWidthBox;
	QSpinBox *imageHeightBox;
	QDialogButtonBox *dialogButtonBox;
	QPushButton *exportButton;
	QPushButton *cancelButton;

	private slots:
	void onAAMNoneToggled(bool);
	void onAAMBlurToggled(bool);
	void onAAMAdaptiveToggled(bool);
	void onAAMOversamplingToggled(bool);
	void exportImage();
};

#endif
