#pragma once

#include <vector>
#include <string>
#include <unordered_map>
#include <experimental/string_view>

#include <boost/archive/binary_iarchive.hpp>
#include <boost/archive/binary_oarchive.hpp>

using InstrumentId = int;

template <typename Obj>
struct Tracker
{
	Tracker() { ++ctor; }
	~Tracker() { ++dtor; }

	Tracker(const Tracker&) { ++copies; }
	Tracker& operator=(const Tracker&) { ++copies; }

	Tracker(Tracker&&) { ++moves; }
	Tracker& operator=(Tracker&&) { ++moves; }

	static std::size_t ctor;
	static std::size_t dtor;
	static std::size_t copies;
	static std::size_t moves;
};

template <typename Obj> std::size_t Tracker<Obj>::ctor = 0;
template <typename Obj> std::size_t Tracker<Obj>::dtor = 0;
template <typename Obj> std::size_t Tracker<Obj>::copies = 0;
template <typename Obj> std::size_t Tracker<Obj>::moves = 0;

struct Instrument : Tracker<Instrument>
{
	Instrument() =default;

	template <typename MarketT, typename FeedcodeT>
	Instrument(InstrumentId _id, MarketT&& _market, FeedcodeT&& _feedcode)
	: 	id(_id),
		market(std::forward<MarketT>(_market)),
		feedcode(std::forward<FeedcodeT>(_feedcode))
	{ }

	InstrumentId id;
	std::string  market;
	std::string  feedcode;
	std::unordered_map<std::experimental::string_view, std::string> attributes;

private:
	friend class boost::serialization::access;
	template<class Archive>
	void serialize(Archive & ar, const unsigned int /*version*/)
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
	void serialize(Archive & ar, const unsigned int /*version*/)
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
	void serialize(Archive & ar, const unsigned int /*version*/)
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


