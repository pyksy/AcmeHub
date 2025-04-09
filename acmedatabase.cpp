#include <QtSql/QSqlDatabase>
#include <QtSql/QSqlError>
#include <QtSql/QSqlQuery>

#include "acmedatabase.h"
#include "acmebatchdatastruct.h"

AcmeDatabase::AcmeDatabase()
{
    qDebug() << Q_FUNC_INFO;
    InitAcmeDatabase();
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
    if (!sqlQuery.exec("CREATE TABLE IF NOT EXISTS acmebatchdata(servername TEXT, starttime INTEGER, endtime INTEGER)")) {
        qDebug() << Q_FUNC_INFO << "Error: Cannot create tables:" << sqlDatabase.lastError().text();
        return false;
    }
    return true;
}

bool AcmeDatabase::AppendAcmeBatchData(AcmeBatchData &acmeBatchData) {
    qDebug() << Q_FUNC_INFO << "serverName:" << acmeBatchData.serverName;
    qDebug() << Q_FUNC_INFO << "startTime: " << acmeBatchData.startTime;
    qDebug() << Q_FUNC_INFO << "endTime:   " << acmeBatchData.endTime;

    return true;
}
