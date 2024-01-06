#ifndef DATACRUDAPI_H
#define DATACRUDAPI_H

#include <QtGlobal>
#include <QtHttpServer/QHttpServer>
#include <QtConcurrent/qtconcurrentrun.h>
#include <QSqlDatabase>
#include <optional>
#include <QSqlQuery>
#include <QJsonObject>
#include <QSqlError>
#include <QSqlRecord>
#include <QDateTime>
#include <QJsonDocument>


class DataCrudApi
{
public:
    DataCrudApi(QSqlDatabase *db) : db(db)
    {
        query = new QSqlQuery(*db);
    }

    QHttpServerResponse getItem(qint64 itemId) const
    {
        query->prepare("SELECT * FROM mydb.Data WHERE id = :id");
        query->bindValue(":id", itemId);
        if (query->exec() && query->next()) {
            QJsonObject res;
            QJsonObject row{
                {QString("id"), query->value(0).toJsonValue()},
                {QString("size"), query->value(1).toJsonValue()},
                {QString("format"), query->value(2).toJsonValue()},
                {QString("name"), query->value(3).toJsonValue()},
                {QString("updatedAt"), query->value(4).toDateTime().toString("yyyy-MM-dd hh:mm:ss")}
            };
            res.insert(QString("%1").arg(query->value(0).toInt()), row);
            return QHttpServerResponse(res.empty() ?
                                           QVariant("No data").toJsonObject()
                                                   : res
                                       , QHttpServerResponder::StatusCode::Ok);
        }

        qDebug() << query->lastError() << query->executedQuery();
        qDebug() << db->lastError();
        return QHttpServerResponse("Цей користувач не знайдений", QHttpServerResponder::StatusCode::NotFound);

    }
    QHttpServerResponse getAllItems() const
    {
        query->prepare("SELECT * FROM mydb.Data");

        if (query->exec()) {
            if (!query->size()) return QHttpServerResponse("No data", QHttpServerResponder::StatusCode::Ok);
            QJsonObject res;
            while (query->next()){
                QJsonObject row{
                    {QString("id"), query->value(0).toJsonValue()},
                    {QString("size"), query->value(1).toJsonValue()},
                    {QString("format"), query->value(2).toJsonValue()},
                    {QString("name"), query->value(3).toJsonValue()},
                    {QString("updatedAt"), query->value(4).toString()}
                };
                res.insert(QString("%1").arg(query->value(0).toInt()), row);
            }
            return QHttpServerResponse(res, QHttpServerResponder::StatusCode::Ok);
        }

        qDebug() << query->lastError() << query->executedQuery();
        qDebug() << db->lastError();
        return QHttpServerResponse("", QHttpServerResponder::StatusCode::NotFound);
    }

    QHttpServerResponse createItem(const QHttpServerRequest &request)
    {
        query->prepare("INSERT INTO mydb.Data (size, format, name, uploadedAt) VALUES "
                            "(:size, :format, :name, :uploadedAt)"
                       );

        QJsonDocument parseDoc= QJsonDocument::fromJson(QByteArray(request.body()));
        QJsonObject parseObject = parseDoc.object();

        query->bindValue(":size", parseObject["size"].toVariant());
        query->bindValue(":format", parseObject["format"].toVariant());
        query->bindValue(":name", parseObject["name"].toVariant());
        query->bindValue(":uploadedAt", QDateTime::currentDateTime());

        if (query->exec()){
            query->exec("SELECT * FROM `mydb`.`Data` WHERE id=(SELECT max(id) FROM `mydb`.`Data`);");
            query->next();
            QJsonObject res{
                {QString("id"), query->value(0).toJsonValue()},
                {QString("size"), query->value(1).toJsonValue()},
                {QString("format"), query->value(2).toJsonValue()},
                {QString("name"), query->value(3).toJsonValue()},
                {QString("updatedAt"), query->value(4).toDateTime().toString("yyyy-MM-dd hh:mm:ss")}
            };
            return QHttpServerResponse(res, QHttpServerResponder::StatusCode::Created);
        };

        qDebug() << query->lastError() << query->executedQuery();
        qDebug() << db->lastError();
        return QHttpServerResponse("Неправильні дані для створення користувача", QHttpServerResponder::StatusCode::BadRequest);
    }


    QHttpServerResponse updateItem(qint64 itemId, const QHttpServerRequest &request)
    {
        query->prepare("SELECT * FROM mydb.Data WHERE id = :id");
        query->bindValue(":id", itemId);
        if (!query->exec() && !query->next())
            return QHttpServerResponse(QHttpServerResponder::StatusCode::BadRequest);



        QJsonDocument parseDoc= QJsonDocument::fromJson(QByteArray(request.body()));
        QJsonObject parseObject = parseDoc.object();
        QString prepareSet;

        for (QJsonObject::iterator it = parseObject.begin(); it != parseObject.end(); ++it) {
            QString key = it.key();
            QJsonValue value = it.value();
            prepareSet.append(QString("`%1` = '%2', ").arg(key, value.toString()));
        }

        query->prepare("UPDATE mydb.Data "
                       "SET " +
                       prepareSet +
                       "uploadedAt = '" +
                       QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss") +
                       "' WHERE (`id` = '" +
                       QString::number(itemId) +
                       "');"
                       );

        if (query->exec()){
            query->prepare("SELECT * FROM mydb.Data WHERE id = :id");
            query->bindValue(":id", itemId);
            query->exec();
            query->next();
            QJsonObject res{
                {QString("id"), query->value(0).toJsonValue()},
                {QString("size"), query->value(1).toJsonValue()},
                {QString("format"), query->value(2).toJsonValue()},
                {QString("name"), query->value(3).toJsonValue()},
                {QString("updatedAt"), query->value(4).toDateTime().toString("yyyy-MM-dd hh:mm:ss")}
            };
            return QHttpServerResponse(res, QHttpServerResponder::StatusCode::Ok);
        };

        qDebug() << query->lastError() << query->executedQuery();
        qDebug() << db->lastError();
        return QHttpServerResponse(QHttpServerResponder::StatusCode::BadRequest);
    }

    QHttpServerResponse deleteItem(qint64 itemId = -1)
    {
        qDebug() << itemId;
        QString deletePrepare= "";
        if (itemId != -1) deletePrepare.append(QString("WHERE id = %1").arg(itemId));
        query->prepare("DELETE FROM mydb.Data " +
                       deletePrepare);
        if (query->exec())
            return QHttpServerResponse(QHttpServerResponder::StatusCode::Ok);

        qDebug() << query->lastError() << query->executedQuery();
        qDebug() << db->lastError();
        return QHttpServerResponse(QHttpServerResponder::StatusCode::BadRequest);
    }

private:
    QSqlDatabase *db;
    QSqlQuery *query;
};


#endif // DATACRUDAPI_H
