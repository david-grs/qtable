#pragma once

#include <boost/optional.hpp>

#include <boost/serialization/serialization.hpp>
#include <boost/serialization/unordered_map.hpp>
#include <boost/serialization/split_member.hpp>

#include <boost/archive/binary_iarchive.hpp>
#include <boost/archive/binary_oarchive.hpp>

#include <vector>
#include <string>
#include <unordered_map>
#include <experimental/string_view>

// cf https://www.cmegroup.com/confluence/display/EPICSANDBOX/MDP+3.0+-+Security+Definition

#define FIX_TAGS \
	FIX_TAG(SecurityExchange, 207) \
	FIX_TAG(SecurityGroup, 1151) \
	FIX_TAG(Asset, 6937) \
	FIX_TAG(InstrumentName, 55) \
	FIX_TAG(SecurityID, 48) \
	FIX_TAG(SecurityType, 167) /* OOF or FUT */ \
	FIX_TAG(CFICode, 461) \
	FIX_TAG(Currency, 15) \
	FIX_TAG(MinPriceIncrement, 969) \
	FIX_TAG(StrikePrice, 202) /* only outright */ \
	FIX_TAG(TradingReferencePrice, 1150) \
	FIX_TAG(UnderlyingSymbol, 311) /* only outright */ \
	FIX_TAG(EventType, 865) /* 7=Last eligible trade date */ \
	FIX_TAG(EventTime, 1145) /* UTCTimestamp */ \

enum struct FIXTagCode : int
{
#define FIX_TAG(name, code) name = code,
	FIX_TAGS
#undef FIX_TAG
};

static std::unordered_map<FIXTagCode, std::string> FIXTagNames = {{
#define FIX_TAG(name, code) {FIXTagCode:: name, #name},
	FIX_TAGS
#undef FIX_TAG
}};

struct FIXTag
{
	explicit FIXTag(const FIXTagCode code) :
		mCode(code)
	{
	}

	FIXTagCode GetCode() const { return mCode; }

	const std::string& GetCodeStr() const
	{
		if (!mCodeStr)
			mCodeStr = std::to_string(static_cast<int>(mCode));
		return *mCodeStr;
	}

	const std::string& GetName() const
	{
		if (!mName)
		{
			static const std::string Unknown = "???";

			auto it = FIXTagNames.find(mCode);
			if (it == FIXTagNames.cend())
				mName = Unknown;
			else
				mName = it->second;
		}

		return *mName;
	}

	bool operator==(const FIXTag& rhs) const { return GetCode() == rhs.GetCode(); }

private:
	friend class boost::serialization::access;

	template<class Archive>
	void save(Archive& ar, const unsigned int) const
	{
		ar & static_cast<const int>(mCode);
	}

	template<class Archive>
	void load(Archive& ar, const unsigned int)
	{
		int code;
		ar & code;

		mCode = static_cast<FIXTagCode>(code);
	}
	BOOST_SERIALIZATION_SPLIT_MEMBER()

	FIXTagCode mCode;
	mutable boost::optional<std::string> mCodeStr;
	mutable boost::optional<const std::string&> mName;
};

inline std::ostream& operator<<(std::ostream& oss, const FIXTag& tag)
{
	return oss << tag.GetName();
}

namespace std {

template <>
struct hash<FIXTag>
{
	inline std::size_t operator()(const FIXTag& tag) const
	{
		return std::hash<int>()(static_cast<int>(tag.GetCode()));
	}
};

}

static constexpr std::size_t FIXTagsCount = 0
#define FIX_TAG(name, code) + 1
	FIX_TAGS
#undef FIX_TAG
;

static const std::vector<FIXTag> FIXTags = {{
#define FIX_TAG(name, code) FIXTag(FIXTagCode:: name),
	FIX_TAGS
#undef FIX_TAG
}};
static const std::unordered_map<std::experimental::string_view, FIXTag> FIXTagsByCode = {
#define FIX_TAG(name, code) {std::experimental::string_view(#code), FIXTag(FIXTagCode:: name)},
	FIX_TAGS
#undef FIX_TAG
};

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

using InstrumentId = int64_t;

struct Instrument : Tracker<Instrument>
{
	Instrument() =default;

	template <typename Market, typename Feedcode, typename Attributes>
	Instrument(Market&& _market, Feedcode&& _feedcode, Attributes&& _attributes) :
		id(NextInstrumentId()),
		market(std::forward<Market>(_market)),
		feedcode(std::forward<Feedcode>(_feedcode)),
		attributes(std::forward<Attributes>(_attributes))
	{ }

	int64_t GetId() const { return id; }
	const std::string& GetMarket() const { return market; }
	const std::string& GetFeedcode() const { return feedcode; }
	const auto& GetAttributes() const { return attributes; }

private:
	static InstrumentId NextInstrumentId()
	{
		static InstrumentId NextId = 0;
		return NextId++;
	}

	InstrumentId id;
	std::string  market;
	std::string  feedcode;
	std::unordered_map<FIXTag, std::string> attributes;

	friend class boost::serialization::access;
	template<class Archive>
	void serialize(Archive & ar, const unsigned int /*version*/)
	{
		ar & id & market & feedcode & attributes;
	}
};


inline std::ostream& operator<<(std::ostream& oss, const Instrument& instr)
{
	oss << instr.GetMarket() << ":" << instr.GetFeedcode() << " attributes={";
	for (const auto& p : instr.GetAttributes())
		oss << p.first << "=" << p.second << " ";
	return oss << "}";
}

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


