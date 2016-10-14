#include "MultiThread.h"

EterIndexThread::EterIndexThread(EINSTANCE *pEixInstance, const char *szEterIndexName) :
    pEixInstance(pEixInstance), szEterIndexName(szEterIndexName)
{
}

EterIndexThread::~EterIndexThread()
{
    this->pEixInstance    = 0;
    this->szEterIndexName = 0;
}

void EterIndexThread::run()
{
    *this->pEixInstance = LoadEterIndex(this->szEterIndexName, g_index_key, MODE_READ);

    this->exit();
}

UINT32 EterPackThread::iPointer = 0;

EterPackThread::EterPackThread(EINSTANCE *pEpkInstance, const char *szEterPackName, PLIST pEterIndexItems) :
    pEpkInstance(pEpkInstance), szEterPackName(szEterPackName), pEterIndexItems(pEterIndexItems)
{
    EterPackThread::iPointer = 0;
}

EterPackThread::~EterPackThread()
{
    this->pEpkInstance    = 0;
    this->szEterPackName  = 0;
    this->pEterIndexItems = 0;

    EterPackThread::iPointer = 0;
}

void EterPackThread::run()
{
    if(!*this->pEpkInstance) *this->pEpkInstance = LoadEterPack(this->szEterPackName, g_pack_key, MODE_READ);

    // Check if the istances are valid
    if(this->pEpkInstance == NULL)
        this->exit();

    UINT8 *pEterPackData = GetFileData(*this->pEpkInstance, this->pEterIndexItems[EterPackThread::iPointer]);
    UINT32 pEterPackSize = GetDataSize(*this->pEpkInstance);
    char szFilePath[MAX_PATH], szMainDirectory[MAX_PATH];

    // Check if the DLL returned valid data
    if(pEterPackData == NULL && pEterPackSize > 0)
        this->exit();

    memcpy(szMainDirectory, this->szEterPackName, strlen(this->szEterPackName)-4);
    szMainDirectory[strlen(this->szEterPackName)-4] = 0;

    _snprintf(szFilePath, MAX_PATH, "%s\\%s", szMainDirectory, GetFilteredPath(this->pEterIndexItems[EterPackThread::iPointer]->VirtualPath));
    CheckAndCreateDir(szFilePath);

    _snprintf(szFilePath, MAX_PATH, "%s\\%s", szMainDirectory, GetFilteredPath(this->pEterIndexItems[EterPackThread::iPointer++]->VirtualPath));
    FastIO::FileWrite(szFilePath, "wb", pEterPackData, pEterPackSize);

    this->exit();
}

INT32 PackThread::iPointer = 0;

PackThread::PackThread(QString szDirectoryPath, QStringList lFilesList, EINSTANCE pEixInstance, EINSTANCE pEpkInstance)
    : szDirectoryPath(szDirectoryPath), lFilesList(lFilesList), pEixInstance(pEixInstance), pEpkInstance(pEpkInstance)
{
    PackThread::iPointer = 0;
}

PackThread::~PackThread()
{
    this->szDirectoryPath = QString();
    this->lFilesList      = QStringList();

    this->pEixInstance = 0;
    this->pEpkInstance = 0;

    PackThread::iPointer = 0;
}

void PackThread::run()
{
    if(this->szDirectoryPath.isEmpty() || this->lFilesList.isEmpty() || !this->pEixInstance || !this->pEpkInstance) return;
    if(this->lFilesList.count() < PackThread::iPointer) return;

    UINT32 iOutSize = 0, iHashCRC32 = 0;
    QString szFileVirtualPath = GetDiffFromPaths(this->lFilesList.at(this->iPointer), this->szDirectoryPath).toLower();

    PutFileData(this->pEpkInstance, this->lFilesList.at(this->iPointer++).toAscii().data(), g_storage_type, &iOutSize, &iHashCRC32);
    PutFile(this->pEixInstance, GetIntelVirtualPath(szFileVirtualPath).toAscii().data(), g_storage_type, iOutSize, iHashCRC32);

    this->exit();
}
