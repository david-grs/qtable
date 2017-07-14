#include "mainwindow.h"

#include <QApplication>

#include <boost/asio.hpp>

#include <thread>

using udp = boost::asio::ip::udp;

class udp_client
{
public:
	udp_client (boost::asio::io_service& io_service, const std::string& host, const std::string& port) :
		mIOService(io_service),
		mSocket(io_service, udp::endpoint(udp::v4(), 0))
	{
		udp::resolver resolver(mIOService);
		udp::resolver::query query(udp::v4(), host, port);
		udp::resolver::iterator itr = resolver.resolve(query);

		mEndpoint = *itr;
	}

	~udp_client()
	{
		mSocket.close();
	}

	void recv_message()
	{
		mSocket.async_receive_from(
			boost::asio::buffer(mData, MaxLength),
			mEndpoint,
			[this](const boost::system::error_code& error, size_t bytes)
			{
				if(!error)
				{
					qDebug() << "received udp data, bytes=" << bytes;
				}
				else
				{
					qDebug() << "error while receiving udp data, reason=" << error;
					mSocket.close();
				}
			});
	}

private:
	enum { MaxLength = 1024 };
	char mData[MaxLength];
	boost::asio::io_service& mIOService;
	udp::socket mSocket;
	udp::endpoint mEndpoint;
};


int main( int argc, char **argv )
{
	QApplication app(argc, argv);
	app.setApplicationName("qtable");

	MainWindow window;
	window.show();

	bool run = true;
	QObject::connect(&window, &MainWindow::closed, [&run]() { run = false; });

	boost::asio::io_service io_service;
	udp_client(io_service, "localhost", "1234");
	while (run)
	{
		std::this_thread::sleep_for(std::chrono::milliseconds(1));
		app.processEvents(QEventLoop::AllEvents, 1);
		io_service.poll_one();
	}

	return 0;
}

