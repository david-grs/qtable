#include "model.h"

#include <QTimer>
#include <QDebug>
#include <QElapsedTimer>

Model::Model(IView& view) :
	mView(view),
	mRenderer(mFilter)
{
	mRenderingTimer = new QTimer(this);
	connect(mRenderingTimer, SIGNAL(timeout()), this, SLOT(UpdateView()));
	mRenderingTimer->start(10000);

	UpdateView();
}

Model::~Model()
{
}

void Model::AddInstrument(InstrumentDefinition&& def)
{
	mDefinitions.push_back(std::move(def));
	mInstruments.emplace_back(mDefinitions.back());
}

void Model::UpdateView()
{
	QElapsedTimer timer;

	timer.start();
	QString html = mRenderer.ToHtml(mInstruments);
	qDebug() << timer.nsecsElapsed() / 1000.0;

	timer.restart();
	mView.SetHtml(html);
	qDebug() << timer.nsecsElapsed() / 1000.0;
}
