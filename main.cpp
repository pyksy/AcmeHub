#include <QCoreApplication>
#include <QtHttpServer/QHttpServer>
#include <QTcpServer>
#include <QJsonDocument>
#include <QJsonObject>

#include "acmedatabase.h"

#define LISTEN_ADDRESS "0.0.0.0"
#define LISTEN_PORT 4080

int main(int argc, char *argv[]) {
    QCoreApplication a(argc, argv);

    auto acmeDatabase = AcmeDatabase();
    if (!acmeDatabase.InitAcmeDatabase()) {
        qInfo() << "Error: Unable to initialize database. No write permission to current dir?";
        return 1;
    }

    QHttpServer httpServer;
    httpServer.route("/",
                     QHttpServerRequest::Method::Get,
                     [] {
        qDebug() << Q_FUNC_INFO << "GET /";
        return QHttpServerResponse("text/plain",
                                   "Please consult the AcmeHub README.md for API usage.\n",
                                   QHttpServerResponder::StatusCode::Ok);
    });

    httpServer.route("/process_report",
                     QHttpServerRequest::Method::Post,
                     [&acmeDatabase](const QHttpServerRequest &request) {
        qDebug() << Q_FUNC_INFO << "POST /process_report";
        // TODO move json parse to db
        QJsonParseError error;
        const QJsonDocument jsonDocument = QJsonDocument::fromJson(request.body(), &error);
        const QJsonObject jsonObject = jsonDocument.object();

        AcmeBatchData data(jsonObject.value("server_name").toString(),
                           jsonObject.value("start_time").toString(),
                           jsonObject.value("end_time").toString());

        bool success = acmeDatabase.AppendAcmeBatchData(data);
        qDebug() << Q_FUNC_INFO << "append data success:" << success;

        return QHttpServerResponse("application/json",
                                   "",
                                   QHttpServerResponder::StatusCode::Ok);
    });

    httpServer.route("/process_statistics",
                     QHttpServerRequest::Method::Get,
                     [&acmeDatabase]() {
        qDebug() << Q_FUNC_INFO << "GET /process_statistics";

        auto statisticsJson = QJsonObject();

        if (acmeDatabase.GetProcessStatistics(statisticsJson)) {
            return QHttpServerResponse(statisticsJson,
                                       QHttpServerResponder::StatusCode::Ok);
        } else {
            return QHttpServerResponse(statisticsJson,
                                       QHttpServerResponder::StatusCode::InternalServerError);
        }
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
