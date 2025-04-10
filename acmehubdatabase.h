#ifndef ACMEDATABASE_H
#define ACMEDATABASE_H

#include <QtSql/QSqlQuery>

#include "acmebatchdatastruct.h"

class AcmeHubDatabase
{
public:

    explicit AcmeHubDatabase();

    ~AcmeHubDatabase();

    bool AppendAcmeBatchData(const AcmeHubBatchData &acmeBatchData);

    bool InitAcmeHubDatabase();

    bool GetProcessStatistics(QJsonObject &statisticsJson);

    bool GetProcessOutliers(QJsonArray &outliersJson);

private:

    int GetMeanAverage(QSqlQuery &);

    int GetStandardDeviation(QSqlQuery &);
};

#endif // ACMEDATABASE_H
