#include <bits/stdc++.h>
#include <QApplication>
#include "gui/login/loginScreen.h"
#include "util/signalHandler.h"
#include "../server/db/createdb.h"
using namespace std;

const int textSize = 16;

QWidget *window;
int main(int argc, char *argv[])
{
    createdb();
    QApplication app(argc, argv);
    window = new QWidget();
    window->setStyleSheet(QString("QLabel {font-size: %1px;}").arg(textSize));
    loginScreen(window);
    window->showMaximized();
    window->show();

    QObject::connect(&app, &QApplication::aboutToQuit, []()
                     { signalHandler(-1); });

    signal(SIGTERM, signalHandler);
    signal(SIGTSTP, signalHandler);
    signal(SIGINT, signalHandler);
    return app.exec();
}
