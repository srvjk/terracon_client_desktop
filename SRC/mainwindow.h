#pragma once

#include <QWidget>
#include <QtWebSockets/QWebSocket>

class QPushButton;
class QLineEdit;
class QSvgWidget;
class QSlider;

class MainWindow : public QWidget
{
	Q_OBJECT

public:
	MainWindow(QWidget* parent = nullptr);

private slots:
	void onConnect();
	void onUpdateFromServer(); // запросить свежую информацию с сервера
	void onRequestServerShutdown();
	void onScriptModeButton();
	void onManualModeButton();
	void onShutdownServer(); // команда серверу завершить работу
	void onLightIntensitySliderValueChanged(int);
	void onLightSet();
	void onLightOff();
	void onLightMax();
	void onWaterOn();
	void onWaterOff();
	void onConnected();
	void onDisconnected();
	void onTextMessageReceived(QString message);
	void messageForUser(const QString& message);

protected:
	void closeEvent(QCloseEvent* e);

private:
	QString makeCommand_UpdateFromServer();
	QString makeCommand_ServerShutdown();
	QString makeCommand_SetScriptMode();
	QString makeCommand_SetManualMode();
	QString makeCommand_SetLightValue();
	QString makeCommand_WaterOn();
	QString makeCommand_WaterOff();
	void sendCommand(const QString& command);
	void applySettings(); // записать настройки из Gui во внутренние переменные
	void updateSettings(); // отобразить в Gui настройки из внутренних переменных
	void readSettings();
	void writeSettings();

private:
	QLineEdit *serverAddrEdit = nullptr;
	QLineEdit *serverPortEdit = nullptr;
	QPushButton *serverConnectButton = nullptr;
	QPushButton* updateFromServerButton = nullptr;
	QPushButton* serverShutdownButton = nullptr;
	QPushButton* scriptModeButton = nullptr;
	QPushButton* manualModeButton = nullptr;
	QSlider* lightIntensitySlider = nullptr;
	QPushButton *setLightValueButton = nullptr;
	QPushButton* lightOffButton = nullptr;
	QPushButton* lightMaxButton = nullptr;
	QPushButton* waterOnButton = nullptr;
	QPushButton* waterOffButton = nullptr;
	QWebSocket webSocket;
	QSvgWidget *connectionIndicator = nullptr;

	QString serverIp = "127.0.0.1";
	QString serverPort = "8001";
	int lightIntensity = 0;
};
