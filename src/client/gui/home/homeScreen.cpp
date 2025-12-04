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
#include "../../util/deleteWidgetLayout.h"
#include "../../util/clearLayout.h"
#include "../../../server/db/model/user.h"
#include "../../../server/db/queries/user.h"
#include "../../../server/db/queries/friends.h"

#include <bits/stdc++.h>
using namespace std;

QWidget *homeWindow;
QVBoxLayout *homeLayout;
QPushButton *accountLabel;
QPushButton *viewCareerBtn;
QWidget *userCareerWidget = nullptr;
QPushButton *logOutBtn;
QWidget *settingWidget;
QWidget *bodyWidget;
QScrollArea *scrollArea;
QVBoxLayout *friendDisplayLayout;
QWidget *friendRequestWidget;
QPushButton *leftButton;
QPushButton *friendRequestConfirmationBtn;
QLabel *friendRequestUsernameLabel;
QPushButton *rightButton;
User user;
vector<User> friends;
vector<User> friendRequests;

const int numOfFriendDisplay = 14;

int titleBarHeight;
int screenWidth;
int screenHeight;
int topHeight;
int accountLabelWidth;
int settingWidgetWidth;
int settingWidgetHeight;
int settingWidgetOptionHeight;
int bodyHeight;
int friendColumnWidth;
int friendWidgetHeight;
int friendRequestIndex = 0;

QWidget *setUpUserCareerWidget(User user)
{
    QWidget *careerWidget = new QWidget();
    careerWidget->setStyleSheet("border: none;");
    careerWidget->setFixedSize(screenWidth, screenHeight - topHeight);

    QHBoxLayout *careerLayout = new QHBoxLayout();
    careerLayout->setContentsMargins(0, 50, 0, 0);
    careerLayout->setAlignment(Qt::AlignLeft);
    careerLayout->setAlignment(Qt::AlignTop);
    careerLayout->setSpacing(0);
    careerWidget->setLayout(careerLayout);

    QPushButton *returnBtn = new QPushButton("Return");
    returnBtn->setFixedSize(screenWidth * 0.1, (screenHeight - topHeight) * 0.1);
    returnBtn->setIcon(QIcon(":images/return_icon.png"));
    returnBtn->setIconSize(QSize(returnBtn->width() * 0.3, returnBtn->height() * 0.8));
    QObject::connect(returnBtn, &QPushButton::clicked, [careerWidget]()
                     {  cout << "return to home screen" << endl;
                        careerWidget->deleteLater();
                        bodyWidget->show();
                        homeLayout->addWidget(bodyWidget);
                        viewCareerBtn->setEnabled(true); });
    careerLayout->addWidget(returnBtn);

    QWidget *careerContentWidget = new QWidget();
    return careerWidget;
}
void setUpFriendRequestsWidgetState()
{
    int numOfFriendRequests = friendRequests.size();
    if (numOfFriendRequests == 0)
    {
        friendRequestWidget->hide();
    }
    else
    {
        friendRequestWidget->raise();
        friendRequestWidget->show();
        friendRequestUsernameLabel->setText(QString::fromStdString(friendRequests.at(friendRequestIndex).username));
        friendRequestConfirmationBtn->setProperty("friend_id", friendRequests.at(friendRequestIndex).id);
    }
}
void setUpFriendRequestsWidget()
{
    int friendNum = friends.size();
    int friendRequestWidgetHeight = 100;
    int friendRequestWidgetWidth;
    if (friendNum > numOfFriendDisplay)
    {
        friendRequestWidgetWidth = friendColumnWidth - scrollArea->verticalScrollBar()->sizeHint().width();
    }
    else
    {
        friendRequestWidgetWidth = friendColumnWidth;
    }

    friendRequestWidget = new QWidget(bodyWidget);
    friendRequestWidget->setStyleSheet("background-color: green; border: 1px solid black;");
    friendRequestWidget->setFixedSize(friendRequestWidgetWidth, friendRequestWidgetHeight);
    friendRequestWidget->move(bodyWidget->width() - friendColumnWidth, bodyWidget->height() - friendRequestWidgetHeight);
    friendRequestWidget->raise();
    friendRequestWidget->show();

    QHBoxLayout *friendRequestLayout = new QHBoxLayout();
    friendRequestLayout->setContentsMargins(0, 0, 0, 0);
    friendRequestLayout->setSpacing(0);
    friendRequestLayout->setAlignment(Qt::AlignLeft);
    friendRequestWidget->setLayout(friendRequestLayout);

    leftButton = new QPushButton();
    leftButton->setStyleSheet("background: none; border: none;");
    leftButton->setFixedWidth(friendRequestWidgetWidth * 0.1);
    leftButton->setIcon(QIcon(":images/left_arrowhead.png"));
    leftButton->setIconSize(leftButton->size() * 0.35);
    friendRequestLayout->addWidget(leftButton);

    QWidget *friendRequestContentWidget = new QWidget();
    friendRequestContentWidget->setFixedWidth(friendRequestWidgetWidth * 0.8);

    QVBoxLayout *friendRequestContentLayout = new QVBoxLayout();
    friendRequestContentLayout->setContentsMargins(0, 0, 0, 0);
    friendRequestContentLayout->setSpacing(0);
    friendRequestContentLayout->setAlignment(Qt::AlignTop);
    friendRequestContentWidget->setLayout(friendRequestContentLayout);
    friendRequestLayout->addWidget(friendRequestContentWidget);

    QWidget *friendRequestInfoWidget = new QWidget();
    friendRequestInfoWidget->setFixedHeight(friendRequestWidgetHeight * 0.5);

    QHBoxLayout *friendRequestInfoLayout = new QHBoxLayout();
    friendRequestInfoLayout->setContentsMargins(0, 0, 0, 0);
    friendRequestInfoLayout->setSpacing(0);
    friendRequestInfoLayout->setAlignment(Qt::AlignLeft);
    friendRequestInfoWidget->setLayout(friendRequestInfoLayout);
    friendRequestContentLayout->addWidget(friendRequestInfoWidget);

    QWidget *friendRequestIconWidget = new QWidget();
    friendRequestIconWidget->setFixedSize(friendRequestWidgetHeight * 0.5 - 2, friendRequestWidgetHeight * 0.5 - 2);
    friendRequestIconWidget->setStyleSheet(
        "QWidget { "
        "border-image: url(:/images/user_icon.png) 0 0 0 0 stretch stretch;"
        "}");
    friendRequestInfoLayout->addWidget(friendRequestIconWidget);

    QWidget *friendRequestTextWidget = new QWidget();
    friendRequestTextWidget->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    friendRequestInfoLayout->addWidget(friendRequestTextWidget);

    QVBoxLayout *friendRequestTextLayout = new QVBoxLayout();
    friendRequestTextLayout->setContentsMargins(0, 0, 0, 0);
    friendRequestTextLayout->setSpacing(0);
    friendRequestTextLayout->setAlignment(Qt::AlignTop);
    friendRequestTextWidget->setLayout(friendRequestTextLayout);

    friendRequestUsernameLabel = new QLabel();
    friendRequestUsernameLabel->setFixedHeight(friendRequestWidgetHeight * 0.25);
    friendRequestTextLayout->addWidget(friendRequestUsernameLabel);

    QLabel *friendRequestMsgLabel = new QLabel("want to be friends");
    friendRequestMsgLabel->setStyleSheet("font-size: 12px;");
    friendRequestMsgLabel->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    friendRequestTextLayout->addWidget(friendRequestMsgLabel);

    friendRequestConfirmationBtn = new QPushButton("ADD FRIEND");
    friendRequestConfirmationBtn->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    friendRequestConfirmationBtn->setStyleSheet("background: none; border: none; font-size: 16px;");
    friendRequestContentLayout->addWidget(friendRequestConfirmationBtn);

    rightButton = new QPushButton();
    rightButton->setStyleSheet("background: none; border: none;");
    rightButton->setFixedWidth(friendRequestWidgetWidth * 0.1);
    rightButton->setIcon(QIcon(":images/right_arrowhead.png"));
    rightButton->setIconSize(rightButton->size() * 0.35);
    friendRequestLayout->addWidget(rightButton);
}

QWidget *setUpFriendInviteWidget(int friendIndex)
{
    cout << friendWidgetHeight << endl;
    QWidget *friendWidget = new QWidget();
    friendWidget->setProperty("friend_id", friends[friendIndex].id);
    friendWidget->setFixedHeight(friendWidgetHeight);
    friendWidget->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    friendWidget->setStyleSheet("background-color: purple; border: 1px solid black;");

    QHBoxLayout *friendWidgetLayout = new QHBoxLayout();
    friendWidgetLayout->setContentsMargins(0, 0, 0, 0);
    friendWidgetLayout->setAlignment(Qt::AlignLeft);
    friendWidgetLayout->setSpacing(0);
    friendWidget->setLayout(friendWidgetLayout);

    QWidget *friendIconWidget = new QWidget();
    friendIconWidget->setFixedSize(friendWidgetHeight - 2, friendWidgetHeight - 2);
    friendIconWidget->setStyleSheet(
        "QWidget { "
        "border-image: url(:/images/user_icon.png) 0 0 0 0 stretch stretch;"
        "}");
    friendWidgetLayout->addWidget(friendIconWidget);

    QWidget *friendInfoWidget = new QWidget();
    friendInfoWidget->setFixedHeight(friendWidgetHeight - 2);
    friendInfoWidget->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    friendInfoWidget->setStyleSheet("background-color: yellow; border: none");

    QVBoxLayout *friendInfoWidgetLayout = new QVBoxLayout();
    friendInfoWidgetLayout->setContentsMargins(10, 0, 0, 0);
    friendInfoWidgetLayout->setSpacing(0);
    friendInfoWidget->setLayout(friendInfoWidgetLayout);

    QString friendUsername = QString::fromStdString(friends[friendIndex].username);
    QLabel *friendUsernameLabel = new QLabel(friendUsername);
    friendInfoWidgetLayout->addWidget(friendUsernameLabel);

    QString friendElo = QString::number(friends[friendIndex].elo) + " points";
    QLabel *friendEloLabel = new QLabel(friendElo);
    friendEloLabel->setStyleSheet("font-size: 14px");
    friendInfoWidgetLayout->addWidget(friendEloLabel);

    QString friendStatus = friends[friendIndex].online_status == 1 ? "Online" : "Offline";
    QLabel *friendStatusLabel = new QLabel(friendStatus);
    friendStatusLabel->setStyleSheet("font-size: 12px");
    friendInfoWidgetLayout->addWidget(friendStatusLabel);
    friendWidget->setProperty("friend_status_label", QVariant::fromValue(static_cast<QObject *>(friendStatusLabel)));

    friendWidgetLayout->addWidget(friendInfoWidget);

    QPushButton *friendInviteBtn = new QPushButton();
    friendInviteBtn->setFixedSize(friendWidgetHeight - 2, friendWidgetHeight - 2);
    friendInviteBtn->setStyleSheet("background-color: red; border: 1px solid white;");
    friendInviteBtn->setProperty("friend_id", friends[friendIndex].id);
    QObject::connect(friendInviteBtn, &QPushButton::clicked, [friendInviteBtn]()
                     { cout << "send invite from user id: " << user.id << " to friend id " << friendInviteBtn->property("friend_id").toInt() << endl; });
    friendInviteBtn->setEnabled(false);
    if (friends[friendIndex].online_status == 1)
    {
        friendInviteBtn->setIcon(QIcon(":images/invite_user_icon.png"));
        friendInviteBtn->setIconSize(friendInviteBtn->size() * 0.35);
        friendInviteBtn->setEnabled(true);
    }
    friendWidget->setProperty("friend_invite_btn", QVariant::fromValue(static_cast<QObject *>(friendInviteBtn)));

    friendWidgetLayout->addWidget(friendInviteBtn);

    friendWidget->setContextMenuPolicy(Qt::CustomContextMenu);
    QObject::connect(friendWidget, &QWidget::customContextMenuRequested, [friendWidget]()
                     {
                        cout << "Right-click at widget with friend id " << friendWidget->property("friend_id").toInt() << endl;

                        QPoint friendWidgetCoord = friendWidget->mapToGlobal(friendWidget->rect().topLeft());
                        int friendOptionsWidgetWidth = friendWidgetCoord.x();
                        int friendOptionsWidgetHeight = friendWidgetCoord.y();

                        cout << friendOptionsWidgetWidth << " " << friendOptionsWidgetHeight << endl;
                        QWidget *friendOptionsWidget = new QWidget(homeWindow, Qt::Popup);
                        friendOptionsWidget->setAttribute(Qt::WA_DeleteOnClose);
                        friendOptionsWidget->setStyleSheet("background-color: white; border: 1px solid black;");
                        friendOptionsWidget->setFixedWidth(friendColumnWidth);
                        cout << friendWidgetHeight << endl;
                        friendOptionsWidget->move(friendOptionsWidgetWidth - friendColumnWidth - 10, friendOptionsWidgetHeight + friendWidgetHeight * 0.5);
                        friendOptionsWidget->raise();
                        friendOptionsWidget->show(); 
                        
                        QVBoxLayout *friendOptionsLayout = new QVBoxLayout();
                        friendOptionsLayout->setContentsMargins(0, 0, 0, 0);
                        friendOptionsLayout->setSpacing(0);
                        friendOptionsLayout->setAlignment(Qt::AlignTop);
                        friendOptionsLayout->setSizeConstraint(QLayout::SetFixedSize); 
                        friendOptionsWidget->setLayout(friendOptionsLayout);

                        int friendOptionHeight = 30;
                        QPushButton *removeFriendButton = new QPushButton("Remove Friend");
                        removeFriendButton->setFixedSize(friendColumnWidth, friendOptionHeight);
                        removeFriendButton->setStyleSheet("text-align: left; padding-left: 10px;");
                        QObject::connect(removeFriendButton, &QPushButton::clicked, [friendWidget, friendOptionsWidget]()
                                        {
                                            friendOptionsWidget->deleteLater();
                                            cout << "remove friend id " << friendWidget->property("friend_id").toInt() << " of user id " << user.id << endl;
                                            removeFriend(user.id, friendWidget->property("friend_id").toInt());
                                            int friendNum = friends.size();
                                            for (int i = 0; i < friendNum; i++)
                                            {
                                                if (friends.at(i).id == friendWidget->property("friend_id").toInt())
                                                {
                                                    friends.erase(friends.begin() + i);
                                                    QWidget *friendWidgetToErase = friendDisplayLayout->takeAt(i)->widget();
                                                    if (friendWidgetToErase)
                                                    {
                                                        friendWidgetToErase->deleteLater();
                                                    }
                                                    break;
                                                }
                                            } });
                        friendOptionsLayout->addWidget(removeFriendButton); });
    return friendWidget;
}
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
void setUpFriendDisplay()
{
    clearLayout(friendDisplayLayout);
    friends = getFriendsOfUser(user.id);
    friendWidgetHeight = bodyHeight / numOfFriendDisplay;
    int friendNum = friends.size();
    for (int i = 0; i < friendNum; i++)
    {
        QWidget *friendWidget = setUpFriendInviteWidget(i);
        friendDisplayLayout->addWidget(friendWidget);
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
    QObject::connect(viewCareerBtn, &QPushButton::clicked, []()
                     { cout << "view career button clicked" << endl; 
                    bodyWidget->hide();
                    homeLayout->removeWidget(bodyWidget);
                    viewCareerBtn->setEnabled(false);
                    homeLayout->addWidget(userCareerWidget = setUpUserCareerWidget(user)); });
    QObject::connect(logOutBtn, &QPushButton::clicked, []()
                     {
                    cout << "Log out button clicked" << endl;
                    deleteWidgetLayout(homeWindow);
                    loginScreen(homeWindow);
                    return; });

    QObject::connect(leftButton, &QPushButton::clicked, []()
                     { cout << "Left button clicked" << endl;
                    friendRequestIndex--;
                    if (friendRequestIndex < 0) friendRequestIndex = static_cast<int>(friendRequests.size()) - 1; 
                    cout << friendRequestIndex << endl;
                    setUpFriendRequestsWidgetState(); });

    QObject::connect(rightButton, &QPushButton::clicked, []()
                     { cout << "Right button clicked" << endl;
                    friendRequestIndex++;
                    if (friendRequestIndex == static_cast<int>(friendRequests.size())) friendRequestIndex = 0; 
                    cout << friendRequestIndex << endl;
                    setUpFriendRequestsWidgetState(); });
    QObject::connect(friendRequestConfirmationBtn, &QPushButton::clicked, []()
                     {
                         cout << "confirm friend request clicked" << endl;
                         int userId = user.id;
                         int friendId = friendRequestConfirmationBtn->property("friend_id").toInt();
                         confirmFriendRequest(userId, friendId, friendId);
                         int numOfFriendRequests = friendRequests.size();
                         for (int i = 0; i < numOfFriendRequests; i++)
                         {
                             if (friendRequests.at(i).id == friendId)
                             {
                                User newFriend = friendRequests.at(i);
                                friendRequests.erase(friendRequests.begin() + i); 
                                if (friendRequestIndex >= static_cast<int>(friendRequests.size())) {
                                    friendRequestIndex = friendRequests.size() - 1;
                                }
                                if (friendRequestIndex < 0)
                                    friendRequestIndex = 0;
                                friends.push_back(newFriend); 
                                sort(friends.begin(), friends.end(), [](User &a,User& b){
                                    return a.username < b.username;
                                });                
                                setUpFriendDisplay();
                                break;
                             }
                         }

                         setUpFriendRequestsWidgetState(); });
}

void homeScreen(QWidget *window, User _user)
{
    user = _user;
    cout << "User in home screen " << user.id << ", " << user.username << endl;
    QString elementStyle = "background-color: #f5f5f5; border: none;";
    homeWindow = window;
    window->setWindowTitle("Home Screen");

    // Screen size
    screenWidth = window->geometry().width();
    screenHeight = window->geometry().height();

    titleBarHeight = window->frameGeometry().height() - screenHeight;

    // 1. Create layout and add widgets
    homeLayout = new QVBoxLayout();
    homeLayout->setAlignment(Qt::AlignTop);
    homeLayout->setContentsMargins(0, 0, 0, 0);
    homeLayout->setSpacing(0);

    // 1.1. Creater header widget, layout
    topHeight = 60;
    QHBoxLayout *topLayout = new QHBoxLayout();
    topLayout->setContentsMargins(0, 0, 0, 0);
    topLayout->addStretch();

    QWidget *topWidget = new QWidget();
    topWidget->setStyleSheet("background-color: grey; border: 1px solid blue;");
    topWidget->setFixedSize(screenWidth, topHeight);
    topWidget->setLayout(topLayout);

    homeLayout->addWidget(topWidget);

    // Create user information widget

    int accountInfoWidth = 200;
    QWidget *accountInfoWidget = new QWidget();
    accountInfoWidget->setStyleSheet("border: none;");
    accountInfoWidget->setFixedSize(accountInfoWidth, topHeight);

    QVBoxLayout *accountInfoLayout = new QVBoxLayout();
    accountInfoLayout->setContentsMargins(0, 0, 0, 0);
    accountInfoLayout->setSpacing(0);
    accountInfoLayout->setAlignment(Qt::AlignTop);
    accountInfoLayout->setAlignment(Qt::AlignRight);
    accountInfoWidget->setLayout(accountInfoLayout);

    QLabel *accountNameLabel = new QLabel(QString::fromStdString(user.username));
    accountInfoLayout->addWidget(accountNameLabel);

    cout << "User elo " << user.elo << endl;
    QLabel *accountEloLabel = new QLabel(QString::number(user.elo) + " points");
    accountInfoLayout->addWidget(accountEloLabel);
    cout << "Account elo label " << accountEloLabel->text().toStdString() << endl;

    topLayout->addWidget(accountInfoWidget);

    // 1.1.1. Create user icon
    accountLabelWidth = 60;
    accountLabel = new QPushButton();
    accountLabel->setFixedSize(accountLabelWidth, topHeight);
    accountLabel->setStyleSheet(elementStyle);
    accountLabel->setIcon(QIcon(":images/user_icon.png"));
    accountLabel->setIconSize(accountLabel->size());
    topLayout->addWidget(accountLabel);

    // 1.1.1.1. Create setting box when click user icon
    settingWidgetWidth = screenWidth * 0.1;
    settingWidgetOptionHeight = 30;

    settingWidget = new QWidget(homeWindow, Qt::Popup);
    settingWidget->setStyleSheet("background-color: #f5f5f5; border: 1px solid rgba(83, 83, 83, 1); border-radius: 0px; font-weight: bold;");
    settingWidget->move(window->width() - settingWidgetWidth - accountLabelWidth, topWidget->height() + 40);
    settingWidget->hide();

    // Create setting box layout
    QVBoxLayout *settingLayout = new QVBoxLayout();
    settingLayout->setAlignment(Qt::AlignTop);
    settingLayout->setContentsMargins(0, 0, 0, 0);
    settingLayout->setSpacing(0);

    settingWidget->setLayout(settingLayout);

    // view career button
    viewCareerBtn = new QPushButton("viewCareer");
    viewCareerBtn->setFixedSize(settingWidgetWidth, settingWidgetOptionHeight);
    settingLayout->addWidget(viewCareerBtn);

    // 1.1.1.1.1. Log out setting option
    logOutBtn = new QPushButton("Log Out");
    logOutBtn->setFixedSize(settingWidgetWidth, settingWidgetOptionHeight);

    settingLayout->addWidget(logOutBtn);

    // 1.2. Create body widget, layout
    bodyHeight = screenHeight - topHeight;
    bodyWidget = new QWidget();
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
    friendColumnWidth = screenWidth * 0.15;

    QWidget *friendDisplay = new QWidget();
    // friendDisplay->setFixedHeight(screenWidth - topHeight);
    friendDisplay->setStyleSheet("border: 1px solid blue;");
    friendDisplayLayout = new QVBoxLayout(friendDisplay);
    friendDisplayLayout->setAlignment(Qt::AlignTop);
    friendDisplayLayout->setContentsMargins(0, 0, 0, 0);
    friendDisplayLayout->setSpacing(0);

    friends = getFriendsOfUser(user.id);
    setUpFriendDisplay();

    scrollArea = new QScrollArea();
    scrollArea->setWidgetResizable(true);
    scrollArea->setFixedWidth(friendColumnWidth);
    scrollArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    scrollArea->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    scrollArea->setStyleSheet("border: 2px solid red;");
    scrollArea->setWidget(friendDisplay);
    bodyLayout->addWidget(scrollArea);

    friendRequests = getFriendRequestsOfUser(user.id);
    setUpFriendRequestsWidget();
    setUpFriendRequestsWidgetState();
    homeWindow->setLayout(homeLayout);

    setUpHomeEvents();
    cout << "Entered home screen" << endl;
};