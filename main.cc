#include "mainwindow.h"

#include <QApplication>

#include <boost/asio.hpp>

#include <thread>

int main( int argc, char **argv )
{
	QApplication app(argc, argv);
	app.setApplicationName("qtable");

	MainWindow window;
	window.show();

	bool run = true;
	QObject::connect(&window, &MainWindow::closed, [&run]() { run = false; });

	boost::asio::io_service io_service;
	while (run)
	{
		std::this_thread::sleep_for(std::chrono::milliseconds(1));
		app.processEvents(QEventLoop::AllEvents, 1);
	}

	return 0;
}

