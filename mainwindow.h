#pragma once

#include <QtWidgets>

class QWebView;
class QWebEngineView;
class QLineEdit;
class QPoint;

class MainWindow : public QMainWindow
{
	Q_OBJECT

public:
	explicit MainWindow();

signals:
	void closed();

public slots:
	void setHtml(const QString& html);

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

//#if QT_VERSION < QT_VERSION_CHECK(5, 4, 0)
	QWebView *mWebView;
//#else
//	QWebEngineView* view;
//#endif
};
