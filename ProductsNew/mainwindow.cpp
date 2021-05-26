#include "mainwindow.h"
#include "treemodel.h"

#include <QFile>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{
    userId = 0;

    connect(&login, SIGNAL(login_button_clicked()), this, SLOT(authorizeUser()));
    connect(&login,SIGNAL(destroyed()), this, SLOT(show()));

    setupUi(this);

    if(!connectDB())
    {
        qDebug() << "Failed to connect DB";
        return;
    }

    QSqlQuery a_query;

    if (!a_query.exec("SELECT * FROM categories")) {
        qDebug() << "Can't fetch data from categories";
        return;
    }

    QSqlRecord rec = a_query.record();
    QString name = "";
    int parent_id = 0;
    int id = 0;

    QMultiMap<int, QMap<int, QString>> parentIdChildMap;

    while (a_query.next())
    {
        QMap<int, QString> idNameMap;

        name = a_query.value(rec.indexOf("name")).toString();
        parent_id = a_query.value(rec.indexOf("parent_id")).toInt();
        id = a_query.value(rec.indexOf("id")).toInt();
        idNameMap.insert(id, name);
        parentIdChildMap.insert(parent_id, idNameMap);
    }

    QMultiMap <int, QMap<int, QString>>::Iterator it = parentIdChildMap.find(0);
    QStandardItemModel *model = new QStandardItemModel();
    int j = 0;
    while (it != parentIdChildMap.end() && it.key() == 0)
    {
        model->appendRow(buildCategoriesTreeRecursion(it, parentIdChildMap));

        ++it;
        j++;
    }

    model->setHorizontalHeaderItem( 0, new QStandardItem( "Name" ) );

    connect(exitAction, &QAction::triggered, qApp, &QCoreApplication::quit);
    connect(actionCreate_check, &QAction::triggered, this, &MainWindow::openCheckForm);
    connect(actionClose_session, &QAction::triggered, this, &MainWindow::closeSession);
    connect(view, &QAbstractItemView::doubleClicked, this, &MainWindow::on_view_doubleClicked);

    view->setModel(model);
}

QStandardItem* MainWindow::buildCategoriesTreeRecursion(QMultiMap <int, QMap<int, QString>>::Iterator it, QMultiMap<int, QMap<int, QString>> parentIdChildMap)
{
    QMap<int, QString> value = it.value();

    QStandardItem *item = new QStandardItem(value.begin().value());
    item->setData(QVariant(value.begin().key()), 5);
    item->setEditable( false );

    QMultiMap <int, QMap<int, QString>>::Iterator itNextLevel = parentIdChildMap.find(value.begin().key());
    while (itNextLevel != parentIdChildMap.end() && itNextLevel.key() == value.begin().key())
    {
        item->appendRow( buildCategoriesTreeRecursion(itNextLevel, parentIdChildMap) );
        ++itNextLevel;
    }

    return item;
}

void MainWindow::on_view_doubleClicked(const QModelIndex &index)
{
    QVariant selected_id = index.data(5).toInt();
    QSqlQuery a_query;

    if (!a_query.exec("SELECT p.id, p.name, p.code, p.price FROM products_categories as pc JOIN products as p ON pc.product_id = p.id where pc.category_id = " + QString::number(index.data(5).toInt()))) {
        qDebug() << "Can't select from table products_categories";
        return;
    }
    QSqlRecord rec = a_query.record();
    QString name = "";
    QString code = 0;
    QString id = 0;
    QString price = 0;
    QStandardItemModel *modelTable = new QStandardItemModel();
    uint16_t count = 0;

    modelTable->setHorizontalHeaderItem( 0, new QStandardItem( "ID" ) );
    modelTable->setHorizontalHeaderItem( 1, new QStandardItem( "Name" ) );
    modelTable->setHorizontalHeaderItem( 2, new QStandardItem( "Code" ) );
    modelTable->setHorizontalHeaderItem( 3, new QStandardItem( "Price" ) );

    while (a_query.next())
    {
       name = a_query.value(rec.indexOf("name")).toString();
       code = a_query.value(rec.indexOf("code")).toString();
       id = a_query.value(rec.indexOf("id")).toString();
       price = a_query.value(rec.indexOf("price")).toString();

       modelTable->setItem(count, 0, new QStandardItem(id));
       modelTable->setItem(count, 1, new QStandardItem(name));
       modelTable->setItem(count, 2, new QStandardItem(code));
       modelTable->setItem(count, 3, new QStandardItem(price));

       count++;
    }

    tableView->setModel(modelTable);
}

bool MainWindow::connectDB()
{
    dbase = QSqlDatabase::addDatabase("QSQLITE");
    dbase.setDatabaseName("products.sqlite");
    if (!dbase.open()) {
        qDebug() << "No db connection!";
        return false;
    }
    return true;
}

void MainWindow::authorizeUser()
{
    QString userNameInput = login.getLogin();
    QString userpassInput = login.getPass();

    if(userNameInput.isEmpty())
    {
        errorMessage("Empty field Username");
        return;
    }

    if(userpassInput.isEmpty())
    {
        errorMessage("Empty field Password");
        return;
    }

    QString str_t = " SELECT * "
                    " FROM users "
                    " WHERE name = '%1'";

    QString username = "";
    QString userpass = "";

    QSqlQuery query;
    QSqlRecord rec;

    if(!query.exec(str_t.arg(userNameInput)))
    {
        qDebug() << "Unable to execute query - exiting" << query.lastError() << " : " << query.lastQuery();
        return;
    }

    rec = query.record();
    query.next();
    userId = query.value(rec.indexOf("id")).toInt();
    username = query.value(rec.indexOf("name")).toString();
    userpass = query.value(rec.indexOf("password")).toString();

    QString pass = QString(encryption(userpassInput));

    if(userNameInput == username && pass == userpass)
    {
        if(sessionOpening())
        {
            login.close();
            this->show();
        }
        else
        {
            return;
        }
    }
    else
    {
        QString message = (userNameInput != username)? "Username missmatch": "Password missmatch";
        errorMessage(message);
    }
}

void MainWindow::loginDisplay()
{
    login.show();
}

void MainWindow::errorMessage(const QString message)
{
    QMessageBox msg(this);
    msg.setWindowModality(Qt::WindowModal);
    msg.setWindowTitle(QLatin1String("Error"));
    msg.setTextFormat(Qt::RichText);
    msg.setText(message);
    msg.setStandardButtons(QMessageBox::Ok);
    msg.setIcon(QMessageBox::Icon::Warning);
    msg.exec();
}

QByteArray MainWindow::encryption(QString s)
{
    QByteArray ba = s.toUtf8();
    return QCryptographicHash::hash(ba, QCryptographicHash::Sha256).toHex();
}

bool MainWindow::sessionOpening(void)
{
    QMessageBox::StandardButton reply = QMessageBox::question(this, "Question", "Do you want to start session?", QMessageBox::Yes|QMessageBox::No);
    if (reply == QMessageBox::Yes)
    {
        QSqlQuery a_query;
        QString str_insert = "INSERT INTO sessions(user_id) "
                "VALUES (%1);";
        QString str = str_insert.arg(QString::number(userId));
        bool b = a_query.exec(str);

        if (!b) {
            qDebug() << "Can't create session";
            QApplication::quit();
            return false;
        }
        sessionId =  a_query.lastInsertId().toInt();
        return true;
    }
    else
    {
        QApplication::quit();
        return false;
    }
}

void MainWindow::openCheckForm(void)
{
    checkForm.setParams(sessionId);
    checkForm.show();
}

void MainWindow::closeSession(void)
{
    QSqlQuery a_query;
    QString str_insert = "update sessions set closed_at = CURRENT_TIMESTAMP where id = " + QString::number(sessionId);
    bool b = a_query.exec(str_insert);

    if (!b) {
        qDebug() << "Can't closed session";
    }

    QString totalCheck = "select count(*) checks_count from checks where session_id = "  + QString::number(sessionId);
    QString totalSum = "select sum(quantity*price)  from checks_items where check_id in (select id from checks where session_id = " + QString::number(sessionId) + ")";
    QString quantityFromMarkdown = "select sum(quantity) from checks_items ci \
    join  products_categories pc  on pc.product_id = ci.product_id\
    join categories c on category_id\
    where c.name = 'markdown'\
    and ci.check_id in (select id from checks where session_id = " + QString::number(sessionId) + ")";


    if (!a_query.exec(totalSum)) {
        qDebug() << "Can't closed session";
    }
    a_query.next();
    QString sum = "Sale Total Sum = " + a_query.value(0).toString() + ";";

    if (!a_query.exec(quantityFromMarkdown)) {
        qDebug() << "Can't close session";
    }
    a_query.next();
    QString Markdown = "Saled Products Quantity From Markdown = " + a_query.value(0).toString() + ";";

    if (!a_query.exec(totalCheck)) {
        qDebug() << "Can't close session";
    }
    a_query.next();
    QString total_Check = "Total Check Count = " + a_query.value(0).toString() + ";";

    this->close();

    QMessageBox msgBox;
    msgBox.setWindowTitle("Statistics");
    msgBox.setText(sum + '\n' + Markdown + '\n' + total_Check);
    msgBox.exec();

}
