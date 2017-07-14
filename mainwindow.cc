#include "mainwindow.h"

#include <QtWidgets>

#if QT_VERSION < QT_VERSION_CHECK(5, 4, 0)
#include <QtWebKitWidgets>
#else
#include <QWebEngineView>
#endif

MainWindow::MainWindow()
{
	QFile file;
	file.setFileName(":/jquery.js");
	file.open(QIODevice::ReadOnly);
	jQuery = file.readAll();
	jQuery.append("\nvar qt = { 'jQuery': jQuery.noConflict(true) };");
	file.close();

	view = new QWebView(this);

	view->setUrl(QUrl("qrc:/index.html"));
	connect(view, SIGNAL(loadFinished(bool)), SLOT(finishLoading(bool)));

	resize(300, 200);
	setWindowFlags(Qt::Window | Qt::FramelessWindowHint);
	setCentralWidget(view);

	view->installEventFilter(this);
}

void MainWindow::setHtml(const QString& html)
{
	view->page()->mainFrame()->setHtml(html);
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

	QWebFrame* frame = view->page()->mainFrame();

	QSize tableSize = frame->findFirstElement("table").geometry().size();
	resize(tableSize);

	frame->setScrollBarPolicy(Qt::Horizontal, Qt::ScrollBarAlwaysOff);
	frame->setScrollBarPolicy(Qt::Vertical, Qt::ScrollBarAsNeeded);
	frame->evaluateJavaScript(jQuery);
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

