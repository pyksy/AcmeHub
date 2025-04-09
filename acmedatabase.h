#ifndef ACMEDATABASE_H
#define ACMEDATABASE_H

#include "acmebatchdatastruct.h"

class AcmeDatabase
{
public:

    explicit AcmeDatabase();

    ~AcmeDatabase();

    bool AppendAcmeBatchData(AcmeBatchData &acmeBatchData);

private:

    bool InitAcmeDatabase();

};

#endif // ACMEDATABASE_H
