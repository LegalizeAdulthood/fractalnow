#include "main.h"
#include "main_window.h"
#include <QApplication>
#include <QPushButton>
 
int main(int argc, char *argv[])
{
	QApplication app(argc, argv);

	MainWindow mainWindow(argc, argv);
	mainWindow.show();

	return app.exec();
}

