#include "EterNexus.h"
#include "ui_EterNexus.h"

UINT32 iInPackCount = 0, iInCurrentPack = 0;

EINSTANCE pEixInstance = 0, pEpkInstance = 0;
char szEixName[MAX_PATH] = {}, szEpkName[MAX_PATH] = {};
PackThread *pPackThread = 0;
QString szDirectoryPath;

void EterNexus::PackArchive()
{
    // Check if the a directory has been passed as argument
    if(!s_processByArg)
    {
        // If not open an open-directory-dialog
        szDirectoryPath = QFileDialog::getExistingDirectory(this, tr("Pack Archive"), "", QFileDialog::ShowDirsOnly);
        if(szDirectoryPath.isEmpty()) return;
    }
    else
    {
        // Copy the directory name
        szDirectoryPath = QString(s_argv[2]);
    }

    QString szDirectoryName = GetFileNameFromPath(szDirectoryPath);
    QFile pFileEpk, pFileEix;

    // Copy the directory name + EterIndex extension
    strncpy(szEixName, szDirectoryPath.left(szDirectoryPath.lastIndexOf("\\")+1).toAscii().data(), MAX_PATH);
    strncat(szEixName, szDirectoryName.toAscii().data(), MAX_PATH);
    strncat(szEixName, g_index_ext, MAX_PATH);

    // Copy the directory name + EterPack extension
    strncpy(szEpkName, szDirectoryPath.left(szDirectoryPath.lastIndexOf("\\")+1).toAscii().data(), MAX_PATH);
    strncat(szEpkName, szDirectoryName.toAscii().data(), MAX_PATH);
    strncat(szEpkName, g_pack_ext, MAX_PATH);

    // Remove files if already exists
    // TODO: Ask confirm
    pFileEpk.setFileName(szEpkName);
    pFileEix.setFileName(szEixName);

    if(pFileEpk.exists()) pFileEpk.remove();
    if(pFileEix.exists()) pFileEix.remove();

    // Load EterIndex and EterPack
    pEixInstance = LoadEterIndex(szEixName, g_index_key, MODE_WRITE);
    pEpkInstance = LoadEterPack (szEpkName, g_pack_key , MODE_WRITE);

    if(!pEixInstance || !pEpkInstance) return;

    // Get all files
    QStringList lFilesList = GetFilesListFromDirectory(szDirectoryPath);

    // Setup global variables
    iInPackCount   = lFilesList.count()-1;
    iInCurrentPack = 0;

    // Setup GUI
    ui->pBarIndexes->setMaximum(0);
    ui->txtIndexes->setText("Elaborating the archive...");
    ui->pBarPacks->setMaximum(iInPackCount+1);

    // Send information to the server
    this->UpdateAds("FILECREATE", szDirectoryName.toAscii().data());

    // Allocate memory and initialize pack
    InitPack(pEixInstance, lFilesList.count());

    // Setup the thread
    pPackThread = new PackThread(szDirectoryPath, lFilesList, pEixInstance, pEpkInstance);
    connect(pPackThread, SIGNAL(finished()), this, SLOT(RoutinePackArchive()));

    // Launch the thread
    ui->txtPacks->setText(QString("Adding: %0").arg(GetDiffFromPaths(GetFilteredPath(lFilesList.at(iInCurrentPack++).toAscii().data()), szDirectoryPath)));
    pPackThread->start();
}

void EterNexus::RoutinePackArchive()
{
    if(!pEixInstance || !pEpkInstance) return;

    if(iInCurrentPack <= iInPackCount)
    {
        ui->pBarPacks->setValue(iInCurrentPack);
        ui->txtPacks->setText(QString("Adding: %0").arg(GetDiffFromPaths(GetFilteredPath(pPackThread->lFilesList.at(iInCurrentPack++).toAscii().data()), szDirectoryPath)));
        pPackThread->start();

        return;
    }

    WritePack(pEixInstance);

    FreeEterIndex(pEixInstance);
    FreeEterPack(pEpkInstance);

    iInPackCount = 0;
    iInCurrentPack = 0;

    ui->txtPacks->clear();
    ui->txtIndexes->clear();

    ui->pBarIndexes->setValue(0);
    ui->pBarIndexes->setMaximum(1);
    ui->pBarPacks->setValue(0);
    ui->pBarPacks->setMaximum(1);

    if(s_processByArg)
        this->close();
}

