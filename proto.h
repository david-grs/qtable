#pragma once

#include "types.h"

#include <boost/archive/binary_iarchive.hpp>
#include <boost/archive/binary_oarchive.hpp>
#include <boost/asio/buffer.hpp>

#include <sstream>

namespace proto
{

using InstrumentDefinition = Instrument;

struct TradeTick
{
	InstrumentId instr_id;
	double       price;
	int          volume;

private:
	friend class boost::serialization::access;
	template<class Archive>
	void serialize(Archive & ar, const unsigned int version)
	{
		ar & instr_id & price & volume;
	}
};

struct BookUpdate
{
	InstrumentId       instr_id;
	BidAsk<DepthEntry> top;

private:
	friend class boost::serialization::access;
	template<class Archive>
	void serialize(Archive & ar, const unsigned int version)
	{
		ar & instr_id & top.bid & top.ask;
	}
};

template <typename T>
std::string serialize(const T& t)
{
	std::ostringstream strm;
	boost::archive::binary_oarchive archive(strm);
	archive << t;
	return strm.str();
}

template <typename T>
T deserialize(const std::string& str)
{
	std::stringstream strm;
	strm.write(str.c_str(), str.size());

	boost::archive::binary_iarchive archive(strm);

	T t;
	archive >> t;
	return t;
}

} // end NS proto

