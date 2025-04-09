#ifndef ACMEHUB_H
#define ACMEHUB_H

#include <QObject>

class AcmeHub : public QObject
{
    Q_OBJECT
public:
    explicit AcmeHub(QObject *parent = nullptr);

signals:
};

#endif // ACMEHUB_H
