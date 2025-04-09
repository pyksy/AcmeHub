#include <QCoreApplication>
#include <QtHttpServer/QHttpServer>
#include <QTcpServer>
#include <QJsonDocument>
#include <QJsonObject>

#include "acmedatabase.h"

#define LISTEN_ADDRESS "0.0.0.0"
#define LISTEN_PORT 4080
#define DEFAULT_HTTP_RESPONSE "The AcmeHub batch data collector is up and running. Please consult the AcmeHub README.md for API usage. Have a nice day! :)\n"

int main(int argc, char *argv[]) {
    QCoreApplication a(argc, argv);

    auto acmeDatabase = AcmeDatabase();

    QHttpServer httpServer;
    httpServer.route("/",
                     QHttpServerRequest::Method::Get,
                     [] {
        return QHttpServerResponse("text/plain",
                                   DEFAULT_HTTP_RESPONSE,
                                   QHttpServerResponder::StatusCode::Ok);
    });

    httpServer.route("/process_report",
                     QHttpServerRequest::Method::Post,
                     [&acmeDatabase](const QHttpServerRequest &request) {
        QJsonParseError error;
        const QJsonDocument jsonDocument = QJsonDocument::fromJson(request.body(), &error);
        const QJsonObject jsonObject = jsonDocument.object();

        qDebug() << Q_FUNC_INFO << "serverName:" << jsonObject.value("server_name").toString();
        qDebug() << Q_FUNC_INFO << "startTime: " << jsonObject.value("start_time").toString();
        qDebug() << Q_FUNC_INFO << "endTime:   " << jsonObject.value("end_time").toString();

        AcmeBatchData data(jsonObject.value("server_name").toString(),
                           jsonObject.value("start_time").toString(),
                           jsonObject.value("end_time").toString());

        acmeDatabase.AppendAcmeBatchData(data);

        return QHttpServerResponse("application/json",
                                   "",
                                   QHttpServerResponder::StatusCode::Ok);
    });

    httpServer.route("/process_statistics",
                     QHttpServerRequest::Method::Get,
                     []() {
        return QHttpServerResponse("application/json",
                                   "",
                                   QHttpServerResponder::StatusCode::InternalServerError);
    });

    auto tcpServer = std::make_unique<QTcpServer>();

    if (!tcpServer->listen(QHostAddress(LISTEN_ADDRESS), LISTEN_PORT)) {
        qInfo() << "Error: Cannot bind TCP address" << LISTEN_ADDRESS << "port" << LISTEN_PORT;
        return 1;
    }

    // bind() in QtHttpServer API changes from void to int in version 6.8.0
#if QT_VERSION >= QT_VERSION_CHECK(6, 8, 0)
    if (!httpServer.bind(tcpServer.get())) {
        qDebug() << Q_FUNC_INFO << "Error: Cannot bind http server to TCP port";
        return 1;
    }
#else
    httpServer.bind(tcpServer.get());
#endif

    tcpServer.release();

    qInfo() << "Listening for incoming http connections on host" << LISTEN_ADDRESS << "port" << LISTEN_PORT;
    return a.exec();
}
