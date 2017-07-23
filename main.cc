#include "mainwindow.h"
#include "model.h"

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

	Model model(window);

	bool run = true;
	QObject::connect(&window, &MainWindow::closed, [&run]() { run = false; });

	boost::asio::io_service io_service;

	udp_server serv(io_service, 1234, [&](std::experimental::string_view str)
	{
		//std::cout << "received " << str.size() << " bytes" << std::endl;

		std::stringstream strm;
		strm.write(str.data(), str.size());

		boost::archive::binary_iarchive archive(strm);

		Instrument instr;
		archive >> instr;

		model.AddInstrument(std::move(instr));
	});

	while (run)
	{
		std::this_thread::sleep_for(std::chrono::milliseconds(1));
		app.processEvents(QEventLoop::AllEvents, 1);
		io_service.poll();
	}

	return 0;
}

