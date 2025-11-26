#include <QWidget>
#include <QLabel>
#include <QVBoxLayout>
#include "homeScreen.h"
#include <bits/stdc++.h>
using namespace std;

QWidget *homeWindow;
QWidget *homePanel;

void homeScreen(QWidget *window)
{
    homeWindow = window;
    window->setWindowTitle("Home Screen");

    // Remove any previous content/layout
    if (window->layout())
    {
        QLayout *oldLayout = window->layout();
        QLayoutItem *item;
        while ((item = oldLayout->takeAt(0)) != nullptr)
        {
            if (item->widget())
                delete item->widget(); // remove old widgets
            delete item;
        }
        delete oldLayout;
    }

    // Create a new panel
    homePanel = new QWidget(window);

    // Create layout and add widgets
    QVBoxLayout *homeLayout = new QVBoxLayout(homePanel);
    QLabel *homeLabel = new QLabel("Welcome to the Home Screen!", homePanel);
    homeLayout->addWidget(homeLabel);

    // Set panel as the central layout of the window
    QVBoxLayout *windowLayout = new QVBoxLayout(window);
    windowLayout->addWidget(homePanel);
    window->setLayout(windowLayout);

    window->show(); // Refresh display
}
