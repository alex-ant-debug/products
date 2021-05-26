#include "loginwindow.h"
#include "ui_loginWindow.h"

loginWindow::loginWindow(QWidget *parent):
    QDialog(parent),
    ui(new Ui::loginWindow)
{
    ui->setupUi(this);
}

loginWindow::~loginWindow()
{
    delete ui;
}

QString loginWindow::getLogin()
{
    return loginWindow::m_username;
}

QString loginWindow::getPass()
{
    return loginWindow::m_userpass;
}

void loginWindow::on_LogInButton_clicked()
{
    emit login_button_clicked();
}


void loginWindow::on_usernameLineEdit_textEdited(const QString &arg1)
{
    loginWindow::m_username = arg1;
}


void loginWindow::on_passwordLineEdit_textEdited(const QString &arg1)
{
     loginWindow::m_userpass = arg1;
}

