#include "mainwindow.h"

#include <QtWidgets>
#include <QtNetwork>
#include <QtWebKitWidgets>

MainWindow::MainWindow()
{
	QFile file;
	file.setFileName(":/jquery.js");
	file.open(QIODevice::ReadOnly);
	jQuery = file.readAll();
	jQuery.append("\nvar qt = { 'jQuery': jQuery.noConflict(true) };");
	file.close();

	QNetworkProxyFactory::setUseSystemConfiguration(true);

	view = new QWebView(this);
	view->setUrl(QUrl("qrc:/index.html"));
	connect(view, SIGNAL(loadFinished(bool)), SLOT(finishLoading(bool)));

	resize(300, 200);
	setWindowFlags(Qt::Window | Qt::FramelessWindowHint);
	setCentralWidget(view);
}

void MainWindow::finishLoading(bool ok)
{
	qDebug() << "OK=" << ok;
	view->page()->mainFrame()->evaluateJavaScript(jQuery);
}

#if 0
void MainWindow::highlightAllLinks()
{
	// We append '; undefined' after the jQuery call here to prevent a possible recursion loop and crash caused by
	// the way the elements returned by the each iterator elements reference each other, which causes problems upon
	// converting them to QVariants.
	QString code = "qt.jQuery('a').each( function () { qt.jQuery(this).css('background-color', 'yellow') } ); undefined";
	view->page()->mainFrame()->evaluateJavaScript(code);
}
#endif

