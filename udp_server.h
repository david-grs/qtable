#pragma once

#include <boost/asio.hpp>

#include <experimental/string_view>

using udp = boost::asio::ip::udp;

class udp_server
{
public:
	using PacketHandler = std::function<void(std::experimental::string_view)>;

	udp_server(boost::asio::io_service& io_service, short port, PacketHandler handler) :
		mIOService(io_service),
		mSocket(io_service, udp::endpoint(udp::v4(), port)),
		mHandler(handler)
	{
		async_receive();
	}

private:
	void async_receive()
	{
		mSocket.async_receive_from(
			boost::asio::buffer(mData, MaxLength),
			mEndpoint,
			[this](const boost::system::error_code& error, std::size_t bytes)
		{
			if (error)
			{
				mSocket.close();
				return;
			}

			mHandler(std::experimental::string_view(mData, bytes));

			//if (bytes > 0)
			//	std::cout << "received " << bytes <<  " bytes" << std::endl;

			async_receive();
		});
	}

	boost::asio::io_service& mIOService;
	udp::socket mSocket;
	udp::endpoint mEndpoint;

	PacketHandler mHandler;

	enum { MaxLength = 1024 };
	char mData[MaxLength];
};
