#ifndef ACMEBATCHDATASTRUCT_H
#define ACMEBATCHDATASTRUCT_H

#include <QString>
#include <QDateTime>

struct AcmeBatchData {
    QString serverName;
    QDateTime startTime;
    QDateTime endTime;

    AcmeBatchData(const QString &serverName,
                         const QString &startTime,
                         const QString &endTime) :
        serverName(serverName),
        startTime(QDateTime::fromString(startTime, Qt::ISODate)),
        endTime(QDateTime::fromString(endTime, Qt::ISODate)) {
    }
};

#endif // ACMEBATCHDATASTRUCT_H
