#pragma once

#include "instrument.h"

#include <QString>

struct Filter;

struct HtmlRenderer
{
	explicit HtmlRenderer(const Filter& filter) :
		mFilter(filter)
	{}

	QString ToHtml(const std::vector<Instrument>& instruments) const;

private:
	const Filter& mFilter;
	QString mHtml;
};
