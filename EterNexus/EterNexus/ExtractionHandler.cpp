#include "EterNexus.h"
#include "ui_EterNexus.h"

UINT32 iFileCount = 0, iCurrentFile = 0;
QStringList lFileNames;

void EterNexus::ExtractArchive()
{
    if(!s_processByArg)
    {
        lFileNames = QFileDialog::getOpenFileNames
                (
                    this,
                    tr("Open Encrypted Files"),
                    QString(),
                    tr(QString("Encrypted Index Files (*"+QString(g_index_ext)+");;Encrypted Pack Files (*"+QString(g_pack_ext)+")").toAscii().data())
                );
    }
    else
    {
        lFileNames = QStringList(s_argv[2]);

        for(int i = 3; i < s_argc; i++)
            lFileNames.append(s_argv[i]);
    }

    if(!lFileNames.count()) return;

    iFileCount = lFileNames.count()-1;
    iCurrentFile = 0;

    ui->pBarIndexes->setMaximum(iFileCount+1);

    this->RoutineIndexes();
}

EINSTANCE pInstanceEix = 0, pInstanceEpk = 0;
UINT32 iPackCount = 0, iCurrentPack = 0;
char *szEterPackName = 0, *szEterIndexName = 0;
EterIndexThread *pEterIndexThread = 0;
EterPackThread *pEterPackThread = 0;
PLIST pEterIndexItems = 0;

void ClearAll()
{
    if(pInstanceEix) FreeEterIndex(pInstanceEix);
    if(pInstanceEpk) FreeEterPack (pInstanceEpk);

    pInstanceEpk = 0;
    pInstanceEix = 0;

    if(szEterIndexName) free(szEterIndexName);
    if(szEterPackName)  free(szEterPackName );

    szEterIndexName = 0;
    szEterPackName  = 0;

    if(pEterIndexThread) pEterIndexThread->deleteLater();
    if(pEterPackThread)  pEterPackThread ->deleteLater();

    pEterIndexThread = 0;
    pEterPackThread  = 0;

    pEterIndexItems = 0;
}

void EterNexus::RoutineIndexes()
{
    ClearAll();

    if(iFileCount < iCurrentFile)
    {
        ui->txtIndexes->clear();
        ui->pBarIndexes->setValue(0);
        ui->pBarIndexes->setMaximum(1);

        iFileCount = 0;
        iCurrentFile = 0;

        return;
    }

    QString szCurrentFilePath = lFileNames.at(iCurrentFile);
    QString szCurrentFileName = szCurrentFilePath.right(szCurrentFilePath.length()-szCurrentFilePath.lastIndexOf("\\")-1);
    int iCurrentFileSize = szCurrentFilePath.length();
    int iExtIndex = (szCurrentFilePath.lastIndexOf(".") != -1) ? szCurrentFilePath.lastIndexOf(".") : szCurrentFilePath.length();

    szEterPackName  = (char *)memcpy(malloc(iCurrentFileSize+1+4), szCurrentFilePath.toAscii().data(), iCurrentFileSize+1);
    szEterIndexName = (char *)memcpy(malloc(iCurrentFileSize+1+4), szCurrentFilePath.toAscii().data(), iCurrentFileSize+1);

    memcpy(&szEterIndexName[iExtIndex], g_index_ext, 5);
    memcpy(&szEterPackName [iExtIndex], g_pack_ext , 5);

    ui->txtIndexes->setText(QString("Extracting: "+szCurrentFileName));
    ui->txtPacks->setText("Obtaining information...");
    ui->pBarPacks->setMaximum(0);

    this->UpdateAds("FILEOPEN", szCurrentFileName.toAscii().data());

    pEterIndexThread = new EterIndexThread(&pInstanceEix, szEterIndexName);
    connect(pEterIndexThread, SIGNAL(finished()), this, SLOT(ElaborateIndexes()));

    pEterIndexThread->start();
}

void EterNexus::ElaborateIndexes()
{
    ui->pBarIndexes->setValue(++iCurrentFile);

    if(!pInstanceEix)
    {
        this->RoutinePacks();
        return;
    }

    pEterIndexItems = GetFilesList(pInstanceEix);
    iPackCount      = GetFileCount(pInstanceEix);
    iCurrentPack    = 0;

    if(iPackCount == 0 || !pEterIndexItems)
    {
        this->RoutinePacks();
        return;
    }

    ui->pBarPacks->setMaximum(iPackCount);

    pEterPackThread = new EterPackThread(&pInstanceEpk, szEterPackName, pEterIndexItems);
    connect(pEterPackThread, SIGNAL(finished()), this, SLOT(RoutinePacks()));

    ui->txtPacks->setText(QString("Extracting: %0").arg(GetFilteredPath(pEterIndexItems[iCurrentPack++]->VirtualPath)));
    pEterPackThread->start();
}

void EterNexus::RoutinePacks()
{
    if(pInstanceEix && pEterIndexItems)
    {
        if(iCurrentPack < iPackCount)
        {
            ui->pBarPacks->setValue(iCurrentPack);
            ui->txtPacks->setText(QString("Extracting: %0").arg(GetFilteredPath(pEterIndexItems[iCurrentPack++]->VirtualPath)));
            pEterPackThread->start();

            return;
        }
    }

    ui->txtPacks->clear();
    ui->pBarPacks->setValue(0);
    ui->pBarPacks->setMaximum(1);

    iPackCount = 0;
    iCurrentPack = 0;

    this->RoutineIndexes();
}
