#include <QApplication>
#include <QWidget>
#include <QLineEdit>
#include <QPushButton>
#include <QLabel>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <iostream>

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    QWidget window;
    window.setWindowTitle("Login");
    window.resize(300, 150);

    // Widgets
    QLabel *userLabel = new QLabel("Username:");
    QLabel *passLabel = new QLabel("Password:");

    QLineEdit *usernameInput = new QLineEdit();
    QLineEdit *passwordInput = new QLineEdit();
    passwordInput->setEchoMode(QLineEdit::Password);

    QPushButton *loginButton = new QPushButton("Login");

    // Layout
    QVBoxLayout *mainLayout = new QVBoxLayout(&window);

    QHBoxLayout *userLayout = new QHBoxLayout();
    userLayout->addWidget(userLabel);
    userLayout->addWidget(usernameInput);

    QHBoxLayout *passLayout = new QHBoxLayout();
    passLayout->addWidget(passLabel);
    passLayout->addWidget(passwordInput);

    mainLayout->addLayout(userLayout);
    mainLayout->addLayout(passLayout);
    mainLayout->addWidget(loginButton);

    // Connect login button to a lambda (no Q_OBJECT needed)
    QObject::connect(loginButton, &QPushButton::clicked, [&]()
                     {
        std::cout << "Username: " << usernameInput->text().toStdString() << "\n";
        std::cout << "Password: " << passwordInput->text().toStdString() << "\n"; });

    window.show();

    return app.exec();
}
