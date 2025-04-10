#include <QtSql/QSqlDatabase>
#include <QtSql/QSqlError>
#include <QtSql/QSqlQuery>

#include "acmedatabase.h"
#include "acmebatchdatastruct.h"

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
    qDebug() << Q_FUNC_INFO;

    QSqlDatabase sqlDatabase = QSqlDatabase::addDatabase("QSQLITE", "AcmeHubDB");
    sqlDatabase.setDatabaseName("acmehub.db");

    if (!sqlDatabase.open()) {
        qDebug() << Q_FUNC_INFO << "Error: Cannot open database:" << sqlDatabase.lastError().text();
        return false;
    }

    QSqlQuery sqlQuery(sqlDatabase);
    if (!sqlQuery.exec("CREATE TABLE IF NOT EXISTS acmebatchdata(servername TEXT NOT NULL, starttime INTEGER NOT NULL, endtime INTEGER NOT NULL, duration INTEGER)")) {
        qDebug() << Q_FUNC_INFO << "Error: Cannot create tables:" << sqlDatabase.lastError().text();
        return false;
    }

    return true;
}

bool AcmeDatabase::AppendAcmeBatchData(AcmeBatchData &acmeBatchData) {
    qDebug() << Q_FUNC_INFO;

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
