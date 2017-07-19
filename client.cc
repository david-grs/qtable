#include <string>

#include "udp_client.h"
#include "proto.h"
#include "futils.h"

#include <boost/regex.hpp>
#include <boost/algorithm/string.hpp>
#include <boost/optional.hpp>

#include <array>
#include <vector>
#include <thread>
#include <chrono>
#include <experimental/string_view>
#include <cassert>

Instrument parse_instrument(const std::string& str)
{
	std::string market, feedcode;
	std::unordered_map<FIXTag, std::string> attributes;

	bool expiryDate = false;

	for (std::size_t i = 0; i < str.size(); ++i)
	{
		if (str[i] == 0x01)
			continue;

		const std::size_t startTag = i;
		const std::size_t separator = str.find('=', i + 1);

		if (separator == std::string::npos)
			throw std::runtime_error("invalid security definition: malformed input");

		const std::size_t tagSize = separator - startTag;
		const std::size_t startValue = separator + 1;
		const std::size_t nextPair = str.find(0x01, startValue + 1);

		const std::size_t valueLength = nextPair != std::string::npos ? nextPair - startValue : str.size() - startValue;
		i = nextPair;

		std::experimental::string_view codeStr(str.c_str() + startTag, tagSize);
		auto it = FIXTagsByCode.find(codeStr);
		if (it != FIXTagsByCode.cend())
		{
			const FIXTag& tag = it->second;
			FIXTagCode tagCode = tag.GetCode();

			std::string value = str.substr(startValue, valueLength);
			if (tagCode == FIXTagCode::EventType) // FIX
			{
				expiryDate = value == "7";
			}
			else if (tagCode == FIXTagCode::SecurityExchange)
			{
				market = std::move(value);
			}
			else if (tagCode == FIXTagCode::InstrumentName)
			{
				feedcode = std::move(value);
			}
			else if (tagCode != FIXTagCode::EventTime || expiryDate)
			{
				auto p = attributes.emplace(tag, std::move(value));

				assert(p.second);
				(void)p;
			}
		}
	}

	if (market.empty() || feedcode.empty())
		throw std::runtime_error("invalid security definition: invalid market/feedcode");

	return Instrument(std::move(market), std::move(feedcode), std::move(attributes));
}

std::vector<Instrument> load(const std::string& filename)
{
	auto start = std::chrono::steady_clock::now();

	std::vector<Instrument> instruments;
	instruments.reserve(1000000);

	std::string content = cpp::read_all(filename);

	cpp::for_each_line(content, [&](const std::string& line)
	{
		instruments.push_back(parse_instrument(line));
		//std::cout << instruments.back() << std::endl;
	});

	auto end = std::chrono::steady_clock::now();

	std::cout << "loaded " << instruments.size() << " instruments after " << std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count() << std::endl;
	return instruments;
}

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

	std::vector<Instrument> instruments = load("secdef.dat");

	std::cout << "ctor=" << Tracker<Instrument>::ctor << " copies=" << Tracker<Instrument>::copies << " moves=" << Tracker<Instrument>::moves << std::endl;

	while (1)
	{
		std::cout << "send" << std::endl;
		std::this_thread::sleep_for(std::chrono::seconds(1));
		send_instrument_definitions(client, instruments);
	}

	return 0;
}
