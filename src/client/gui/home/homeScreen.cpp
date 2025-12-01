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
#include <QScrollArea>
#include "homeScreen.h"

#include "../login/loginScreen.h"
#include "../../util/clearLayout.h"

#include <bits/stdc++.h>
using namespace std;

QWidget *homeWindow;
QVBoxLayout *homeLayout;
QPushButton *accountLabel;
QPushButton *logOutBtn;
QWidget *settingWidget;

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
    int screenWidth = window->width();
    int screenHeight = window->height();

    // 1. Create layout and add widgets
    homeLayout = new QVBoxLayout();
    homeLayout->setAlignment(Qt::AlignTop);
    homeLayout->setContentsMargins(0, 0, 0, 0);

    // 1.1. Creater header widget, layout
    int topHeight = 50;
    QHBoxLayout *topLayout = new QHBoxLayout();
    topLayout->setContentsMargins(0, 0, 0, 0);
    topLayout->addStretch();

    QWidget *topWidget = new QWidget();
    topWidget->setStyleSheet("background-color: grey; border: 1px solid blue;");
    topWidget->setFixedSize(screenWidth, topHeight);
    topWidget->setLayout(topLayout);

    homeLayout->addWidget(topWidget);

    // 1.1.1. Create user icon
    int accountLabelWidth = 60;
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

    // 1.1.1.1. Create setting box when click user icon
    int settingWidgetWidth = screenWidth * 0.1;
    int settingWidgetOptionHeight = 30;

    settingWidget = new QWidget(nullptr, Qt::Popup);
    settingWidget->setStyleSheet("background-color: #f5f5f5; border: 1px solid rgba(83, 83, 83, 1); border-radius: 0px; font-weight: bold;");
    settingWidget->move(window->width() - settingWidgetWidth - accountLabelWidth, topWidget->height() + 40);
    cout << topWidget->height() << endl;
    settingWidget->hide();

    // Create setting box layout
    QVBoxLayout *settingLayout = new QVBoxLayout();
    settingLayout->setAlignment(Qt::AlignTop);
    settingLayout->setContentsMargins(0, 0, 0, 0);

    settingWidget->setLayout(settingLayout);

    // 1.1.1.1.1. Log out setting option
    logOutBtn = new QPushButton("Log Out");
    logOutBtn->setFixedSize(settingWidgetWidth, settingWidgetOptionHeight);

    settingLayout->addWidget(logOutBtn);

    // 1.2. Create body widget, layout
    QWidget *bodyWidget = new QWidget();
    QHBoxLayout *bodyLayout = new QHBoxLayout(bodyWidget);
    bodyLayout->setContentsMargins(0, 0, 0, 0);

    homeLayout->addWidget(bodyWidget);

    // 1.2.1 Content display (fills remaining width)
    QWidget *contentDisplay = new QWidget();
    contentDisplay->setStyleSheet("background-color: #90EE90; border: 1px solid purple;");
    contentDisplay->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    bodyLayout->addWidget(contentDisplay);

    // 1.2.2. Friend display (fixed width, vertical scroll)
    int friendColumnWidth = screenWidth * 0.15;

    QWidget *friendDisplay = new QWidget();
    QVBoxLayout *friendDisplayLayout = new QVBoxLayout(friendDisplay);
    friendDisplayLayout->setAlignment(Qt::AlignTop);
    friendDisplayLayout->setContentsMargins(0, 0, 0, 0);
    friendDisplayLayout->setSpacing(0);

    for (int i = 0; i < 30; i++)
    {
        // TODO: replace label with friend display widget
        QLabel *label = new QLabel("Item " + QString::number(i));
        label->setStyleSheet("background-color: purple; border: 1px solid black;");
        label->setFixedHeight(screenHeight * 0.05);
        label->setMargin(0);
        label->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
        friendDisplayLayout->addWidget(label);
    }

    QScrollArea *scrollArea = new QScrollArea();
    scrollArea->setWidgetResizable(true);
    scrollArea->setFixedWidth(friendColumnWidth);
    scrollArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    scrollArea->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    scrollArea->setWidget(friendDisplay);

    bodyLayout->addWidget(scrollArea);

    homeWindow->setLayout(homeLayout);

    setUpHomeEvents();
    cout << "Entered home screen" << endl;
};