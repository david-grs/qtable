#pragma once

#include "types.h"

#include <boost/optional.hpp>

#include <string>

struct Instrument
{
	explicit Instrument(const InstrumentDefinition& def);

	int64_t GetId() const { return mId; }
	const std::string& GetMarket() const { return mMarket; }
	const std::string& GetFeedcode() const { return mFeedcode; }

	const PriceDepth& GetPriceDepth() const { return mPriceDepth; }

	double GetTheo() const { return mTheo; }

	boost::optional<const DepthEntry&> GetBid() const { return mPriceDepth.bids.size() ? boost::make_optional<const DepthEntry&>(mPriceDepth.bids[0]) : boost::none; }
	boost::optional<const DepthEntry&> GetAsk() const { return mPriceDepth.asks.size() ? boost::make_optional<const DepthEntry&>(mPriceDepth.asks[0]) : boost::none; }

	template <typename DepthT>
	void UpdatePriceDepth(DepthT&& priceDepth) { mPriceDepth = std::move(priceDepth); }

	void UpdateTheo(double theo) { mTheo = theo; }

private:
	int64_t mId;
	const std::string mMarket;
	const std::string mFeedcode;

	PriceDepth mPriceDepth;

	double mTheo;
};
