#include <bits/stdc++.h>
#include <QApplication>
#include "gui/login/loginScreen.h"
using namespace std;

QWidget *window;
int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    window = loginScreen();
    window->show();
    window->showMaximized();
    return app.exec();
}
