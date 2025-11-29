#include <QWidget>
#include <QApplication>
#include <QScreen>
#include <QLabel>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QIcon>
#include <QPushButton>
#include <QPainter>
#include <QTimer>
#include <QMouseEvent>
#include "homeScreen.h"

#include "../login/loginScreen.h"
// #include "../../../shared/resources.cpp"
#include "../../util/clearLayout.h"

#include <bits/stdc++.h>
using namespace std;

QWidget *homeWindow;
QVBoxLayout *homeLayout;
QPushButton *accountLabel;
QPushButton *logOutBtn;
QWidget *settingWidget;
QWidget *overlay;

void setUpHomeEvents()
{
    QObject::connect(accountLabel, &QPushButton::clicked, []()
                     { 
                    cout << "Clicked account icon" << endl;
                    if (settingWidget->isVisible()){
                        settingWidget->hide();
                    }else {
                        settingWidget->show();
                        settingWidget->raise();
                    } });

    QObject::connect(logOutBtn, &QPushButton::clicked, []()
                     {
                    cout << "Log out button clicked" << endl;
                    clearLayout(homeWindow);
                    loginScreen(homeWindow);
                    return; });
}

void homeScreen(QWidget *window)
{

    QString elementStyle = "background-color: #f5f5f5; border: none;";
    homeWindow = window;
    window->setWindowTitle("Home Screen");

    // Screen size
    QScreen *screen = QApplication::primaryScreen();
    QRect screenGeometry = screen->geometry();
    int screenWidth = screenGeometry.width();
    int screenHeight = screenGeometry.height();

    // Create layout and add widgets
    homeLayout = new QVBoxLayout();
    homeLayout->setAlignment(Qt::AlignTop);

    // Creater header layout and overlay
    int topHeight = 50;
    QHBoxLayout *topLayout = new QHBoxLayout();
    topLayout->addStretch();

    // Create user icon
    int accountLabelWidth = 50;
    accountLabel = new QPushButton();
    accountLabel->setFixedSize(accountLabelWidth, topHeight);
    accountLabel->setStyleSheet(elementStyle);

    QPixmap pix(":images/user_icon.png");
    QPixmap scaledPix = pix.scaled(
        accountLabel->size(),    // scale to label size
        Qt::IgnoreAspectRatio,   // scale exactly (may stretch)
        Qt::SmoothTransformation // smooth scaling
    );
    QIcon icon(scaledPix);
    accountLabel->setIcon(icon);
    accountLabel->setIconSize(scaledPix.size());
    topLayout->addWidget(accountLabel);

    // Create overlay for setting box
    overlay = new QWidget(window);
    overlay->setAttribute(Qt::WA_NoSystemBackground);
    overlay->setAttribute(Qt::WA_TranslucentBackground);
    overlay->setStyleSheet("background: transparent;");
    overlay->setStyleSheet(elementStyle);
    overlay->setGeometry(0, 0, window->width(), window->height());
    overlay->raise(); // always on top
    overlay->show();

    // Create setting box when click user icon
    int settingWidgetWidth = 120;
    int settingWidgetOptionHeight = 30;

    settingWidget = new QWidget(overlay);
    settingWidget->setStyleSheet("background-color: #f5f5f5; border: 1px solid rgba(83, 83, 83, 1); border-radius: 0px; font-weight: bold;");
    settingWidget->move(window->width() - settingWidgetWidth - accountLabelWidth, accountLabel->height() + 20);
    settingWidget->hide();

    // Log out setting option
    logOutBtn = new QPushButton("Log Out");
    logOutBtn->setFixedSize(settingWidgetWidth, settingWidgetOptionHeight);

    // Create setting box layout
    QVBoxLayout *settingLayout = new QVBoxLayout();
    settingLayout->setAlignment(Qt::AlignTop);
    settingLayout->setContentsMargins(0, 0, 0, 0);
    settingLayout->addWidget(logOutBtn);
    settingWidget->setLayout(settingLayout);

    homeLayout->addLayout(topLayout);

    homeWindow->setLayout(homeLayout);
    QTimer::singleShot(0, [=]()
                       { settingWidget->move(
                             window->width() - settingWidgetWidth - accountLabelWidth,
                             accountLabel->height() + 20); });

    setUpHomeEvents();
    cout << "Entered home screen" << endl;
};