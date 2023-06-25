#include "mainwindow.h"
#include <QLabel>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QPushButton>
#include <QLineEdit>
#include <QSvgWidget>
#include <QGroupBox>
#include <QSlider>
#include <QDebug>
#include <QXmlStreamReader>
#include <QXmlStreamAttributes>
#include <QMessageBox>

MainWindow::MainWindow(QWidget* parent):
	QWidget(parent)
{
	QVBoxLayout *mainLayout = new QVBoxLayout();

	QHBoxLayout* addressLayout = new QHBoxLayout();
	mainLayout->addLayout(addressLayout);

	connectionIndicator = new QSvgWidget();
	connectionIndicator->load(QString(":/not_connected.svg"));
	addressLayout->addWidget(connectionIndicator);

	serverAddrEdit = new QLineEdit("192.168.0.108");
	addressLayout->addWidget(serverAddrEdit);
	QLabel* commaLabel = new QLabel(":");
	addressLayout->addWidget(commaLabel);
	serverPortEdit = new QLineEdit("8001");
	addressLayout->addWidget(serverPortEdit);
	serverConnectButton = new QPushButton("Подключение");
	addressLayout->addWidget(serverConnectButton);
	updateFromServerButton = new QPushButton("<->");
	addressLayout->addWidget(updateFromServerButton);
	serverShutdownButton = new QPushButton("X");
	addressLayout->addWidget(serverShutdownButton);

	// свет
	QGroupBox *lightGroup = new QGroupBox("Свет");
	mainLayout->addWidget(lightGroup);

	QVBoxLayout *lightLayout = new QVBoxLayout();
	lightGroup->setLayout(lightLayout);
	
	QHBoxLayout* sliderLayout = new QHBoxLayout();
	lightLayout->addLayout(sliderLayout);
	lightIntensitySlider = new QSlider(Qt::Horizontal);
	sliderLayout->addWidget(lightIntensitySlider);
	setLightValueButton = new QPushButton("ОК");
	sliderLayout->addWidget(setLightValueButton);

	lightOffButton = new QPushButton("Выкл");
	lightLayout->addWidget(lightOffButton);
	lightMaxButton = new QPushButton("Максимум");
	lightLayout->addWidget(lightMaxButton);

	// вода
	QGroupBox* waterGroup = new QGroupBox("Вода");
	mainLayout->addWidget(waterGroup);

	QVBoxLayout* waterLayout = new QVBoxLayout();
	waterGroup->setLayout(waterLayout);

	QHBoxLayout* waterButtonsLayout = new QHBoxLayout();
	waterLayout->addLayout(waterButtonsLayout);

	waterOnButton = new QPushButton("Вкл");
	waterButtonsLayout->addWidget(waterOnButton);
	waterOffButton = new QPushButton("Выкл");
	waterButtonsLayout->addWidget(waterOffButton);

	connect(serverConnectButton, SIGNAL(clicked()), this, SLOT(onConnect()));
	connect(updateFromServerButton, SIGNAL(clicked()), this, SLOT(onUpdateFromServer()));
	connect(serverShutdownButton, &QPushButton::clicked, this, &MainWindow::onRequestServerShutdown);
	connect(lightIntensitySlider, SIGNAL(valueChanged(int)), this, SLOT(onLightIntensitySliderValueChanged(int)));
	connect(setLightValueButton, SIGNAL(clicked()), this, SLOT(onLightSet()));
	connect(lightOffButton, SIGNAL(clicked()), this, SLOT(onLightOff()));
	connect(lightMaxButton, SIGNAL(clicked()), this, SLOT(onLightMax()));
	connect(waterOnButton, SIGNAL(clicked()), this, SLOT(onWaterOn()));
	connect(waterOffButton, SIGNAL(clicked()), this, SLOT(onWaterOff()));
	connect(&webSocket, SIGNAL(connected()), this, SLOT(onConnected()));
	connect(&webSocket, SIGNAL(disconnected()), this, SLOT(onDisconnected()));
	connect(&webSocket, &QWebSocket::textFrameReceived, this, &MainWindow::onTextMessageReceived);

	setLayout(mainLayout);
}

void MainWindow::onConnect()
{
	//QUrl url(serverAddrEdit->text());
	QString addrStr = serverAddrEdit->text();
	QString portStr = serverPortEdit->text();

	QHostAddress ha;
	if (!ha.setAddress(addrStr)) {
		messageForUser("Некорректный адрес сервера!");
		return;
	}

	bool ok = false;
	int port = portStr.toInt(&ok);
	if (!ok) {
		messageForUser("Некорректный порт!");
		return;
	}
	if (port < 1 || port > 65535) {
		messageForUser("Некорректный порт!");
	}

	QUrl url;
	url.setScheme("ws");
	url.setHost(ha.toString());
	url.setPort(port);
	qDebug() << url.toString();
	qDebug() << "connecting to " << url.scheme() << "//" << url.host() << ":" << url.port();
	webSocket.open(url);
}

void MainWindow::onUpdateFromServer()
{
	QString strCommand = makeCommand_UpdateFromServer();
	sendCommand(strCommand);
}

void MainWindow::onRequestServerShutdown()
{
	QMessageBox msgBox;
	msgBox.setText("Запрос на выключение сервера");
	msgBox.setInformativeText("Сервер будет остановлен, потребуется ручной перезапуск! Продолжить?");
	msgBox.setStandardButtons(QMessageBox::Yes | QMessageBox::Cancel);
	msgBox.setDefaultButton(QMessageBox::Cancel);
	int ret = msgBox.exec();
	if (ret == QMessageBox::Yes) {
		onShutdownServer();
	}
}

void MainWindow::onShutdownServer()
{
	QString strCommand = makeCommand_ServerShutdown();
	sendCommand(strCommand);
}

void MainWindow::onLightIntensitySliderValueChanged(int value)
{
	lightIntensity = value;
	setLightValueButton->setText(QString("%1").arg(lightIntensity));
}

void MainWindow::onLightSet()
{
	QString strCommand = makeCommand_SetLightValue();
	qDebug() << strCommand;

	sendCommand(strCommand);
}

void MainWindow::onLightOff()
{

}

void MainWindow::onLightMax()
{

}

void MainWindow::onWaterOn()
{
	QString strCommand = makeCommand_WaterOn();
	qDebug() << strCommand;

	sendCommand(strCommand);
}

void MainWindow::onWaterOff()
{
	QString strCommand = makeCommand_WaterOff();
	qDebug() << strCommand;

	sendCommand(strCommand);
}

void MainWindow::onConnected()
{
	connect(&webSocket, &QWebSocket::textFrameReceived, this, &MainWindow::onTextMessageReceived);
	connectionIndicator->load(QString(":/connected.svg"));
	onUpdateFromServer();
}

void MainWindow::onDisconnected()
{
	connectionIndicator->load(QString(":/not_connected.svg"));
}

void MainWindow::onTextMessageReceived(QString message)
{
	qDebug() << "Message received: " << message;
}

void MainWindow::messageForUser(const QString& message)
{
	qDebug() << "INFO: " << message;
}

void MainWindow::sendCommand(const QString& command)
{
	webSocket.sendTextMessage(command);
}

QString MainWindow::makeCommand_UpdateFromServer()
{
	QString strXml;
	QXmlStreamWriter writer(&strXml);
	writer.setAutoFormatting(true);
	writer.writeStartDocument();

	writer.writeStartElement("command");
	writer.writeAttribute("opcode", "updateFromServer");
	writer.writeEndElement(); // command

	writer.writeEndElement();

	return strXml;
}

QString MainWindow::makeCommand_ServerShutdown()
{
	QString strXml;
	QXmlStreamWriter writer(&strXml);
	writer.setAutoFormatting(true);
	writer.writeStartDocument();

	writer.writeStartElement("command");
	writer.writeAttribute("opcode", "serverShutdown");
	writer.writeEndElement(); // command

	writer.writeEndElement();

	return strXml;
}

QString MainWindow::makeCommand_SetLightValue()
{
	QString strXml;
	QXmlStreamWriter writer(&strXml);
	writer.setAutoFormatting(true);
	writer.writeStartDocument();

	writer.writeStartElement("command");
	writer.writeAttribute("opcode", "setLightIntensity");
	writer.writeCharacters(QString("%1").arg(lightIntensity));
	writer.writeEndElement(); // command

	writer.writeEndElement();

	return strXml;
}

QString MainWindow::makeCommand_WaterOn()
{
	QString strXml;
	QXmlStreamWriter writer(&strXml);
	writer.setAutoFormatting(true);
	writer.writeStartDocument();

	writer.writeStartElement("command");
	writer.writeAttribute("opcode", "waterOn");
	writer.writeEndElement(); // command

	writer.writeEndElement();

	return strXml;
}

QString MainWindow::makeCommand_WaterOff()
{
	QString strXml;
	QXmlStreamWriter writer(&strXml);
	writer.setAutoFormatting(true);
	writer.writeStartDocument();

	writer.writeStartElement("command");
	writer.writeAttribute("opcode", "waterOff");
	writer.writeEndElement(); // command

	writer.writeEndElement();

	return strXml;
}
