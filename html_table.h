#pragma once

#include "types.h"

#include <QString>

struct HtmlTable
{
	void AddRow(const Instrument& instrument);

	QString ToHtml() const;

private:
	QString mHtml;
};
