#ifndef ACMEDATABASE_H
#define ACMEDATABASE_H

#include <QtSql/QSqlQuery>

class AcmeHubDatabase
{
public:

    explicit AcmeHubDatabase();

    ~AcmeHubDatabase();

    bool InitAcmeHubDatabase();

    bool AppendAcmeBatchData(const QJsonObject &jsonObject);

    bool GetProcessStatistics(QJsonObject &statisticsJson);

    bool GetProcessOutliers(QJsonArray &outliersJson);

private:

    int GetMeanAverage(QSqlQuery &);

    int GetStandardDeviation(QSqlQuery &);
};

#endif // ACMEDATABASE_H
