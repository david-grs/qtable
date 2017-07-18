#include <string>

#include "udp_client.h"
#include "proto.h"
#include "futils.h"

#include <boost/regex.hpp>
#include <boost/algorithm/string.hpp>

#include <array>
#include <vector>
#include <thread>
#include <chrono>
#include <experimental/string_view>
#include <cassert>

template <typename T, typename... Args>
constexpr auto make_array(T&& t, Args... args)
{
	return std::array<T, sizeof...(Args) + 1>{t, args...};
}

Instrument parse_instrument(const std::string& str) // std::experimental::string_view
{
	// cf https://www.cmegroup.com/confluence/display/EPICSANDBOX/MDP+3.0+-+Security+Definition
	static auto FIXTags = make_array(
//#define TAG(name, code) std::make_pair(std::string(#name), std::string(#code))
#define TAG(name, code) std::make_pair(std::experimental::string_view(#name), std::experimental::string_view(#code))
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
	TAG(EventTime, 1145) // UTCTimestamp
#undef TAG
	);

	std::array<bool, FIXTags.size()> parsed{};
	Instrument instr;
	bool expiryDate = false;

	for (std::size_t i = 0; i < str.size(); ++i)
	{
		if (str[i] == 0x01)
			continue;

		const std::size_t startTag = i;
		const std::size_t separator = str.find('=', i + 1);

		if (separator == std::string::npos)
			throw std::runtime_error("invalid security definition");

		const std::size_t tagLength = separator - startTag;
		const std::size_t startValue = separator + 1;
		const std::size_t nextPair = str.find(0x01, startValue + 1);

		const std::size_t valueLength = nextPair != std::string::npos ? nextPair - startValue - 1 : str.size() - startValue;
		i = nextPair;

		for (std::size_t t = 0; t < FIXTags.size(); ++t)
		{
			if (parsed[t])
				continue;

			const auto& p = FIXTags[t];

			const auto& tagName = p.first;
			const auto& tagCode = p.second;
			const std::size_t tagSize = tagCode.size();

			if (tagSize == tagLength && tagCode == std::experimental::string_view(str.c_str() + startTag, tagSize))
			{
				std::string value = str.substr(startValue, valueLength);
				if (tagCode == "865") // FIX
				{
					expiryDate = value == "7";
				}
				else if (tagCode != "1145" || expiryDate)
				{
					auto p = instr.attributes.emplace(tagName, std::move(value));

					assert(p.second);
					(void)p;

					parsed[t] = true;
				}

				break;
			}
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

	std::cout << "ctor=" << Tracker<Instrument>::ctor << " copies=" << Tracker<Instrument>::copies << " moves=" << Tracker<Instrument>::moves << std::endl;

	while (1)
	{
		std::cout << "send" << std::endl;
		std::this_thread::sleep_for(std::chrono::seconds(1));
		send_instrument_definitions(client, instruments);
	}

	return 0;
}
