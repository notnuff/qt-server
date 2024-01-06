#ifndef CONNECTTODB_H
#define CONNECTTODB_H

#include <QString>
#include <QSqlDatabase>
#include <QSqlError>
#include <QDebug>

class ConnectionToDB{

private:
    QString dbName;
    QString host;
    QString userName;
    QString password;
    QSqlDatabase *db = new QSqlDatabase();
    int port;
public:
    bool conectToDb(
            QString host = "localhost",
            QString dbName = "mydb",
            QString userName = "root",
            QString password = "6=Oo6s9<GDfqa5QTg£^T-yC,kbFs8",
            int port = 3306)
    {
        *db = QSqlDatabase::addDatabase("QMYSQL");
        db->setHostName(host);
        db->setDatabaseName(dbName);
        db->setUserName(userName);
        db->setPassword(password);
        db->setPort(port);
        bool ok = db->open();
        if (!ok){
            qDebug() << db->lastError();
            return false;
        }
        return true;
    }
    QSqlDatabase *getDB (){
        return db;
    }
};

#endif // CONNECTTODB_H
