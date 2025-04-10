#include <QtSql/QSqlDatabase>
#include <QtSql/QSqlError>
#include <QtSql/QSqlQuery>
#include <QJsonObject>
#include <QJsonArray>
#include <QtMath>
#include <QDateTime>

#include "acmehubdatabase.h"

/* Minimum required data items required to
 * provide processing statistics */
#define MINIMUM_ENTRIES 10

/*
 * Empty default constructor
 *
 **/
AcmeHubDatabase::AcmeHubDatabase()
{
    //
}

/*
 * Destuctor, close databases
 *
 **/
AcmeHubDatabase::~AcmeHubDatabase() {
    qDebug() << Q_FUNC_INFO;

    QSqlDatabase::database("AcmeHubDB").close();
    QSqlDatabase::removeDatabase("AcmeHubDB");
}

/*
 * Initialize SQL database for later use
 *
 **/
bool AcmeHubDatabase::InitAcmeHubDatabase() {
    QSqlDatabase sqlDatabase = QSqlDatabase::addDatabase("QSQLITE", "AcmeHubDB");
    sqlDatabase.setDatabaseName("acmehub.db");

    if (!sqlDatabase.open()) {
        qDebug() << Q_FUNC_INFO << "Error: Cannot open database:" << sqlDatabase.lastError().text();
        return false;
    }

    /* Create database and index if missing */
    QSqlQuery sqlQuery(sqlDatabase);
    if (!sqlQuery.exec("CREATE TABLE IF NOT EXISTS acmebatchdata(servername TEXT NOT NULL, starttime INTEGER NOT NULL, endtime INTEGER NOT NULL, duration INTEGER)")) {
        qDebug() << Q_FUNC_INFO << "Error: Cannot create tables:" << sqlQuery.lastError().text();
        return false;
    }
    if (!sqlQuery.exec("CREATE INDEX IF NOT EXISTS duration_index ON acmebatchdata(duration)")) {
        qDebug() << Q_FUNC_INFO << "Error: Cannot create index" << sqlQuery.lastError().text();
        return false;
    }

    return true;
}

/*
 * Append batch data entries to SQL database
 *
 **/
bool AcmeHubDatabase::AppendAcmeBatchData(const QJsonObject &jsonObject) {

    QSqlDatabase sqlDatabase = QSqlDatabase::database("AcmeHubDB");
    if (!sqlDatabase.isOpen()) {
        qDebug() << Q_FUNC_INFO << "Error: Database closed";
        return false;
    }

    QString serverName(jsonObject.value("server_name").toString());
    QDateTime startTime = QDateTime::fromString(jsonObject.value("start_time").toString(), Qt::ISODate);
    QDateTime endTime = QDateTime::fromString(jsonObject.value("end_time").toString(), Qt::ISODate);
    qDebug() << Q_FUNC_INFO << "serverName:" << serverName << "startTime:" << startTime << "endTime:" << endTime << "duration:" << startTime.secsTo(endTime);

    QSqlQuery sqlQuery(sqlDatabase);
    sqlQuery.prepare("INSERT INTO acmebatchdata(servername, starttime, endtime, duration) VALUES(:servername, :starttime, :endtime, :duration)");
    sqlQuery.bindValue(":servername", serverName);
    sqlQuery.bindValue(":starttime", startTime);
    sqlQuery.bindValue(":endtime", endTime);
    sqlQuery.bindValue(":duration", startTime.secsTo(endTime));

    return sqlQuery.exec();
}

/*
 * Calculate and return some statistics as JSON
 *
 **/
bool AcmeHubDatabase::GetProcessStatistics(QJsonObject &statisticsJson) {

    QSqlDatabase sqlDatabase = QSqlDatabase::database("AcmeHubDB");
    if (!sqlDatabase.isOpen()) {
        qDebug() << Q_FUNC_INFO << "Error: Database closed";
        return false;
    }

    /* Return error JSON if not enough data */
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

/*
 * Fetch and return statistically anomalous server names as JSON
 *
 **/
bool AcmeHubDatabase::GetProcessOutliers(QJsonArray &outliersJson) {

    QSqlDatabase sqlDatabase = QSqlDatabase::database("AcmeHubDB");
    if (!sqlDatabase.isOpen()) {
        qDebug() << Q_FUNC_INFO << "Error: Database closed";
        return false;
    }

    QSqlQuery sqlQuery(sqlDatabase);

    int meanAverage = GetMeanAverage(sqlQuery);
    int standardDeviation = GetStandardDeviation(sqlQuery);

    qDebug() << Q_FUNC_INFO << "mean average:" << meanAverage << "stddev" << standardDeviation;

    /* Fetch outliers, can be optimized with Forward Only flag */
    sqlQuery.setForwardOnly(true);
    sqlQuery.prepare("SELECT DISTINCT servername FROM acmebatchdata WHERE duration < :lowerbound OR duration > :upperbound");
    sqlQuery.bindValue(":lowerbound", QString::number(-3*standardDeviation + meanAverage));
    sqlQuery.bindValue(":upperbound", QString::number(3*standardDeviation + meanAverage));
    if (!sqlQuery.exec()) {
        qDebug() << Q_FUNC_INFO << "query failed";
        return false;
    }

    /* Append outlier server names to JSON list */
    while(sqlQuery.next()) {
        outliersJson.append(QJsonValue::fromVariant(sqlQuery.value(0).toString()));
    }

    qDebug() << Q_FUNC_INFO << outliersJson;
    return true;
}

/*
 * Return mean average from data in SQL database
 *
 **/
int AcmeHubDatabase::GetMeanAverage(QSqlQuery &sqlQuery) {
    if (!sqlQuery.exec("SELECT AVG(duration) FROM acmebatchdata")
        || !sqlQuery.next()) {
        qDebug() << Q_FUNC_INFO << "failed to fetch mean average";
        return 0;
    }

    return sqlQuery.value(0).toInt();
}

/*
 * Return standard deviation from data in SQL database
 *
 **/
int AcmeHubDatabase::GetStandardDeviation(QSqlQuery &sqlQuery) {
    /* Get variance */
    if (!sqlQuery.exec("SELECT AVG((acmebatchdata.duration - sub.a) * (acmebatchdata.duration - sub.a)) AS var FROM acmebatchdata, (SELECT AVG(duration) AS a FROM acmebatchdata) AS sub")
        || !sqlQuery.next()) {
        qDebug() << Q_FUNC_INFO << "failed to fetch variance";
        return 0;
    }

    /* Square root the variance to return standard deviance */
    return qRound(qSqrt(sqlQuery.value(0).toFloat()));
}
