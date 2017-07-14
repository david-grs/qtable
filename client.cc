#include "udp_client.h"

#include <thread>

int main()
{
	boost::asio::io_service io_service;
	udp_client client(io_service, "localhost", 1234);

	while (1)
	{
		std::this_thread::sleep_for(std::chrono::milliseconds(1));
		client.send("foo");
	}

}
