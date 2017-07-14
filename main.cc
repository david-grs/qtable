#include "mainwindow.h"

#include <QApplication>

#include <boost/asio.hpp>

int main( int argc, char **argv )
{
	QApplication app(argc, argv);
	app.setApplicationName("qtable");

	MainWindow window;
	window.show();

	boost::asio::io_service io_service;


	return app.exec();
}

