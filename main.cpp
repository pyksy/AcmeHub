#include <QCoreApplication>
#include <QtHttpServer/QHttpServer>
#include <QTcpServer>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>

#include "acmehubdatabase.h"

#define LISTEN_ADDRESS "0.0.0.0"
#define LISTEN_PORT 4080

/*
 *  main()
 *
 **/
int main(int argc, char *argv[]) {
    QCoreApplication a(argc, argv);

    /* Create and init database helper class */
    auto acmeDatabase = AcmeHubDatabase();
    if (!acmeDatabase.InitAcmeHubDatabase()) {
        qInfo() << "Error: Unable to initialize database. No write permission to current dir?";
        return 1;
    }

    /* Create HTTP server and add routes (path handlers) */
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
        qInfo().noquote() << request.remoteAddress().toString() << "POST /process_report";
        QJsonParseError error;
        const QJsonDocument jsonDocument = QJsonDocument::fromJson(request.body(), &error);
        const QJsonObject jsonObject = jsonDocument.object();

        /* Result ignored for now */
        bool success = acmeDatabase.AppendAcmeBatchData(jsonObject);

        return QHttpServerResponse(QJsonObject(),
                                   QHttpServerResponder::StatusCode::Ok);
    });

    httpServer.route("/process_statistics",
                     QHttpServerRequest::Method::Get,
                     [&acmeDatabase](const QHttpServerRequest &request) {
        qInfo().noquote() << request.remoteAddress().toString() << "GET /process_statistics";
        auto statisticsJson = QJsonObject();

        if (acmeDatabase.GetProcessStatistics(statisticsJson)) {
            return QHttpServerResponse(statisticsJson,
                                       QHttpServerResponder::StatusCode::Ok);
        } else {
            return QHttpServerResponse(statisticsJson,
                                       QHttpServerResponder::StatusCode::InternalServerError);
        }
    });

    httpServer.route("/process_outliers",
                     QHttpServerRequest::Method::Get,
                     [&acmeDatabase](const QHttpServerRequest &request) {
        qInfo().noquote() << request.remoteAddress().toString() << "GET /process_outliers";
        auto outliersJson = QJsonArray();

        if (acmeDatabase.GetProcessOutliers(outliersJson)) {
            return QHttpServerResponse(outliersJson,
                                       QHttpServerResponder::StatusCode::Ok);
        } else {
            return QHttpServerResponse(outliersJson,
                                       QHttpServerResponder::StatusCode::InternalServerError);
        }
    });

    /* Create TCP listen socket */
    auto tcpServer = std::make_unique<QTcpServer>();
    if (!tcpServer->listen(QHostAddress(LISTEN_ADDRESS), LISTEN_PORT)) {
        qInfo() << "Error: Cannot bind TCP address" << LISTEN_ADDRESS << "port" << LISTEN_PORT;
        return 1;
    }

    /* Bind HTTP server to the TCP socket.
       QHttpServer::bind() API changed in version 6.8.0 */
#if QT_VERSION >= QT_VERSION_CHECK(6, 8, 0)
    if (!httpServer.bind(tcpServer.get())) {
        qDebug() << Q_FUNC_INFO << "Error: Cannot bind http server to TCP port";
        return 1;
    }
#else
    httpServer.bind(tcpServer.get());
#endif

    /* Print out info and enter application event loop */
    qInfo().noquote() << "Listening for incoming http connections on host" << tcpServer->serverAddress().toString() << "port" << tcpServer->serverPort();
    tcpServer.release();
    return a.exec();
}
