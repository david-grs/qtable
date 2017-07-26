#pragma once

struct Instrument;

struct Filter
{
	bool Matches(const Instrument&);
};
