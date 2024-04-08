/*
 *  export_fractal_image_dialog.cpp -- part of FractalNow
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

#include "ppm.h"

#include "export_fractal_image_dialog.h"
#include "task_progress_dialog.h"

#include <QApplication>
#include <QDesktopServices>
#include <QFileDialog>
#include <QFormLayout>
#include <QGridLayout>
#include <QGroupBox>
#include <QMessageBox>
#include <QProgressDialog>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QPushButton>

ExportFractalImageDialog::ExportFractalImageDialog(const FractalConfig &config,
					uint_fast32_t nbThreads, QString imageDir,
					QWidget *parent, Qt::WindowFlags f) :
	 QDialog(parent, f), 
	 config(CopyFractalConfig(&config)),
	 fractal(this->config.fractal), render(this->config.render)
{
	this->imageDir = imageDir;
	if (this->imageDir.isEmpty()) {
		this->imageDir = QDesktopServices::storageLocation(QDesktopServices::PicturesLocation);
	}

	setWindowTitle(tr("Export image"));
	QVBoxLayout *vBoxLayout = new QVBoxLayout;

	/* Image parameters (width, height, depth) .*/
	QGroupBox *imageParamBox = new QGroupBox(tr("&Image parameters"), this);
	QHBoxLayout *hBoxLayout1 = new QHBoxLayout;
	QFormLayout *formLayout = new QFormLayout;
	imageWidthBox = new QSpinBox(this);
	imageWidthBox->setRange(2, 20000);
	imageWidthBox->setValue(1280);
	imageHeightBox = new QSpinBox(this);
	imageHeightBox->setRange(2, 20000);
	imageHeightBox->setValue(1024);
	colorDepthBox = new QComboBox(this);
	colorDepthBox->addItem(tr("8 bits"));
	colorDepthBox->addItem(tr("16 bits"));
	colorDepthBox->setCurrentIndex(0);
	formLayout->addRow(tr("Width:"), imageWidthBox);
	formLayout->addRow(tr("Height:"), imageHeightBox);
	formLayout->addRow(tr("Color depth:"), colorDepthBox);
	hBoxLayout1->addLayout(formLayout);
	hBoxLayout1->addStretch(1);
	imageParamBox->setLayout(hBoxLayout1);

	/* Anti-aliasing methods */
	QGroupBox *antiAliasingBox = new QGroupBox(tr("&Anti-aliasing method"), this);
	QGridLayout *gridLayout = new QGridLayout;

	noAAMButton = new QRadioButton(tr("&None"));
	connect(noAAMButton, SIGNAL(toggled(bool)), this, SLOT(onAAMNoneToggled(bool)));

	gaussianBlurAAMButton = new QRadioButton(tr("Gaussian &blur"));
	connect(gaussianBlurAAMButton, SIGNAL(toggled(bool)), this, SLOT(onAAMBlurToggled(bool)));
	QLabel *blurRadiusLabel = new QLabel(tr("Radius:"));
	blurRadiusBox = new QDoubleSpinBox;
	blurRadiusBox->setRange(0.5, 5);
	blurRadiusBox->setValue(2);

	adaptiveAAMButton = new QRadioButton(tr("&Adaptive"));
	connect(adaptiveAAMButton, SIGNAL(toggled(bool)), this, SLOT(onAAMAdaptiveToggled(bool)));
	QLabel *adaptiveSizeLabel = new QLabel(tr("Size:"));
	adaptiveSizeBox = new QSpinBox;
	adaptiveSizeBox->setRange(2, 7);
	adaptiveSizeBox->setValue(3);

	oversamplingAAMButton = new QRadioButton(tr("&Oversampling"));
	connect(oversamplingAAMButton, SIGNAL(toggled(bool)), this, SLOT(onAAMOversamplingToggled(bool)));
	QLabel *oversamplingSizeLabel = new QLabel(tr("Size:"));
	oversamplingSizeBox = new QDoubleSpinBox;
	oversamplingSizeBox->setRange(1.5, 7);
	oversamplingSizeBox->setValue(5);

	adaptiveAAMButton->setChecked(true);

	gridLayout->addWidget(noAAMButton, 0, 0);
	gridLayout->addWidget(gaussianBlurAAMButton, 1, 0);
	gridLayout->addWidget(blurRadiusLabel, 2, 1);
	gridLayout->addWidget(blurRadiusBox, 2, 2);
	gridLayout->addWidget(adaptiveAAMButton, 3, 0);
	gridLayout->addWidget(adaptiveSizeLabel, 4, 1);
	gridLayout->addWidget(adaptiveSizeBox, 4, 2);
	gridLayout->addWidget(oversamplingAAMButton, 5, 0);
	gridLayout->addWidget(oversamplingSizeLabel, 6, 1);
	gridLayout->addWidget(oversamplingSizeBox, 6, 2);
	gridLayout->setColumnStretch(3, 1);
	gridLayout->setRowStretch(7, 1);
	antiAliasingBox->setLayout(gridLayout);

	/* Set dialog main layouts. */
	QHBoxLayout *hBoxLayout2 = new QHBoxLayout;
	hBoxLayout2->addWidget(antiAliasingBox);
	hBoxLayout2->addSpacing(20);
	hBoxLayout2->addWidget(imageParamBox);

	dialogButtonBox = new QDialogButtonBox(this);
	cancelButton = dialogButtonBox->addButton(QDialogButtonBox::Cancel);
	connect(cancelButton, SIGNAL(clicked()), this, SLOT(reject()));
	exportButton = dialogButtonBox->addButton(tr("Export"), QDialogButtonBox::AcceptRole);
	connect(exportButton, SIGNAL(clicked()), this, SLOT(exportImage()));
	
	vBoxLayout->addLayout(hBoxLayout2);
	vBoxLayout->addWidget(dialogButtonBox);

	setLayout(vBoxLayout);

	threads = CreateThreads(nbThreads);
}

ExportFractalImageDialog::~ExportFractalImageDialog()
{
	DestroyThreads(threads);
	FreeFractalConfig(config);
}

void ExportFractalImageDialog::resetFractalConfig(const FractalConfig &config)
{
	FractalConfig oldConfig = this->config;
	this->config = CopyFractalConfig(&config);
	FreeFractalConfig(oldConfig);
}

QString ExportFractalImageDialog::exportedFile()
{
	return m_exportedFile;
}

void ExportFractalImageDialog::onAAMNoneToggled(bool checked)
{
	if (checked) {
		blurRadiusBox->setEnabled(false);
		adaptiveSizeBox->setEnabled(false);
		oversamplingSizeBox->setEnabled(false);
	}
}

void ExportFractalImageDialog::onAAMBlurToggled(bool checked)
{
	if (checked) {
		blurRadiusBox->setEnabled(true);
		adaptiveSizeBox->setEnabled(false);
		oversamplingSizeBox->setEnabled(false);
	}
}

void ExportFractalImageDialog::onAAMAdaptiveToggled(bool checked)
{
	if (checked) {
		blurRadiusBox->setEnabled(false);
		adaptiveSizeBox->setEnabled(true);
		oversamplingSizeBox->setEnabled(false);
	}
}

void ExportFractalImageDialog::onAAMOversamplingToggled(bool checked)
{
	if (checked) {
		blurRadiusBox->setEnabled(false);
		adaptiveSizeBox->setEnabled(false);
		oversamplingSizeBox->setEnabled(true);
	}
}

void ExportFractalImageDialog::reInitFractal(Fractal &fractal)
{
	InitFractal(&fractal, fractal.fractalFormula, fractal.p, fractal.c,
			fractal.centerX, fractal.centerY,
			fractal.spanX, fractal.spanY, 
			fractal.escapeRadius, fractal.maxIter);
}

ExportFractalImageDialog::AntiAliasingMethod ExportFractalImageDialog::getAntiAliasingMethod() const
{
	if (noAAMButton->isChecked()) {
		return AAM_NONE;
	} else if (gaussianBlurAAMButton->isChecked()) {
		return AAM_GAUSSIANBLUR;
	} else if (oversamplingAAMButton->isChecked()) {
		return AAM_OVERSAMPLING;
	} else {
		return AAM_ADAPTIVE;
	}
}

void ExportFractalImageDialog::exportImage()
{
	int depth = colorDepthBox->currentIndex() + 1;
	QString imageFormats = (depth == 1) ? "(*.png *.jpg *.tiff *.ppm)" : "(*.ppm)";
	QString imageSuffix = (depth == 1) ? ".png" : ".ppm";
	QString fileName = QFileDialog::getSaveFileName(this, tr("Export image"),
				imageDir + "/fractal" + imageSuffix,
				tr("Images ") + imageFormats);

	/* It is not very clear in Qt documentation,
	 * but I think a file dialog cannot return
	 * an empty string if user clicks OK.
	 * (one thing for sure is that it returns a
	 * null string when user clicks Cancel)
	 */
	if (fileName.isEmpty()) {
		return;
	}

	QFileInfo fileInfo(fileName);
	QString imageFormat;
	RenderingParameters render = this->render;
	Fractal fractal = this->fractal;
	Gradient gradient16;

	if (depth == 2) {
		/* Check that extension is PPM (only format supported for 16 bits).*/
		if (fileInfo.suffix().compare(QString("PPM"), Qt::CaseInsensitive)) {
			QMessageBox::warning(this, tr("File name extension is not PPM"),
				tr("16-bits image will be exported as PPM, ignoring file extension."));
		}
		gradient16 = Gradient16(&render.gradient);
		render.gradient = gradient16;
		render.bytesPerComponent = 2;
	} else {
		/* Get image format from extension (suffix). */
		QString allowedFormats[] = {"PNG","JPG","JPEG","TIFF","PPM"};
		int nbAllowedFormats = sizeof(allowedFormats) / sizeof(QString);
		int i;
		for (i = 0; i < nbAllowedFormats; ++i) {
			if (!fileInfo.suffix().compare(allowedFormats[i], Qt::CaseInsensitive)) {
				imageFormat = allowedFormats[i];
				break;
			}
		}
		if (i == nbAllowedFormats) {
			imageFormat = allowedFormats[0];
			QMessageBox::warning(this, tr("Unknown image format"),
				tr("Could not guess image format from file name extension. Image "
				"will be exported as PNG."));
		}
	}

	/* Adjust spanX & spanY to image width and height (keep ratio). */
	int width = imageWidthBox->value();
	int height = imageHeightBox->value();
	if (height / (double)width < 1) {
		fractal.spanX = width * fractal.spanY / height;
	} else {
		fractal.spanY = height * fractal.spanX / width;
	}
	reInitFractal(fractal);

	/* Now generate fractal image according to anti-aliasing method. */
	Image fractalImg, tmpImg;
	CreateImage(&fractalImg, width, height, render.bytesPerComponent);
	Task *task;
	int canceled = 0;

	switch (getAntiAliasingMethod()) {
	case AAM_NONE: {
		task = CreateDrawFractalTask(&fractalImg, &fractal, &render,
			DEFAULT_QUAD_INTERPOLATION_SIZE, DEFAULT_COLOR_DISSIMILARITY_THRESHOLD,
			NULL, threads->N);
		LaunchTask(task, threads);

		canceled = TaskProgressDialog::progress(task, tr("Drawing fractal..."),
							tr("Abort"), this);

		break; }
	case AAM_GAUSSIANBLUR:
		CreateImage(&tmpImg, width, height, render.bytesPerComponent);
		
		task = CreateDrawFractalTask(&fractalImg, &fractal, &render,
			DEFAULT_QUAD_INTERPOLATION_SIZE, DEFAULT_COLOR_DISSIMILARITY_THRESHOLD,
			NULL, threads->N);
		LaunchTask(task, threads);
		canceled = TaskProgressDialog::progress(task, tr("Drawing fractal..."),
							tr("Abort"), this);
		if (!canceled) {
			FreeTask(task);
			task = CreateApplyGaussianBlurTask(&fractalImg, &tmpImg, &fractalImg,
							blurRadiusBox->value(), threads->N);
			LaunchTask(task, threads);
			canceled = TaskProgressDialog::progress(task,
					tr("Applying blur..."), tr("Abort"), this);
		}

		FreeImage(tmpImg);
		break;
	case AAM_OVERSAMPLING:
		CreateImage(&tmpImg, width*oversamplingSizeBox->value(),
			height*oversamplingSizeBox->value(), render.bytesPerComponent);

		task = CreateDrawFractalTask(&tmpImg, &fractal, &render,
			DEFAULT_QUAD_INTERPOLATION_SIZE, DEFAULT_COLOR_DISSIMILARITY_THRESHOLD,
			NULL, threads->N);
		LaunchTask(task, threads);
		canceled = TaskProgressDialog::progress(task, tr("Drawing fractal..."),
							tr("Abort"), this);
		if (!canceled) {
			FreeTask(task);
			task = CreateDownscaleImageTask(&fractalImg, &tmpImg, threads->N);
			LaunchTask(task, threads);
			canceled = TaskProgressDialog::progress(task, tr("Downscaling image..."),
								tr("Abort"), this);
		}

		FreeImage(tmpImg);
		break;
	case AAM_ADAPTIVE:
		task = CreateDrawFractalTask(&fractalImg, &fractal, &render,
			DEFAULT_QUAD_INTERPOLATION_SIZE, DEFAULT_COLOR_DISSIMILARITY_THRESHOLD,
			NULL, threads->N);
		LaunchTask(task, threads);
		canceled = TaskProgressDialog::progress(task, tr("Drawing fractal..."),
							tr("Abort"), this);
		if (!canceled) {
			FreeTask(task);
			task = CreateAntiAliaseFractalTask(&fractalImg, &fractal, &render,
					adaptiveSizeBox->value(), DEFAULT_ADAPTIVE_AAM_THRESHOLD,
					NULL, threads->N);
			LaunchTask(task, threads);
			canceled = TaskProgressDialog::progress(task,
					tr("Anti-aliasing fractal..."), tr("Abort"), this);
		}

		break;
	default:
		FractalNow_error("Unknown anti-aliasing method.\n");
		break;
	}
	FreeTask(task);

	if (canceled) {
		FreeImage(fractalImg);
	} else {
		setEnabled(false);
		QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));
		QApplication::processEvents();
		/* Export image. */
		int exportError = 0;
		if (depth == 2) {
			if ((exportError = ExportPPM(fileName.toStdString().c_str(), &fractalImg)) != 0) {
				QMessageBox::critical(this, tr("Failed to export image"),
					tr("Error occured while exporting image."));
			}
			FreeGradient(gradient16);
		} else {
			QImage qImage(fractalImg.data, fractalImg.width, fractalImg.height,
					QImage::Format_RGB32);
			qImage.setText("Description", tr("Fractal image generated by ")
				+ QApplication::applicationName());
			qImage.setText("Fractal formula",
				fractalFormulaDescStr[(int)fractal.fractalFormula]);
			qImage.setText("Fractal p", QString::number(crealF(fractal.p), 'G', FLOATT_DIG) + " + i*"
							+ QString::number(cimagF(fractal.p), 'G', FLOATT_DIG));
			qImage.setText("Fractal c", QString::number(crealF(fractal.c), 'G', FLOATT_DIG) + " + i*"
							+ QString::number(cimagF(fractal.c), 'G', FLOATT_DIG));
			qImage.setText("Fractal center", "(" +
							QString::number(fractal.centerX, 'G', FLOATT_DIG) +
							", " + QString::number(fractal.centerY, 'G', FLOATT_DIG) +
							")");
			qImage.setText("Fractal span X", QString::number(fractal.spanX, 'G', FLOATT_DIG));
			qImage.setText("Fractal span Y", QString::number(fractal.spanY, 'G', FLOATT_DIG));
			qImage.setText("Fractal bailout radius", QString::number(fractal.escapeRadius,
							'G', FLOATT_DIG));
			qImage.setText("Fractal max iterations", QString::number(fractal.maxIter));
			if ((exportError = !qImage.save(fileName, imageFormat.toStdString().c_str())) != 0) {
				QMessageBox::critical(this, tr("Failed to export image"),
					tr("Error occured while exporting image."));
			}
		}
		FreeImage(fractalImg);
		setEnabled(true);
		QApplication::restoreOverrideCursor();

		if (!exportError) {
			m_exportedFile = fileName;
			this->accept();
		}
	}
}

