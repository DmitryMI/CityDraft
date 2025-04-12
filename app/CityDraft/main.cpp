#include <QApplication>
#include <QWidget>
#include "UI/MainWindow.h"

using namespace CityDraft;

int main(int argc, char* argv[])
{
    QApplication app(argc, argv);

    MainWindow mainWindow;
    mainWindow.show();

    return app.exec();
}