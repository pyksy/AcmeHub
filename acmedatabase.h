#ifndef ACMEDATABASE_H
#define ACMEDATABASE_H

#include <QtSql/QSqlQuery>

#include "acmebatchdatastruct.h"

class AcmeDatabase
{
public:

    explicit AcmeDatabase();

    ~AcmeDatabase();

    bool AppendAcmeBatchData(const AcmeBatchData &acmeBatchData);

    bool InitAcmeDatabase();

    bool GetProcessStatistics(QJsonObject &statisticsJson);

    bool GetProcessOutliers(QJsonArray &outliersJson);

private:

    int GetMeanAverage(QSqlQuery &);

    int GetStandardDeviation(QSqlQuery &);
};

#endif // ACMEDATABASE_H
