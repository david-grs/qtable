#include "udp_server.h"
#include "udp_client.h"
#include "proto.h"

int main( int argc, char **argv )
{
	boost::asio::io_service io_service;

	std::vector<InstrumentDefinition> instruments;

	udp_server serv(io_service, 1234, [&](std::experimental::string_view str)
	{
		//std::cout << "received " << str.size() << " bytes" << std::endl;

		std::stringstream strm;
		strm.write(str.data(), str.size());

		boost::archive::binary_iarchive archive(strm);

		InstrumentDefinition instr;
		archive >> instr;

		instruments.push_back(std::move(instr));
	});

	udp_client price_feed(io_service, "localhost", 1235);

	while (true)
	{
		std::this_thread::sleep_for(std::chrono::milliseconds(1));
		io_service.poll();

		send_price();
	}

	return 0;
}



int main()
{
	boost::asio::io_service io_service;
	udp_client client(io_service, "localhost", 1234);

	std::vector<InstrumentDefinition> instruments = load("secdef.dat");

	std::vector<std::string> messages = serialize_instruments(instruments);

	//std::cout << "ctor=" << Tracker<Instrument>::ctor << " copies=" << Tracker<Instrument>::copies << " moves=" << Tracker<Instrument>::moves << std::endl;

	while (1)
	{
		std::cout << "send" << std::endl;
		send_instrument_definitions(client, messages);
		std::this_thread::sleep_for(std::chrono::seconds(1));
	}

	return 0;
}
