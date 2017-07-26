#pragma once

#include "interfaces.h"
#include "types.h"

#include "filter.h"
#include "html_table.h"

#include <QObject>

class QTimer;

class Model :
	public QObject,
	public IModel
{
	Q_OBJECT

public:
	explicit Model(IView& view);
	~Model();

	void AddInstrument(InstrumentDefinition&& instr);

private slots:
	void UpdateView();

private:
	IView& mView;
	HtmlRenderer mRenderer;
	Filter mFilter;

	std::vector<InstrumentDefinition> mDefinitions;
	std::vector<Instrument> mInstruments;

	QTimer* mRenderingTimer;
};
