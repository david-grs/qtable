#include "mainwindow.h"
#include "types.h"

#include <QtWidgets>

#if QT_VERSION < QT_VERSION_CHECK(5, 4, 0)
#include <QtWebKitWidgets>
#include <QWebSettings>
#else
#include <QWebEngineView>
#include <QWebEngineSettings>
#include <QWebChannel>
#endif

MainWindow::MainWindow()
{
	QFile file;
	file.setFileName(":/jquery.js");
	file.open(QIODevice::ReadOnly);
	mJQuery = file.readAll();
	file.close();

#if QT_VERSION < QT_VERSION_CHECK(5, 4, 0)
	QWebSettings::globalSettings()->setAttribute(QWebSettings::DeveloperExtrasEnabled, true);
	mWebView = new QWebView(this);
#else
	//QWebEngineSettings::globalSettings()->setAttribute(QWebEngineSettings::DeveloperExtrasEnabled, true);
	// enable it via an environment variable QTWEBENGINE_REMOTE_DEBUGGING=<port>. You can put 0.0.0.0:<port> if you are doing debugging of an embedded device
	// and cant use the local console. Then you can point can connect to http://127.0.0.1: to get the debugger. It will need to be a chromium based browser.
	// do you have to use Chrome, or you can actually use the "quick nano browser" example if you want.

	mWebView = new QWebEngineView(this);
#endif

	QWebEnginePage* page = mWebView->page();
	QWebChannel *channel = new QWebChannel(page);
	page->setWebChannel(channel);

	//channel->registerObject(QStringLiteral("jshelper"), helper);

	mWebView->setUrl(QUrl("qrc:/index.html"));
	connect(mWebView, SIGNAL(loadFinished(bool)), SLOT(finishLoading(bool)));

	resize(300, 200);
	setWindowFlags(Qt::Window | Qt::FramelessWindowHint);
	setCentralWidget(mWebView);

	//mWebView->setContextMenuPolicy(Qt::CustomContextMenu); // disable default context menu (on right click) with Reload
	mWebView->installEventFilter(this); // forward mouse events from the widget to this object
}

void MainWindow::SetHtml(const QString& html)
{
	//mWebView->page()->mainFrame()->setHtml(html,  QUrl("qrc:/"));

	mWebView->page()->setHtml(html, QUrl("qrc:/"));
	mWebView->page();
	//QWebElement table = mWebView->page()->mainFrame()->findFirstElement("table");
	//table.setInnerXml(html);
}

bool MainWindow::eventFilter(QObject *obj, QEvent *event)
{
	if (event->type() ==  QEvent::MouseButtonPress)
	{
		QMouseEvent *mouseEvent = static_cast<QMouseEvent*>(event);
		mousePressEvent(mouseEvent);
		return true;
	}
	else if (event->type() == QEvent::MouseMove)
	{
		QMouseEvent *mouseEvent = static_cast<QMouseEvent*>(event);
		mouseMoveEvent(mouseEvent);
		return true;
	}
	else if (event->type() == QEvent::MouseButtonDblClick)
	{
		return true;
	}
	else
	{
		return QObject::eventFilter(obj, event);
	}
}

void MainWindow::mousePressEvent(QMouseEvent *event)
{
	mPosition = event->pos();
}

void MainWindow::mouseMoveEvent(QMouseEvent *event)
{
	if (event->buttons() & Qt::LeftButton)
	{
		QPoint diff = event->pos() - mPosition;
		QPoint newpos = this->pos() + diff;

		this->move(newpos);
	}
}

void MainWindow::finishLoading(bool ok)
{
	if (!ok)
	{
		qDebug() << "page failed to load";
		return;//throw std::runtime_error("cannot load page from resource"); // TODO
	}
	qDebug() << "page successfully loaded";


#if QT_VERSION < QT_VERSION_CHECK(5, 4, 0)
	QWebFrame* frame = mWebView->page()->mainFrame();

	frame->setScrollBarPolicy(Qt::Horizontal, Qt::ScrollBarAlwaysOff);
	frame->setScrollBarPolicy(Qt::Vertical, Qt::ScrollBarAsNeeded);
	frame->evaluateJavaScript(mJQuery);
#endif

	//QString code = "$('#table td:nth-child(2)').hide(); $('#table th:nth-child(2)').hide();";
	//frame->evaluateJavaScript(code);
}

#if 0
void MainWindow::highlightAllLinks()
{
	// We append '; undefined' after the mJQuery call here to prevent a possible recursion loop and crash caused by
	// the way the elements returned by the each iterator elements reference each other, which causes problems upon
	// converting them to QVariants.
	QString code = "qt.mJQuery('a').each( function () { qt.mJQuery(this).css('background-color', 'yellow') } ); undefined";
	mWebView->page()->mainFrame()->evaluateJavaScript(code);
}
#endif

