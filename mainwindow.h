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

	void adjustLocation();
	void changeLocation();
	void adjustTitle();
	void setProgress(int p);
	void finishLoading(bool);

	void viewSource();
	void slotSourceDownloaded();

	void highlightAllLinks();
	void rotateImages(bool invert);
	void removeGifImages();
	void removeInlineFrames();
	void removeObjectElements();
	void removeEmbeddedElements();

private:
	QString jQuery;
	QWebView *view;
	QLineEdit *locationEdit;
	QAction *rotateAction;
	int progress;
};
