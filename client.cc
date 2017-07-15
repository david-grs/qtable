#include "udp_client.h"
#include "proto.h"
#include "futils.h"

#include <boost/regex.hpp>
#include <boost/algorithm/string.hpp>

#include <thread>
#include <chrono>
#include <experimental/string_view>
#include <cassert>

Instrument parse_instrument(const std::string& str) // std::experimental::string_view
{
	// cf https://www.cmegroup.com/confluence/display/EPICSANDBOX/MDP+3.0+-+Security+Definition
	static std::vector<std::pair<std::string, std::string>> FIXTags {
#define TAG(name, code) {#name, #code}
	TAG(SecurityExchange, 207),
	TAG(SecurityGroup, 1151),
	TAG(Asset, 6937),
	TAG(InstrumentName, 55),
	TAG(SecurityID, 48),
	TAG(SecurityType, 167), // OOF or FUT
	TAG(CFICode, 461),
	TAG(Currency, 15),
	TAG(MinPriceIncrement, 969),
	TAG(StrikePrice, 202), // only for outright
	TAG(TradingReferencePrice, 1150),
	TAG(UnderlyingSymbol, 311), // only for outright
	TAG(EventType, 865), // 7=Last eligible trade date
	TAG(EventTime, 1145), // UTCTimestamp
#undef TAG
	};

	Instrument instr;
	bool expiryDate = false;

	for (std::size_t i = 0; i < str.size(); ++i)
	{
		if (str[i] == 0x01)
			continue;

		const std::size_t startTag = i;
		for (; i < str.size() && str[i] != '='; ++i);

		const std::size_t tagLength = i - startTag;
		const std::size_t startValue = ++i;
		for (; i < str.size() && str[i] != 0x01; ++i);
		const std::size_t valueLength = i - startValue;

		if (startValue >= str.size() || valueLength == 0)
			throw std::runtime_error("invalid security definition");

		for (const auto& p : FIXTags)
		{
			const std::string& tagName = p.first;
			const std::string& tagCode = p.second;
			const std::size_t tagSize = tagCode.size();

			if (tagSize == tagLength && str.substr(startTag, tagSize) == tagCode)
			{
				std::string value = str.substr(startValue, valueLength);
				if (tagCode == "865") // FIX
				{
					expiryDate = value == "7";
					continue;
				}

				if (tagCode != "1145" || expiryDate)
				{
					auto p = instr.attributes.emplace(tagName, std::move(value));
					assert(p.second);

					//auto it = p.first;
					//std::cout << it->first << " " << it->second << std::endl;
				}
			}
#if 0
			else
			{
				std::cout << "fail " << startTag << " " << startValue << " " << valueLength << " " << str.substr(startTag, tagSize) << std::endl;
			}
#endif
		}
	}

	return instr;
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

#if 0
			static const boost::regex LineValidator//("([A-Za-z]+),([A-Za-z0-9]+),[0-9]+\\.[0-9]+)
			boost::smatch groups;

			if (!boost::regex_match(line, groups, LineValidator) || groups.size() != 3)
			{
				std::cerr << "malformed line, ignoring" << std::endl;
				return;
			}
#endif
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

	while (1)
	{
		std::cout << "send" << std::endl;
		std::this_thread::sleep_for(std::chrono::seconds(1));
		send_instrument_definitions(client, instruments);
	}

	return 0;
}
