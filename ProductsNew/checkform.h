#ifndef CHECKFORM_H
#define CHECKFORM_H

#include <QWidget>
#include <QtSql>
#include <QTableView>
#include <QStandardItemModel>
#include <QNetworkAccessManager>
#include <QtNetwork/QNetworkReply>

namespace Ui {
class CheckForm;
}

class CheckForm : public QWidget
{
    Q_OBJECT

public:
    explicit CheckForm(QWidget *parent = nullptr);
    ~CheckForm();
    void setParams(int sessionId);
    bool internetConnection(void);
    QSqlDatabase dbase;

private slots:
    void on_lineEdit_editingFinished();
    void on_savePushButton_clicked();

private:
    Ui::CheckForm *ui = nullptr;
    QStandardItemModel *modelTable = nullptr;
    int sessionId = 0;

};

#endif // CHECKFORM_H
