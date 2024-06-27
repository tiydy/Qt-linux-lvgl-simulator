
#include <QApplication>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlError>
#include <QDebug>
#include <vtslcd.h>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE");
    db.setDatabaseName("device.db");
    if(!db.open())
    {
        qDebug()<<db.lastError().text();
        return -1;
    }
    //创建表格
    QSqlQuery query;
    if(!query.exec("create table if not exists device(id integer NOT NULL PRIMARY KEY AUTOINCREMENT,"
                   "devname varchar(128) unique, "
                   "resx int, resy int, perlbits int, type int default 0 )"))
    {
        qDebug()<<query.lastError().text();
        return -1;
    }

    VTSLcd w;
    w.show();

    return a.exec();
}
