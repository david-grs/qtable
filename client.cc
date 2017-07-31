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

InstrumentDefinition parse_instrument(const std::string& str)
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

	return InstrumentDefinition(std::move(market), std::move(feedcode), std::move(attributes));
}

std::vector<InstrumentDefinition> load(const std::string& filename)
{
	auto start = std::chrono::steady_clock::now();

	std::vector<InstrumentDefinition> instruments;
	instruments.reserve(1000000);

	std::string content = cpp::read_all(filename);

	cpp::for_each_line(content, [&](const std::string& line)
	{
		InstrumentDefinition instr = parse_instrument(line);
		if (instr.GetFeedcode().substr(0, 2) == "OZ")
			instruments.push_back(std::move(instr));
		//std::cout << instruments.back() << std::endl;
	});

	auto end = std::chrono::steady_clock::now();

	std::cout << "loaded " << instruments.size() << " instruments after " << std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count() << std::endl;
	return instruments;
}

std::vector<std::string> serialize_instruments(const std::vector<InstrumentDefinition>& instruments)
{
	std::vector<std::string> messages;
	messages.reserve(instruments.size());

	for (const InstrumentDefinition& instr : instruments)
	{
		std::string msg = proto::serialize(instr);
		messages.push_back(std::move(msg));//client.send(str);
	}

	return messages;
}

void send_instrument_definitions(udp_client& client, const std::vector<std::string>& messages)
{
	for (const std::string& msg : messages)
		client.send(msg);
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
