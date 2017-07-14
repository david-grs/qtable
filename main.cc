#include "mainwindow.h"

#include <QApplication>

#include <boost/asio.hpp>

#include <thread>


using udp = boost::asio::ip::udp;

enum { max_length = 1024 };

void server(boost::asio::io_service& io_service, unsigned short port)
{
		udp::socket sock(io_service, udp::endpoint(udp::v4(), port));

		while (1)
		{
				char data[max_length];
				udp::endpoint sender_endpoint;
				size_t length = sock.receive_from(boost::asio::buffer(data, max_length), sender_endpoint);

				qDebug() << "receive " << length << " bytes from market";
		}
}

class udp_client
{
public:
	udp_client (boost::asio::io_service& io_service, const std::string& host, const std::string& port) :
		io_service_(io_service),
		socket_(io_service, udp::endpoint(udp::v4(), 0))
	{
		udp::resolver resolver(io_service_);
		udp::resolver::query query(udp::v4(), host, port);
		udp::resolver::iterator itr = resolver.resolve(query);

		sender_endpoint_ = *itr;
	}

	~udp_client()
	{
		socket_.close();
	}

	void recv_message()
	{
		socket_.async_receive_from(
			boost::asio::buffer(data_, max_length),
			sender_endpoint_,
			[this](const boost::system::error_code& error, size_t bytes)
			{
				if(!error)
				{
					//std::cout<<"handle_receive_from "<<std::endl;
					//std::cout<<"recv data(str):"<<show_str(data_.data(), bytes)<<std::endl;
					//if(bytes > message::header_length)
					//	data_.body_length(bytes_recvd-message::header_length);
					//std::cout<<"message.lenght()="<<data_.length()<<"bytes_recvd="<<bytes<<std::endl;
				}
				else
				{
					std::cerr<<"error in handle_receive_from:"<<error<<std::endl;
					socket_.close();
				}
			});
	}

private:
	enum {max_length = 1024};
	char data_[max_length];
	boost::asio::io_service& io_service_;
	udp::socket socket_;
	udp::endpoint sender_endpoint_;
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

