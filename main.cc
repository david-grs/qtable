#include "mainwindow.h"
#include "udp_server.h"

#include <QApplication>

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
	udp_server serv(io_service, 1234);

	while (run)
	{
		std::this_thread::sleep_for(std::chrono::milliseconds(1));
		app.processEvents(QEventLoop::AllEvents, 1);
		io_service.poll();
	}

	return 0;
}

