#ifndef MULTITHREAD_H
#define MULTITHREAD_H

#include <QThread>

#include "API/EterPackAPI.hpp"
#include "Global/Utils.h"
#include "Global/IO.h"
#include "Global/Global.h"

class EterIndexThread : public QThread
{
    Q_OBJECT
public:
    explicit EterIndexThread(EINSTANCE *pEixInstance, const char *szEterIndexName);
    ~EterIndexThread();

    void run();

public:
    EINSTANCE *pEixInstance;
    const char *szEterIndexName;
};

class EterPackThread : public QThread
{
    Q_OBJECT
public:
    explicit EterPackThread(EINSTANCE *pEpkInstance, const char *szEterPackName, PLIST pEterIndexItems);
    ~EterPackThread();

    void run();

public:
    EINSTANCE *pEpkInstance;
    const char *szEterPackName;
    PLIST pEterIndexItems;

    static UINT32 iPointer;
};

class PackThread : public QThread
{
    Q_OBJECT
public:
    explicit PackThread(QString szDirectoryPath, QStringList lFilesList, EINSTANCE pEixInstance, EINSTANCE pEpkInstance);
    ~PackThread();

    void run();

public:
    QString szDirectoryPath;
    QStringList lFilesList;
    EINSTANCE pEixInstance, pEpkInstance;

    static INT32 iPointer;
};

#endif // MULTITHREAD_H
