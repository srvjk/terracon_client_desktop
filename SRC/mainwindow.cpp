#include "mainwindow.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QPushButton>
#include <QLineEdit>
#include <QSvgWidget>
#include <QDebug>

MainWindow::MainWindow(QWidget* parent):
	QWidget(parent)
{
	QVBoxLayout *mainLayout = new QVBoxLayout();

	QHBoxLayout* addressLayout = new QHBoxLayout();
	mainLayout->addLayout(addressLayout);

	connectionIndicator = new QSvgWidget();
	connectionIndicator->load(QString(":/not_connected.svg"));
	addressLayout->addWidget(connectionIndicator);

	serverAddrEdit = new QLineEdit("127.0.0.1:10000");
	addressLayout->addWidget(serverAddrEdit);
	serverConnectButton = new QPushButton("Connect");
	addressLayout->addWidget(serverConnectButton);

	QHBoxLayout *lightLayout = new QHBoxLayout();
	mainLayout->addLayout(lightLayout);
	
	lightOffButton = new QPushButton("Off");
	lightLayout->addWidget(lightOffButton);
	lightMaxButton = new QPushButton("Max");
	lightLayout->addWidget(lightMaxButton);

	connect(serverConnectButton, SIGNAL(clicked()), this, SLOT(onConnect()));
	connect(lightOffButton, SIGNAL(clicked()), this, SLOT(onLightOff()));
	connect(lightMaxButton, SIGNAL(clicked()), this, SLOT(onLightMax()));
	connect(&webSocket, SIGNAL(connected()), this, SLOT(onConnected()));
	connect(&webSocket, SIGNAL(disconnected()), this, SLOT(onDisconnected()));
	connect(&webSocket, &QWebSocket::textFrameReceived, this, &MainWindow::onTextMessageReceived);

	setLayout(mainLayout);
}

void MainWindow::onConnect()
{
	webSocket.open(QUrl(serverAddrEdit->text()));
}

void MainWindow::onLightOff()
{

}

void MainWindow::onLightMax()
{

}

void MainWindow::onConnected()
{
	connectionIndicator->load(QString(":/connected.svg"));
}

void MainWindow::onDisconnected()
{
	connectionIndicator->load(QString(":/not_connected.svg"));
}

void MainWindow::onTextMessageReceived(QString message)
{
	qDebug() << "Message received: " << message;
}
