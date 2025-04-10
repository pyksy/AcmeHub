#ifndef ACMEBATCHDATASTRUCT_H
#define ACMEBATCHDATASTRUCT_H

#include <QString>
#include <QDateTime>

struct AcmeHubBatchData {
    QString serverName;
    QDateTime startTime;
    QDateTime endTime;

    AcmeHubBatchData(const QString &serverName,
                     const QString &startTime,
                     const QString &endTime) :
        serverName(serverName),
        startTime(QDateTime::fromString(startTime, Qt::ISODate)),
        endTime(QDateTime::fromString(endTime, Qt::ISODate)) {
    }
};

#endif // ACMEBATCHDATASTRUCT_H
