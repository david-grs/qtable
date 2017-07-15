#pragma once

#include <vector>
#include <string>
#include <unordered_map>

#include <boost/archive/binary_iarchive.hpp>
#include <boost/archive/binary_oarchive.hpp>

using InstrumentId = int;

struct Instrument
{
	Instrument() =default;

	template <typename Arg1, typename Arg2>
	Instrument(InstrumentId _id, Arg1&& _market, Arg2&& _feedcode)
	: 	id(_id),
		market(std::forward<Arg1>(_market)),
		feedcode(std::forward<Arg2>(_feedcode))
	{}

	InstrumentId id;
	std::string  market;
	std::string  feedcode;
	std::unordered_map<std::string, std::string> attributes;

private:
	friend class boost::serialization::access;
	template<class Archive>
	void serialize(Archive & ar, const unsigned int version)
	{
		ar & id & market & feedcode;
	}
};

struct DepthEntry
{
	double price;
	int    volume;

private:
	friend class boost::serialization::access;
	template<class Archive>
	void serialize(Archive & ar, const unsigned int version)
	{
		ar & price & volume;
	}
};

struct Bid {};
struct Ask {};

template <class T>
struct BidAsk
{
	template <class Side> const T& get() const;
	template <class Side> T& get();

	T bid;
	T ask;

private:
	friend class boost::serialization::access;
	template<class Archive>
	void serialize(Archive & ar, const unsigned int version)
	{
		ar & bid & ask;
	}
};

namespace detail
{

template <class T, class Side>
struct get;

template <class T>
struct get<T, Bid>
{
	static const T& impl(const BidAsk<T>& p) { return p.bid; }
	static T& impl(BidAsk<T>& p) { return p.bid; }
};

template <class T>
struct get<T, Ask>
{
	static const T& impl(const BidAsk<T>& p) { return p.ask; }
	static T& impl(BidAsk<T>& p) { return p.ask; }
};

}

template <class T> template <class Side>
const T& BidAsk<T>::get() const { return detail::get<T, Side>::impl(*this); }

template <class T> template <class Side>
T& BidAsk<T>::get() { return detail::get<T, Side>::impl(*this); }


