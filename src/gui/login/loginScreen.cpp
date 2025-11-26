#include "loginScreen.h"
#include "../homeScreen/homeScreen.h"
#include <QApplication>
#include <QWidget>
#include <QScreen>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QFrame>
#include <QGraphicsDropShadowEffect>
#include <QStackedWidget>
#include <iostream>
#include <string>
using namespace std;

QWidget *loginWindow;
QWidget *loginPanel;
QStackedWidget *currentForm;
QWidget *signInPage;
QWidget *signUpPage;
QPushButton *signInButton;
QPushButton *signUpButton;
QVBoxLayout *signUpLayout;
QVBoxLayout *signInLayout;
QLineEdit *signInUsernameTextbox;
QLineEdit *signUpUsernameTextbox;
QLineEdit *signInPasswordEdit;
QLineEdit *signUpPasswordEdit;
QLineEdit *signUpConfirmPasswordEdit;
QPushButton *submitButton;

bool signInMode = true;

void clearAllInputs(QWidget *parent)
{
    for (QLineEdit *edit : parent->findChildren<QLineEdit *>())
    {
        edit->clear();
    }
}

void setUp(bool newModeIsSignIn)
{
    QString inactiveStyle = "background-color: #d1cbcb; font-weight: lighter;";
    if (newModeIsSignIn)
    {
        signInButton->setStyleSheet("");
        signUpButton->setStyleSheet(inactiveStyle);
        if (!signInMode)
        {
            clearAllInputs(signUpPage);
            currentForm->setCurrentWidget(signInPage);
        }
        submitButton->setText("Sign In");
    }
    else
    {
        signUpButton->setStyleSheet("");
        signInButton->setStyleSheet(inactiveStyle);
        if (signInMode)
        {
            clearAllInputs(signInPage);
            currentForm->setCurrentWidget(signUpPage);
        }
        submitButton->setText("Sign Up");
    }
    signInMode = newModeIsSignIn;
}
void setEvents()
{
    QObject::connect(signInButton, &QPushButton::clicked, []()
                     { setUp(true); });

    QObject::connect(signUpButton, &QPushButton::clicked, []()
                     { setUp(false); });
    QObject::connect(submitButton, &QPushButton::clicked, []()
                     {
        if (signInMode)
        {
            cout << "sign in username: " << signInUsernameTextbox->text().toStdString() << ", password: " << signInPasswordEdit->text().toStdString() << endl;
            QString username = signInUsernameTextbox->text();
            QString password = signInPasswordEdit->text();

            // TODO: query db, verify account
            int loginResult = (username == "user" && password == "pass") ? 1 : 0;
            if (loginResult == 1){
                cout << "Login successful!" << endl;
                delete loginPanel; // Close login content loginPanel
                homeScreen(loginWindow);
                return;
            } else {
                cout << "Login failed: Invalid username or password." << endl;
            }
        }
        else
        {
            cout << "sign up username: " << signUpUsernameTextbox->text().toStdString() << ", password: " << signUpPasswordEdit->text().toStdString() << ", confirm password: " << signUpConfirmPasswordEdit->text().toStdString() << endl;
        } });
}
void loginScreen(QWidget *window)
{
    loginWindow = window;
    window->setWindowTitle("Login Screen");

    // Screen size
    QScreen *screen = QApplication::primaryScreen();
    QRect screenGeometry = screen->geometry();
    int screenWidth = screenGeometry.width();
    int screenHeight = screenGeometry.height();

    // loginPanel
    int loginPanelWidth = screenWidth * 0.7;
    int loginPanelHeight = screenHeight * 0.7;

    loginPanel = new QWidget(window);
    loginPanel->setFixedSize(loginPanelWidth, loginPanelHeight);
    loginPanel->move((screenWidth - loginPanelWidth) / 2, (screenHeight - loginPanelHeight) / 2);
    loginPanel->setStyleSheet("QWidget#loginPanel {background-color: #F5F5F5; border-radius: 0px;}");
    loginPanel->setObjectName("loginPanel");

    // Main layout for loginPanel
    QVBoxLayout *loginPanelLayout = new QVBoxLayout(loginPanel);
    loginPanelLayout->setContentsMargins(0, 0, 0, 0);

    // Top mode layout
    QWidget *modeWidget = new QWidget();
    modeWidget->setStyleSheet("background-color: #F5F5F5; border-radius: 0px;");
    QHBoxLayout *modeLayout = new QHBoxLayout();
    int modeButtonHeight = loginPanelHeight * 0.1;
    signInButton = new QPushButton("Sign In");
    signInButton->setFixedHeight(modeButtonHeight);

    signUpButton = new QPushButton("Sign Up");
    signUpButton->setFixedHeight(modeButtonHeight);

    modeLayout->addWidget(signInButton);
    modeLayout->addWidget(signUpButton);
    modeWidget->setLayout(modeLayout);

    // Form layout
    int labelHeight = loginPanelHeight * 0.8 * 0.1;
    // Sign in layout
    signInLayout = new QVBoxLayout();
    signInLayout->setAlignment(Qt::AlignTop);

    QLabel *signInUsernameLabel = new QLabel("Username:");
    signInUsernameLabel->setFixedHeight(labelHeight);
    signInUsernameTextbox = new QLineEdit();

    QLabel *signInPasswordLabel = new QLabel("Password:");
    signInPasswordLabel->setFixedHeight(labelHeight);
    signInPasswordEdit = new QLineEdit();
    signInPasswordEdit->setEchoMode(QLineEdit::Password);

    signInLayout->addWidget(signInUsernameLabel);
    signInLayout->addWidget(signInUsernameTextbox);
    signInLayout->addWidget(signInPasswordLabel);
    signInLayout->addWidget(signInPasswordEdit);

    signInPage = new QWidget();
    signInPage->setLayout(signInLayout);
    // Sign up layout
    signUpLayout = new QVBoxLayout();
    signUpLayout->setAlignment(Qt::AlignTop);

    QLabel *signUpUsernameLabel = new QLabel("Username:");
    signUpUsernameLabel->setFixedHeight(labelHeight);
    signUpUsernameTextbox = new QLineEdit();

    QLabel *signUpPasswordLabel = new QLabel("Password:");
    signUpPasswordLabel->setFixedHeight(labelHeight);
    signUpPasswordEdit = new QLineEdit();
    signUpPasswordEdit->setEchoMode(QLineEdit::Password);

    QLabel *signUpConfirmPasswordLabel = new QLabel("Confirm Password:");
    signUpConfirmPasswordLabel->setFixedHeight(labelHeight);
    signUpConfirmPasswordEdit = new QLineEdit();
    signUpConfirmPasswordEdit->setEchoMode(QLineEdit::Password);

    signUpLayout->addWidget(signUpUsernameLabel);
    signUpLayout->addWidget(signUpUsernameTextbox);
    signUpLayout->addWidget(signUpPasswordLabel);
    signUpLayout->addWidget(signUpPasswordEdit);
    signUpLayout->addWidget(signUpConfirmPasswordLabel);
    signUpLayout->addWidget(signUpConfirmPasswordEdit);

    signUpPage = new QWidget();
    signUpPage->setLayout(signUpLayout);

    // Set up current form
    int formHeight = loginPanelHeight * 0.8;
    currentForm = new QStackedWidget();
    currentForm->setFixedHeight(formHeight);
    currentForm->addWidget(signInPage);
    currentForm->addWidget(signUpPage);
    currentForm->setContentsMargins(0, 0, 0, 0);

    // Sign in/up button
    int submitButtonHeight = loginPanelHeight * 0.05;
    int submitButtonWidth = loginPanelWidth * 0.3;
    submitButton = new QPushButton("lorem ipsum");
    submitButton->setFixedHeight(submitButtonHeight);
    submitButton->setFixedWidth(submitButtonWidth);
    submitButton->setStyleSheet("border-radius: 10px; border: 1px solid rgba(0, 0, 0, 0.20); border-style: inset; background-color: #ffffff; font-size: 16px;");

    QGraphicsDropShadowEffect *shadowEffect = new QGraphicsDropShadowEffect;
    shadowEffect->setColor(QColor(0, 0, 0, 60));
    shadowEffect->setOffset(5, 5);
    shadowEffect->setBlurRadius(7);

    submitButton->setGraphicsEffect(shadowEffect);
    // Add to loginPanel layout
    loginPanelLayout->addWidget(modeWidget);
    loginPanelLayout->addWidget(currentForm);
    loginPanelLayout->addWidget(submitButton, 0, Qt::AlignHCenter);
    loginPanelLayout->addStretch(); // push everything up

    setUp(true);
    setEvents();

    return;
}
