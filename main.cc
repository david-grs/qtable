#include "mainwindow.h"
#include "udp_server.h"
#include "proto.h"

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

	udp_server serv(io_service, 1234, [&](std::experimental::string_view str)
	{
		std::cout << "received " << str.size() << " bytes" << std::endl;

		std::stringstream strm;
		strm.write(str.data(), str.size());

		boost::archive::binary_iarchive archive(strm);

		Instrument instr;
		archive >> instr;

		std::cout << "received instrument " << instr << std::endl;
	});

	while (run)
	{
		std::this_thread::sleep_for(std::chrono::milliseconds(1));
		app.processEvents(QEventLoop::AllEvents, 1);
		io_service.poll();
	}

	return 0;
}

