#include <bits/stdc++.h>
#include <QApplication>
#include "gui/login/loginScreen.h"
using namespace std;

const int textSize = 16;

QWidget *window;
int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    window = new QWidget();
    window->setStyleSheet(QString("QLabel {font-size: %1px;}").arg(textSize));
    loginScreen(window);
    window->showMaximized();
    window->show();
    return app.exec();
}
