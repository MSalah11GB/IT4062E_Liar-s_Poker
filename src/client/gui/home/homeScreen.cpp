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
#include <QScrollBar>
#include <QThread>
#include "homeScreen.h"

#include "../login/loginScreen.h"
#include "../../util/clearLayout.h"
#include "../../../server/db/model/user.h"
#include "../../../server/db/queries/user.h"

#include <bits/stdc++.h>
using namespace std;

QWidget *homeWindow;
QVBoxLayout *homeLayout;
QPushButton *accountLabel;
QPushButton *logOutBtn;
QWidget *settingWidget;
QVBoxLayout *friendDisplayLayout;

void changeFriendStatus(int friendId, int online_status)
{
    int friendNum = friendDisplayLayout->count();
    QWidget *friendWidget = nullptr;
    for (int i = 0; i < friendNum; i++)
    {
        friendWidget = friendDisplayLayout->itemAt(i)->widget();
        if (friendWidget->property("friend_id").toInt() == friendId)
        {
            break;
        }
    }
    QLabel *friendStatusLabel = qobject_cast<QLabel *>(friendWidget->property("friend_status_label").value<QObject *>());
    QPushButton *friendInviteBtn =
        qobject_cast<QPushButton *>(friendWidget->property("friend_invite_btn").value<QObject *>());

    if (online_status == 1)
    {
        friendStatusLabel->setText("Online");
        friendInviteBtn->setIcon(QIcon(":images/invite_user_icon.png"));
        friendInviteBtn->setIconSize(friendInviteBtn->size() * 0.35);
        friendInviteBtn->setEnabled(true);
    }
    else if (online_status == 0)
    {
        friendStatusLabel->setText("Offline");
        friendInviteBtn->setIcon(QIcon());
        friendInviteBtn->setEnabled(false);
    }
}
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

void homeScreen(QWidget *window, User user)
{
    cout << "User in home screen " << user.id << ", " << user.username << endl;
    QString elementStyle = "background-color: #f5f5f5; border: none;";
    homeWindow = window;
    window->setWindowTitle("Home Screen");

    // Screen size
    int screenWidth = window->geometry().width();
    int screenHeight = window->geometry().height();

    // 1. Create layout and add widgets
    homeLayout = new QVBoxLayout();
    homeLayout->setAlignment(Qt::AlignTop);
    homeLayout->setContentsMargins(0, 0, 0, 0);
    homeLayout->setSpacing(0);

    // 1.1. Creater header widget, layout
    int topHeight = 60;
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
    accountLabel->setIcon(QIcon(":images/user_icon.png"));
    accountLabel->setIconSize(accountLabel->size());
    topLayout->addWidget(accountLabel);

    // 1.1.1.1. Create setting box when click user icon
    int settingWidgetWidth = screenWidth * 0.1;
    int settingWidgetOptionHeight = 30;

    settingWidget = new QWidget(nullptr, Qt::Popup);
    settingWidget->setStyleSheet("background-color: #f5f5f5; border: 1px solid rgba(83, 83, 83, 1); border-radius: 0px; font-weight: bold;");
    settingWidget->move(window->width() - settingWidgetWidth - accountLabelWidth, topWidget->height() + 40);
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
    bodyWidget->setFixedSize(screenWidth, screenHeight - topHeight);
    QHBoxLayout *bodyLayout = new QHBoxLayout();
    bodyLayout->setContentsMargins(0, 0, 0, 0);
    bodyLayout->setSpacing(0);
    bodyWidget->setLayout(bodyLayout);

    homeLayout->addWidget(bodyWidget);

    // 1.2.1 Content display (fills remaining width)
    QWidget *contentDisplay = new QWidget();
    contentDisplay->setStyleSheet("background-color: #90EE90; border: 1px solid purple;");
    contentDisplay->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    bodyLayout->addWidget(contentDisplay);

    // 1.2.2. Friend display (fixed width, vertical scroll)
    int friendColumnWidth = screenWidth * 0.15;

    QWidget *friendDisplay = new QWidget();
    // friendDisplay->setFixedHeight(screenWidth - topHeight);
    friendDisplay->setStyleSheet("border: 1px solid blue;");
    friendDisplayLayout = new QVBoxLayout(friendDisplay);
    friendDisplayLayout->setAlignment(Qt::AlignTop);
    friendDisplayLayout->setContentsMargins(0, 0, 0, 0);
    friendDisplayLayout->setSpacing(0);

    vector<User> friends = getFriendsOfUser(user.id);
    int friendNum = friends.size();
    int numOfFriendDisplay = 18;
    for (int i = 0; i < friendNum; i++)
    {
        // TODO: replace label with friend display widget
        QWidget *friendWidget = new QWidget();
        friendWidget->setProperty("friend_id", friends[i].id);
        friendWidget->setFixedHeight(screenHeight / numOfFriendDisplay);
        friendWidget->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
        friendWidget->setStyleSheet("background-color: purple; border: 1px solid black;");

        QHBoxLayout *friendWidgetLayout = new QHBoxLayout();
        friendWidgetLayout->setContentsMargins(0, 0, 0, 0);
        friendWidgetLayout->setAlignment(Qt::AlignLeft);
        friendWidgetLayout->setSpacing(0);
        friendWidget->setLayout(friendWidgetLayout);

        QWidget *friendIconWidget = new QWidget();
        friendIconWidget->setFixedSize(screenHeight / numOfFriendDisplay - 2, screenHeight / numOfFriendDisplay - 2);
        friendIconWidget->setStyleSheet(
            "QWidget { "
            "border-image: url(:/images/user_icon.png) 0 0 0 0 stretch stretch;"
            "}");
        friendWidgetLayout->addWidget(friendIconWidget);

        QWidget *friendInfoWidget = new QWidget();
        friendInfoWidget->setFixedHeight(screenHeight / numOfFriendDisplay - 2);
        friendInfoWidget->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
        friendInfoWidget->setStyleSheet("background-color: yellow; border: none");

        QVBoxLayout *friendInfoWidgetLayout = new QVBoxLayout();
        friendInfoWidgetLayout->setContentsMargins(10, 0, 0, 0);
        friendInfoWidgetLayout->setSpacing(0);
        friendInfoWidget->setLayout(friendInfoWidgetLayout);

        QString friendUsername = QString::fromStdString(friends[i].username);
        QLabel *friendUsernameLabel = new QLabel(friendUsername);
        friendInfoWidgetLayout->addWidget(friendUsernameLabel);

        QString friendStatus = friends[i].online_status == 1 ? "Online" : "Offline";
        QLabel *friendStatusLabel = new QLabel(friendStatus);
        friendStatusLabel->setStyleSheet("font-size: 12px");
        friendInfoWidgetLayout->addWidget(friendStatusLabel);
        friendWidget->setProperty("friend_status_label", QVariant::fromValue(static_cast<QObject *>(friendStatusLabel)));

        friendWidgetLayout->addWidget(friendInfoWidget);

        QPushButton *friendInviteBtn = new QPushButton();
        friendInviteBtn->setFixedSize(screenHeight / numOfFriendDisplay - 2, screenHeight / numOfFriendDisplay - 2);
        friendInviteBtn->setStyleSheet("background-color: red; border: 1px solid white;");
        friendInviteBtn->setProperty("user_id", user.id);
        friendInviteBtn->setProperty("friend_id", friends[i].id);
        QObject::connect(friendInviteBtn, &QPushButton::clicked, [friendInviteBtn]()
                         { cout << "send invite from user id: " << friendInviteBtn->property("user_id").toInt() << " to friend id " << friendInviteBtn->property("friend_id").toInt() << endl; });
        friendInviteBtn->setEnabled(false);
        if (friends[i].online_status == 1)
        {
            friendInviteBtn->setIcon(QIcon(":images/invite_user_icon.png"));
            friendInviteBtn->setIconSize(friendInviteBtn->size() * 0.35);
            friendInviteBtn->setEnabled(true);
        }
        friendWidget->setProperty("friend_invite_btn", QVariant::fromValue(static_cast<QObject *>(friendInviteBtn)));

        friendWidgetLayout->addWidget(friendInviteBtn);

        friendDisplayLayout->addWidget(friendWidget);
    }

    QScrollArea *scrollArea = new QScrollArea();
    scrollArea->setWidgetResizable(true);
    scrollArea->setFixedWidth(friendColumnWidth);
    scrollArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    scrollArea->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    scrollArea->setStyleSheet("border: 2px solid red;");
    scrollArea->setWidget(friendDisplay);
    bodyLayout->addWidget(scrollArea);

    int friendRequestWidgetHeight = 50;
    int friendRequestWidgetWidth = friendColumnWidth - scrollArea->verticalScrollBar()->sizeHint().width();
    QWidget *friendRequestWidget = new QWidget(bodyWidget);
    friendRequestWidget->setStyleSheet("background-color: green; border: 1px solid black;");
    friendRequestWidget->setFixedSize(friendRequestWidgetWidth, friendRequestWidgetHeight);
    friendRequestWidget->move(bodyWidget->width() - friendColumnWidth, bodyWidget->height() - friendRequestWidgetHeight);
    friendRequestWidget->raise();
    friendRequestWidget->show();

    homeWindow->setLayout(homeLayout);

    setUpHomeEvents();
    cout << "Entered home screen" << endl;
};