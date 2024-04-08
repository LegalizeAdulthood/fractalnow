#ifndef __MAIN_WINDOW_H__
#define __MAIN_WINDOW_H__

#include "command_line.h"
#include "fractal.h"
#include "fractal_rendering_parameters.h"
#include "image.h"
#include "image_label.h"
#include <QCloseEvent>
#include <QFuture>
#include <QFutureWatcher>
#include <QMainWindow>
#include <QImage>
#include <QTimer>

class MainWindow : public QMainWindow
{
	Q_OBJECT

	public:
	MainWindow(int argc, char *argv[]);
	~MainWindow();
	void closeEvent(QCloseEvent * event);
	void LaunchFractalDrawing();
	void LaunchFractalAntiAliasing();

	CommandLineArguments *args;
	Fractal fractal;
	RenderingParameters render;
	Image image;
	Action action;
	ImageLabel *imageLabel;
	QImage *qimage;
	QTimer *timer;
	QFutureWatcher<int> fractalDrawingWatcher;
	QFutureWatcher<int> fractalAntialiasingWatcher;
	QFuture<int> future;
	double currentAntiAliasingSize;

	public slots:
	void FractalDrawingFinished();
	void FractalAntiAliasingFinished();

	private:
};

#endif
