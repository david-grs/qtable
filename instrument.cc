#include "instrument.h"

#include "types.h"

Instrument::Instrument(const InstrumentDefinition& def) :
	mId(def.GetId()),
	mMarket(def.GetMarket()),
	mFeedcode(def.GetFeedcode())
{}
