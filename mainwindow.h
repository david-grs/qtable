#pragma once

#include <QtWidgets>

class QWebView;
class QLineEdit;

class MainWindow : public QMainWindow
{
	Q_OBJECT

public:
	explicit MainWindow();

protected slots:
	void finishLoading(bool);

private:
	QString jQuery;
	QWebView *view;
};
