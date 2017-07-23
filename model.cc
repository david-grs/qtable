#include "model.h"

Model::Model(IView& view) :
	mView(view)
{}

void Model::AddInstrument(Instrument&& instr)
{
	mInstruments.push_back(std::move(instr));

	if (mFilter.Matches(mInstruments.back()))
		mTable.AddRow(mInstruments.back());
}

void Model::UpdateView()
{

}


bool Filter::Matches(const Instrument&)
{
	// TODO
	return true;
}

