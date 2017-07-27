#pragma once

#include "interfaces.h"

#include <QtWidgets>

class QWebView;
class QWebEngineView;
class QLineEdit;
class QPoint;

struct Instrument;

class HtmlDocument : public QObject
{
	Q_OBJECT
	Q_PROPERTY(QString html MEMBER mHtml NOTIFY htmlChanged)

public:
	explicit HtmlDocument(QObject *parent = nullptr) : QObject(parent) {}

	void setHtml(const QString& html)
	{
		mHtml = html;
		emit htmlChanged(mHtml);
	}

signals:
	void htmlChanged(const QString& html);

private:
	QString mHtml;
};

class MainWindow :
	public QMainWindow,
	public IView
{
	Q_OBJECT

public:
	explicit MainWindow();

	// [IView]
	void SetHtml(const QString& html) override;

signals:
	void closed();

protected slots:
	void finishLoading(bool);
	bool eventFilter(QObject *obj, QEvent *event);

private:
	void mousePressEvent(QMouseEvent *event);
	void mouseMoveEvent(QMouseEvent *event);

protected:
	void closeEvent(QCloseEvent*) { emit closed(); }

	QString mJQuery;
	QPoint mPosition;

	HtmlDocument mHtmlContent;

#if QT_VERSION < QT_VERSION_CHECK(5, 4, 0)
	QWebView *mWebView;
#else
	QWebEngineView* mWebView;
#endif
};
