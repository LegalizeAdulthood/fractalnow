#include "main_window.h"
#include "error.h"
#include "fractal.h"
#include <clocale>
#include <iostream>
#include <QTextCodec>
#include <QtConcurrentRun>

MainWindow::MainWindow(int argc, char *argv[])
{
	QTextCodec::setCodecForCStrings(QTextCodec::codecForName("UTF-8"));
	QTextCodec::setCodecForTr(QTextCodec::codecForName("UTF-8"));
	setlocale(LC_NUMERIC, "C");

	args = new CommandLineArguments(argc, argv);
	if (!args->fractalFileName || !args->renderingFileName) {
		/* Build default fractal and rendering parameters */
		
		/* Build default fractal */
		InitFractal2(&fractal, FRAC_MANDELBROT, 2, 0, -0.7, 0, 3, 3, 1E3, 250);
		
		/* Generate gradient */
		uint32_t hex_color[5] = { 0xFF, 0xFFFFFF, 0xFFFF00, 0xFF0000, 0xFF };
		Color color[5];
		for (uint_fast32_t i = 0; i < 5; ++i) {
			color[i] = ColorFromUint32(hex_color[i]);
		}
		Gradient gradient;
		GenerateGradient(&gradient, color, 5, 10000);

		std::string identityName = "identity";
		std::string smoothName = "smooth";
		/* Init rendering parameters */
		InitRenderingParameters(&render, 1, ColorFromUint32(0x0), GetCountingFunction(
					smoothName.c_str()), CM_SIMPLE, AF_TRIANGLEINEQUALITY,
					IM_NONE, GetTransferFunction(identityName.c_str()),
					1E3, 0, gradient);
	} else {
		ReadFractalFile(&fractal, args->fractalFileName);
		ReadRenderingFile(&render, args->renderingFileName);
		if (render.bytesPerComponent != 1) {
			error("Rendering files with more than 1 byte per component are not accepted.\n");
		}
	}
	if (args->width == 0) {
		args->width = roundF(fractal.spanX * args->height / fractal.spanY);
	} else if (args->height == 0) {
		args->height = roundF(fractal.spanY * args->width / fractal.spanX);
	}
	CreateImage(&image, args->width, args->height, 1);
	qimage = new QImage(image.data, args->width, args->height, args->width*3, QImage::Format_RGB888);
	imageLabel = new ImageLabel;
	imageLabel->setImage(qimage);
	this->setCentralWidget(imageLabel);

	LaunchFractalDrawing();

	connect(&fractalDrawingWatcher, SIGNAL(finished()), this, SLOT(FractalDrawingFinished()));
	connect(&fractalAntialiasingWatcher, SIGNAL(finished()), this, SLOT(FractalAntiAliasingFinished()));

	timer = new QTimer(this);
	connect(timer, SIGNAL(timeout()), imageLabel, SLOT(update()));
	timer->start(100);

	this->resize(qimage->size());
}

MainWindow::~MainWindow()
{
	delete args;
	FreeRenderingParameters(&render);
	FreeImage(&image);
}

void MainWindow::LaunchFractalDrawing()
{
	action = LaunchDrawFractalFast(&image, &fractal, &render, DEFAULT_QUAD_INTERPOLATION_SIZE,
				DEFAULT_COLOR_DISSIMILARITY_THRESHOLD);

	future = QtConcurrent::run(WaitForActionTermination, action);
	fractalDrawingWatcher.setFuture(future);
}

void MainWindow::LaunchFractalAntiAliasing()
{
	action = LaunchAntiAliaseFractal(&image, &fractal, &render, currentAntiAliasingSize, DEFAULT_ADAPTIVE_AAM_THRESHOLD);

	future = QtConcurrent::run(WaitForActionTermination, action);
	fractalAntialiasingWatcher.setFuture(future);
}

void MainWindow::closeEvent(QCloseEvent * event)
{
	if (!future.isFinished()) {
		CancelAction(action); /* Send cancelation request to drawing/antialiasing action.*/
		int res = future.result(); /* Wait for it to finish. */
		(void)res;
	}

	event->accept();
}

void MainWindow::FractalDrawingFinished()
{
	std::cout << "Drawing fractal : ";
	if (future.result() == 0) {
		std::cout << "DONE.\n";
		currentAntiAliasingSize = args->minAntiAliasingSize;
		LaunchFractalAntiAliasing();
	} else {
		std::cout << "CANCELED.\n";
		//LaunchFractalDrawing();
	}
}

void MainWindow::FractalAntiAliasingFinished()
{
	std::cout << "Anti-aliasing fractal : ";
	if (future.result() == 0) {
		std::cout << "DONE.\n";
		if (currentAntiAliasingSize < args->maxAntiAliasingSize) {
			currentAntiAliasingSize++;
			LaunchFractalAntiAliasing();
		}
	} else {
		std::cout << "CANCELED.\n";
		//LaunchFractalDrawing();
	}
}

