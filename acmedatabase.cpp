#include <QtSql/QSqlDatabase>
#include <QtSql/QSqlError>
#include <QtSql/QSqlQuery>
#include <QJsonObject>
#include <QtMath>

#include "acmedatabase.h"
#include "acmebatchdatastruct.h"

#define MINIMUM_ENTRIES 10

AcmeDatabase::AcmeDatabase()
{
    //
}

AcmeDatabase::~AcmeDatabase() {
    qDebug() << Q_FUNC_INFO;

    QSqlDatabase::database("AcmeHubDB").close();
    QSqlDatabase::removeDatabase("AcmeHubDB");
}

bool AcmeDatabase::InitAcmeDatabase() {

    QSqlDatabase sqlDatabase = QSqlDatabase::addDatabase("QSQLITE", "AcmeHubDB");
    sqlDatabase.setDatabaseName("acmehub.db");

    if (!sqlDatabase.open()) {
        qDebug() << Q_FUNC_INFO << "Error: Cannot open database:" << sqlDatabase.lastError().text();
        return false;
    }

    QSqlQuery sqlQuery(sqlDatabase);
    if (!sqlQuery.exec("CREATE TABLE IF NOT EXISTS acmebatchdata(servername TEXT NOT NULL, starttime INTEGER NOT NULL, endtime INTEGER NOT NULL, duration INTEGER)")) {
        qDebug() << Q_FUNC_INFO << "Error: Cannot create tables:" << sqlQuery.lastError().text();
        return false;
    }

    return true;
}

bool AcmeDatabase::AppendAcmeBatchData(const AcmeBatchData &acmeBatchData) {

    QSqlDatabase sqlDatabase = QSqlDatabase::database("AcmeHubDB");
    if (!sqlDatabase.isOpen()) {
        qDebug() << Q_FUNC_INFO << "Error: Database closed";
        return false;
    }

    qDebug() << Q_FUNC_INFO << "serverName:" << acmeBatchData.serverName;
    qDebug() << Q_FUNC_INFO << "startTime: " << acmeBatchData.startTime;
    qDebug() << Q_FUNC_INFO << "endTime:   " << acmeBatchData.endTime;

    QSqlQuery sqlQuery(sqlDatabase);
    sqlQuery.prepare("INSERT INTO acmebatchdata(servername, starttime, endtime, duration) VALUES(:servername, :starttime, :endtime, :duration)");
    sqlQuery.bindValue(":servername", acmeBatchData.serverName);
    sqlQuery.bindValue(":starttime", acmeBatchData.startTime);
    sqlQuery.bindValue(":endtime", acmeBatchData.endTime);
    sqlQuery.bindValue(":duration", acmeBatchData.startTime.secsTo(acmeBatchData.endTime));

    return sqlQuery.exec();
}

bool AcmeDatabase::GetProcessStatistics(QJsonObject &statisticsObject) {

    QSqlDatabase sqlDatabase = QSqlDatabase::database("AcmeHubDB");
    if (!sqlDatabase.isOpen()) {
        qDebug() << Q_FUNC_INFO << "Error: Database closed";
        return false;
    }

    QSqlQuery sqlQuery(sqlDatabase);
    if (!sqlQuery.exec("SELECT COUNT(servername) FROM acmebatchdata")
        || !sqlQuery.next()
        || sqlQuery.value(0).toInt() < MINIMUM_ENTRIES) {

        statisticsObject["error"] = "Not enough reports received.";

        qDebug() << Q_FUNC_INFO << "not enough data";
        return false;
    }

    // Get mean averages
    if (!sqlQuery.exec("SELECT AVG(duration) FROM acmebatchdata")
        || !sqlQuery.next()) {
        qDebug() << Q_FUNC_INFO << "failed to fetch mean average";
        return false;
    }
    statisticsObject["mean"] = QString::number(sqlQuery.value(0).toInt());

    // Get standard deviation (by square rooting the variance)
    if (!sqlQuery.exec("SELECT AVG((acmebatchdata.duration - sub.a) * (acmebatchdata.duration - sub.a)) AS var FROM acmebatchdata, (SELECT AVG(duration) AS a FROM acmebatchdata) AS sub")
        || !sqlQuery.next()) {
        qDebug() << Q_FUNC_INFO << "failed to fetch variance";
    }
    statisticsObject["stddev"] = QString::number(qRound(qSqrt(sqlQuery.value(0).toFloat())));

    qDebug() << Q_FUNC_INFO << statisticsObject;

    return true;
}
