#ifndef SERVER_H
#define SERVER_H

#include <QHttpServer>
#include <QHttpServerResponse>
#include <QCoreApplication>

#include "connecttodb.h"
#include "dataCrudApi.h"

class BasicHttpServer{
private:
    QHttpServer httpServer;
    QSqlDatabase *db;
    QSqlQuery *query;
    DataCrudApi *api;
    int port;
public:
    BasicHttpServer(QString host = "127.0.0.1", int port = 1337){
        ConnectionToDB connect;
        if (connect.conectToDb()) db = connect.getDB();
        query = new QSqlQuery(*db);
        api = new DataCrudApi(db);
        qInfo().noquote()
            << QCoreApplication::translate("QHttpServerExample",
                                           "Running on %1:%2/"
                                           "(Press CTRL+C to quit)").arg(host).arg(port);
        port = httpServer.listen(QHostAddress(host), port);

        if (!port) {
            qWarning() << QCoreApplication::translate("QHttpServerExample",
                                                      "Server failed to listen on a port.");
        }
    }

    void setupRoutes(){
        DataCrudApi thisApi = *api;
        httpServer.route(
            QString("/data"), QHttpServerRequest::Method::Get,
            [this](const QHttpServerRequest &request) {
                return api->getAllItems();
            });

        httpServer.route(
            QString("/data/<arg>"), QHttpServerRequest::Method::Get,
            [this](qint64 itemId) {
                return api->getItem(itemId);
            });

        httpServer.route(
            QString("/data"), QHttpServerRequest::Method::Post,
            [this](const QHttpServerRequest &request) {
                return api->createItem(request);
            });

        httpServer.route(
            QString("/data/<arg>"), QHttpServerRequest::Method::Patch,
            [this](qint64 itemId, const QHttpServerRequest &request) {
                return api->updateItem(itemId, request);
            });

        httpServer.route(
            QString("/data"), QHttpServerRequest::Method::Delete,
            [this]() {
                return api->deleteItem();
            });

        httpServer.route(
            QString("/data/<arg>"), QHttpServerRequest::Method::Delete,
            [this](qint64 itemId) {
                return api->deleteItem(itemId);
            });

        httpServer.afterRequest(
            [](QHttpServerResponse &&resp) {
                resp.setHeader("Server", "Qt REST full api");
                return std::move(resp);
            });
    }
};

#endif // SERVER_H
