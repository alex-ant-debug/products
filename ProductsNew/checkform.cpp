#include "checkform.h"
#include "ui_checkform.h"

CheckForm::CheckForm(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::CheckForm)
{
    ui->setupUi(this);
    modelTable = new QStandardItemModel();

    modelTable->setHorizontalHeaderItem( 0, new QStandardItem( "Name" ) );
    modelTable->setHorizontalHeaderItem( 1, new QStandardItem( "Price" ) );
    modelTable->setHorizontalHeaderItem( 2, new QStandardItem( "Quantity" ) );
    modelTable->setHorizontalHeaderItem( 3, new QStandardItem( "Total" ) );

    connect(ui->findCodeLineEdit, &QLineEdit::editingFinished, this, &CheckForm::on_lineEdit_editingFinished);

    ui->productTableView->setModel(modelTable);
}

CheckForm::~CheckForm()
{
    delete ui;
}

void CheckForm::on_lineEdit_editingFinished()
{
    QString findCode = ui->findCodeLineEdit->text();
    ui->findCodeLineEdit->clear();

    if(findCode.isEmpty())
    {
        return;
    }

    dbase = QSqlDatabase::addDatabase("QSQLITE");
    dbase.setDatabaseName("products.sqlite");
    if (!dbase.open())
    {
        qDebug() << "No db connection!";
        return;
    }

    QString str_t = " SELECT * "
                    " FROM products "
                    " WHERE code = '%1'";

    QSqlQuery query;
    QSqlRecord rec;

    if(!query.exec(str_t.arg(findCode)))
    {
        qDebug() << "Unable to execute query - exiting" << query.lastError() << " : " << query.lastQuery();
        return;
    }

    rec = query.record();
    query.next();

    int id = query.value(rec.indexOf("id")).toInt();
    QString name = query.value(rec.indexOf("name")).toString();
    float price = query.value(rec.indexOf("price")).toFloat();

    if(!id)
    {
        return;
    }

    bool isAlready = false;

    for (int i = 0; i < modelTable->rowCount(); ++i)
    {
        if(id == modelTable->item(i, 0)->data(5).toInt())
        {
            int temp = modelTable->item(i, 2)->data(0).toInt() + 1;
            modelTable->setItem(i, 2, new QStandardItem(QString::number(temp)));
            modelTable->setItem(i, 3, new QStandardItem(QString::number(price*temp)));
            isAlready = true;
            break;
        }
    }

    int count = modelTable->rowCount();

    if(!isAlready)
    {
        QStandardItem *productItem = new QStandardItem(name);
        productItem->setData(QVariant(id), 5);
        modelTable->setItem(count, 0, productItem);
        modelTable->setItem(count, 1, new QStandardItem(QString::number(price)));
        modelTable->setItem(count, 2, new QStandardItem(QString::number(1)));
        modelTable->setItem(count, 3, new QStandardItem(QString::number(price)));
    }
}

void CheckForm::setParams(int sessionId)
{
    this->sessionId = sessionId;
}

void CheckForm::on_savePushButton_clicked()
{
    QString status = (internetConnection())? "Sent": "Pending connection";
    QSqlQuery a_query;
    QString queryTemplate;
    QString query;

    queryTemplate = "INSERT INTO checks(session_id, status) "
                    "VALUES (%1, '%2');";

    query = queryTemplate.arg(QString::number(sessionId))
            .arg(status);

    if (!a_query.exec(query))
    {
        qDebug() << "Can't create check";
    }

    queryTemplate.clear();
    query.clear();

    int check_id =  a_query.lastInsertId().toInt();
    queryTemplate = "INSERT INTO checks_items(check_id, product_id, quantity, price) "
                    "VALUES (%1, %2, %3, %4);";

    for (int i = 0; i < modelTable->rowCount(); ++i)
    {
        query = queryTemplate.arg(QString::number(check_id))
             .arg(modelTable->item(i, 0)->data(5).toString())
             .arg(modelTable->item(i, 2)->data(0).toString())
             .arg(modelTable->item(i, 1)->data(0).toString());

        if (!a_query.exec(query))
        {
            qDebug() << "Can't create checks_items";
        }
    }

    this->close();
}

bool CheckForm::internetConnection(void)
{
//    QNetworkAccessManager nam;
//    QNetworkRequest req(QUrl("http://www.google.com"));[
//    QNetworkReply *reply = nam.get(req);
//    QEventLoop loop;
//    connect(reply, SIGNAL(finished()), &loop, SLOT(quit()));
//    loop.exec();
//    if(reply->bytesAvailable())
//    return true;//QMessageBox::information(this, “Info”, “Интернет есть :)”);
//    else
//    return false;//QMessageBox::critical(this, “Info”, “Интернета нету :(“);
    return true;
}
