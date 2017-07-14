#pragma once

#include <boost/asio.hpp>

using udp = boost::asio::ip::udp;

class udp_server
{
public:
	udp_server(boost::asio::io_service& io_service, short port) :
		mIOService(io_service),
		mSocket(io_service, udp::endpoint(udp::v4(), port))
	{
		async_receive();
	}

private:
	void async_receive()
	{
		mSocket.async_receive_from(
			boost::asio::buffer(mData, MaxLength),
			mEndpoint,
			[this](const boost::system::error_code& error, size_t bytes)
		{
			if (error)
			{
				mSocket.close();
				return;
			}

			if (bytes > 0)
				std::cout << "received " << bytes <<  " bytes" << std::endl;

			async_receive();
		});
	}

	boost::asio::io_service& mIOService;
	udp::socket mSocket;
	udp::endpoint mEndpoint;

	enum { MaxLength = 1024 };
	char mData[MaxLength];
};
