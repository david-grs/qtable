#include "udp_client.h"
#include "proto.h"

#include <thread>

void send_instrument_definitions(udp_client& client, const std::vector<Instrument>& instruments)
{
	for (const Instrument& instr : instruments)
	{
		std::string str = proto::serialize(instr);
		client.send(str);
	}
}

int main()
{
	boost::asio::io_service io_service;
	udp_client client(io_service, "localhost", 1234);

	std::vector<Instrument> instruments;

	while (1)
	{
		std::this_thread::sleep_for(std::chrono::milliseconds(1));
		send_instrument_definitions(client, instruments);
	}

	return 0;
}
