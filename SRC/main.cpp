#include <QApplication>
#include <QSettings>
#include "mainwindow.h"

using namespace std;

int main(int argc, char* argv[])
{
    QApplication app(argc, argv);

    QApplication::setOrganizationName("SergeVjk");
    QApplication::setOrganizationDomain("sngspace.com");
    QApplication::setApplicationName("TerraCon Desktop");

    MainWindow* mainWindow = new MainWindow();
    mainWindow->show();

    return app.exec();
}
