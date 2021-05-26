#ifndef LOGINWINDOW_H
#define LOGINWINDOW_H

#include <QDialog>


namespace Ui {
class loginWindow;
}

class loginWindow : public QDialog
{
    Q_OBJECT

public:
    explicit loginWindow(QWidget *parent = nullptr);
    ~loginWindow();
    QString getLogin();
    QString getPass();

signals:
    void login_button_clicked();

private slots:

    void on_LogInButton_clicked();
    void on_usernameLineEdit_textEdited(const QString &arg1);
    void on_passwordLineEdit_textEdited(const QString &arg1);

private:
    Ui::loginWindow *ui;
    QString m_username;
    QString m_userpass;
};

#endif // LOGINWINDOW_H
