#pragma once

#include <QString>

struct IModel
{
	virtual ~IModel() {}
};


struct IView
{
	virtual ~IView() {}

	virtual void SetHtml(const QString& html) =0;
};

