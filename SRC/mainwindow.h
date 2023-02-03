#pragma once

#include <QWidget>
#include <QtWebSockets/QWebSocket>

class QPushButton;
class QLineEdit;
class QSvgWidget;

class MainWindow : public QWidget
{
	Q_OBJECT

public:
	MainWindow(QWidget* parent = nullptr);

private slots:
	void onConnect();
	void onLightOff();
	void onLightMax();
	void onConnected();
	void onDisconnected();
	void onTextMessageReceived(QString message);

private:
	QLineEdit *serverAddrEdit = nullptr;
	QPushButton *serverConnectButton = nullptr;
	QPushButton *lightOffButton = nullptr;
	QPushButton *lightMaxButton = nullptr;
	QWebSocket webSocket;
	QSvgWidget *connectionIndicator = nullptr;
};
