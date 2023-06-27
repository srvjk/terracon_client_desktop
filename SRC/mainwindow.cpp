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
#include <QSettings>

MainWindow::MainWindow(QWidget* parent):
	QWidget(parent)
{
	readSettings();

	QVBoxLayout *mainLayout = new QVBoxLayout();

	// общее
	QGroupBox* generalGroup = new QGroupBox("");
	mainLayout->addWidget(generalGroup);

	QVBoxLayout* generalLayout = new QVBoxLayout();
	generalGroup->setLayout(generalLayout);

	QHBoxLayout* addressLayout = new QHBoxLayout();
	generalLayout->addLayout(addressLayout);

	connectionIndicator = new QSvgWidget();
	connectionIndicator->load(QString(":/not_connected.svg"));
	addressLayout->addWidget(connectionIndicator);

	//serverAddrEdit = new QLineEdit("192.168.0.108");
	serverAddrEdit = new QLineEdit(serverIp);
	addressLayout->addWidget(serverAddrEdit);
	QLabel* commaLabel = new QLabel(":");
	addressLayout->addWidget(commaLabel);
	serverPortEdit = new QLineEdit(serverPort);
	addressLayout->addWidget(serverPortEdit);
	serverConnectButton = new QPushButton("Подключение");
	addressLayout->addWidget(serverConnectButton);
	updateFromServerButton = new QPushButton("<->");
	addressLayout->addWidget(updateFromServerButton);
	serverShutdownButton = new QPushButton("X");
	addressLayout->addWidget(serverShutdownButton);

	QHBoxLayout* generalButtonsLayout = new QHBoxLayout();
	generalLayout->addLayout(generalButtonsLayout);

	scriptModeButton = new QPushButton("Режим сценария");
	generalButtonsLayout->addWidget(scriptModeButton);
	manualModeButton = new QPushButton("Ручной режим");
	generalButtonsLayout->addWidget(manualModeButton);

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

	QHBoxLayout* lightButtonsLayout = new QHBoxLayout();
	lightLayout->addLayout(lightButtonsLayout);

	lightOffButton = new QPushButton("Выкл");
	lightButtonsLayout->addWidget(lightOffButton);
	lightMaxButton = new QPushButton("Макс");
	lightButtonsLayout->addWidget(lightMaxButton);

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
	connect(scriptModeButton, &QPushButton::clicked, this, &MainWindow::onScriptModeButton);
	connect(manualModeButton, &QPushButton::clicked, this, &MainWindow::onManualModeButton);
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

void MainWindow::onScriptModeButton()
{
	QString strCommand = makeCommand_SetScriptMode();
	sendCommand(strCommand);
}

void MainWindow::onManualModeButton()
{
	QString strCommand = makeCommand_SetManualMode();
	sendCommand(strCommand);
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
	lightIntensity = 0;
	updateSettings();
	QString strCommand = makeCommand_SetLightValue();
	qDebug() << strCommand;
	sendCommand(strCommand);
}

void MainWindow::onLightMax()
{
	lightIntensity = 99;
	updateSettings();
	QString strCommand = makeCommand_SetLightValue();
	qDebug() << strCommand;
	sendCommand(strCommand);
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

void MainWindow::closeEvent(QCloseEvent* e)
{
	applySettings();
	writeSettings();
}

void MainWindow::sendCommand(const QString& command)
{
	webSocket.sendTextMessage(command);
}

void MainWindow::applySettings()
{
	serverIp = serverAddrEdit->text();
	serverPort = serverPortEdit->text();
}

void MainWindow::updateSettings()
{
	lightIntensitySlider->setValue(lightIntensity);
}

void MainWindow::readSettings()
{
	QSettings settings;

	settings.beginGroup("MainWindow");
	QString ip = settings.value("serverIp").toString();
	if (!ip.isEmpty()) {
		serverIp = ip;
	}

	QString port = settings.value("serverPort").toString();
	if (!port.isEmpty()) {
		serverPort = port;
	}

	settings.endGroup();
}

void MainWindow::writeSettings()
{
	QSettings settings;

	settings.beginGroup("MainWindow");
	settings.setValue("serverIp", serverIp);
	settings.setValue("serverPort", serverPort);
	settings.endGroup();
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

QString MainWindow::makeCommand_SetScriptMode()
{
	QString strXml;
	QXmlStreamWriter writer(&strXml);
	writer.setAutoFormatting(true);
	writer.writeStartDocument();

	writer.writeStartElement("command");
	writer.writeAttribute("opcode", "setScriptMode");
	writer.writeEndElement(); // command

	writer.writeEndElement();

	return strXml;
}

QString MainWindow::makeCommand_SetManualMode()
{
	QString strXml;
	QXmlStreamWriter writer(&strXml);
	writer.setAutoFormatting(true);
	writer.writeStartDocument();

	writer.writeStartElement("command");
	writer.writeAttribute("opcode", "setManualMode");
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
