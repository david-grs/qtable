#pragma once

#include <QtWidgets>

class QWebView;
class QLineEdit;
class QPoint;

class MainWindow : public QMainWindow
{
	Q_OBJECT

public:
	explicit MainWindow();

protected slots:
	void finishLoading(bool);
	bool eventFilter(QObject *obj, QEvent *event);

private:
	void mousePressEvent(QMouseEvent *event);
	void mouseMoveEvent(QMouseEvent *event);

	QString jQuery;
	QWebView *view;
	QPoint mPosition;
};
