#include <QtSql/QSqlDatabase>
#include <QtSql/QSqlError>
#include <QtSql/QSqlQuery>
#include <QJsonObject>
#include <QJsonArray>
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

    qDebug() << Q_FUNC_INFO << "serverName:" << acmeBatchData.serverName << "startTime:" << acmeBatchData.startTime << "endTime:" << acmeBatchData.endTime;

    QSqlQuery sqlQuery(sqlDatabase);
    sqlQuery.prepare("INSERT INTO acmebatchdata(servername, starttime, endtime, duration) VALUES(:servername, :starttime, :endtime, :duration)");
    sqlQuery.bindValue(":servername", acmeBatchData.serverName);
    sqlQuery.bindValue(":starttime", acmeBatchData.startTime);
    sqlQuery.bindValue(":endtime", acmeBatchData.endTime);
    sqlQuery.bindValue(":duration", acmeBatchData.startTime.secsTo(acmeBatchData.endTime));

    return sqlQuery.exec();
}

bool AcmeDatabase::GetProcessStatistics(QJsonObject &statisticsJson) {

    QSqlDatabase sqlDatabase = QSqlDatabase::database("AcmeHubDB");
    if (!sqlDatabase.isOpen()) {
        qDebug() << Q_FUNC_INFO << "Error: Database closed";
        return false;
    }

    QSqlQuery sqlQuery(sqlDatabase);
    if (!sqlQuery.exec("SELECT COUNT(servername) FROM acmebatchdata")
        || !sqlQuery.next()
        || sqlQuery.value(0).toInt() < MINIMUM_ENTRIES) {

        statisticsJson["error"] = "Not enough reports received.";

        qDebug() << Q_FUNC_INFO << "not enough data";
        return false;
    }

    statisticsJson["mean"] = QString::number(GetMeanAverage(sqlQuery));
    statisticsJson["stddev"] = QString::number(GetStandardDeviation(sqlQuery));

    qDebug() << Q_FUNC_INFO << statisticsJson;
    return true;
}

bool AcmeDatabase::GetProcessOutliers(QJsonArray &outliersJson) {

    QSqlDatabase sqlDatabase = QSqlDatabase::database("AcmeHubDB");
    if (!sqlDatabase.isOpen()) {
        qDebug() << Q_FUNC_INFO << "Error: Database closed";
        return false;
    }

    QSqlQuery sqlQuery(sqlDatabase);
    sqlQuery.setForwardOnly(true);

    int meanAverage = GetMeanAverage(sqlQuery);
    int standardDeviation = GetStandardDeviation(sqlQuery);

    qDebug() << Q_FUNC_INFO << "mean average:" << meanAverage << "stddev" << standardDeviation;

    sqlQuery.prepare("SELECT DISTINCT servername FROM acmebatchdata WHERE duration < :lowerbound OR duration > :upperbound");
    sqlQuery.bindValue(":lowerbound", QString::number(-3*standardDeviation + meanAverage));
    sqlQuery.bindValue(":upperbound", QString::number(3*standardDeviation + meanAverage));
    if (!sqlQuery.exec()) {
        qDebug() << Q_FUNC_INFO << "query failed";
        return false;
    }

    while(sqlQuery.next()) {
        outliersJson.append(QJsonValue::fromVariant(sqlQuery.value(0).toString()));
    }

    qDebug() << Q_FUNC_INFO << outliersJson;
    return true;
}

int AcmeDatabase::GetMeanAverage(QSqlQuery &sqlQuery) {
    if (!sqlQuery.exec("SELECT AVG(duration) FROM acmebatchdata")
        || !sqlQuery.next()) {
        qDebug() << Q_FUNC_INFO << "failed to fetch mean average";
        return 0;
    }

    return sqlQuery.value(0).toInt();
}

int AcmeDatabase::GetStandardDeviation(QSqlQuery &sqlQuery) {
    // Get variance
    if (!sqlQuery.exec("SELECT AVG((acmebatchdata.duration - sub.a) * (acmebatchdata.duration - sub.a)) AS var FROM acmebatchdata, (SELECT AVG(duration) AS a FROM acmebatchdata) AS sub")
        || !sqlQuery.next()) {
        qDebug() << Q_FUNC_INFO << "failed to fetch variance";
        return 0;
    }

    // Square root the variance to return standard deviance
    return qRound(qSqrt(sqlQuery.value(0).toFloat()));
}
