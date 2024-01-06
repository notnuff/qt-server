#include <QCoreApplication>
#include <QCommandLineParser>
#include "server.h"

int main(int argc, char *argv[])
{
    QCoreApplication app(argc, argv);
    QCommandLineParser parser;

    parser.addOptions({
                       { "port", QCoreApplication::translate("main", "The port the server listens on."),
                        "port" },
                       });
    parser.addHelpOption();
    parser.process(app);

    QString host = "127.0.0.1";
    quint16 portArg = 1337;
    if (!parser.value("port").isEmpty())
        portArg = parser.value("port").toUShort();

    BasicHttpServer server{host, portArg};
    server.setupRoutes();

    return app.exec();
}
