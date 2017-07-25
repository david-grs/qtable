#include "mainwindow.h"
#include "types.h"

#include <QtWidgets>

#if QT_VERSION < QT_VERSION_CHECK(5, 4, 0)
#include <QtWebKitWidgets>
#else
#include <QWebEnginemWebView>
#endif

MainWindow::MainWindow()
{
	QFile file;
	file.setFileName(":/jquery.js");
	file.open(QIODevice::ReadOnly);
	mJQuery = file.readAll();
	file.close();

	QWebSettings::globalSettings()->setAttribute(QWebSettings::DeveloperExtrasEnabled, true);


	mWebView = new QWebView(this);

	mWebView->setUrl(QUrl("qrc:/index.html"));
	connect(mWebView, SIGNAL(loadFinished(bool)), SLOT(finishLoading(bool)));

	resize(300, 200);
	setWindowFlags(Qt::Window | Qt::FramelessWindowHint);
	setCentralWidget(mWebView);

	//mWebView->setContextMenuPolicy(Qt::CustomContextMenu); // disable default context menu (on right click) with Reload
	mWebView->installEventFilter(this); // forward mouse events from the widget to this object
}

void MainWindow::OnInstrumentAdded(Instrument&& instr)
{

}

void MainWindow::setHtml(const QString& html)
{
	mWebView->page()->mainFrame()->setHtml(html);
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
		throw std::runtime_error("cannot load page from resource"); // TODO

	QWebFrame* frame = mWebView->page()->mainFrame();

	//QSize tableSize = frame->findFirstElement("table").geometry().size();
	//resize(tableSize);

	frame->setScrollBarPolicy(Qt::Horizontal, Qt::ScrollBarAlwaysOff);
	frame->setScrollBarPolicy(Qt::Vertical, Qt::ScrollBarAsNeeded);
	frame->evaluateJavaScript(mJQuery);

	QString code = "$('#table td:nth-child(2)').hide(); $('#table th:nth-child(2)').hide();";
	frame->evaluateJavaScript(code);
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

