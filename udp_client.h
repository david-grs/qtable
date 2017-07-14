#pragma once

#include <boost/asio.hpp>

#include <string>

using udp = boost::asio::ip::udp;

class udp_client
{
public:
	udp_client (boost::asio::io_service& io_service, const std::string& host, short port) :
		mIOService(io_service),
		mSocket(io_service, udp::endpoint(udp::v4(), 0))
	{
		udp::resolver resolver(mIOService);
		udp::resolver::query query(udp::v4(), host, std::to_string(port));
		udp::resolver::iterator itr = resolver.resolve(query);

		mEndpoint = *itr;
	}

	~udp_client()
	{
		mSocket.close();
	}

	void send(const std::string& msg)
	{
		mSocket.send_to(boost::asio::buffer(msg, msg.size()), mEndpoint);
	}

private:
	boost::asio::io_service& mIOService;
	udp::socket mSocket;
	udp::endpoint mEndpoint;
};
