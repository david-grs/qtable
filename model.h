#pragma once

#include "interfaces.h"
#include "types.h"

#include "html_table.h"

struct Filter
{
	bool Matches(const Instrument&);
};

struct Model : public IModel
{
	explicit Model(IView& view);

	void AddInstrument(Instrument&& instr);

private:
	void UpdateView();

	IView& mView;
	HtmlTable mTable;
	Filter mFilter;
	std::vector<Instrument> mInstruments;
};
