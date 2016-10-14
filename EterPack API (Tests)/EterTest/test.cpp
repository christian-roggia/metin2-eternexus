#include "test.h"
#include "ui_test.h"

Test::Test(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::Test)
{
    ui->setupUi(this);
}

Test::~Test()
{
    delete ui;
}

#include <windows.h>
#include <fstream>

#include "EterPack API.h"

using namespace EterPackAPI;

void Test::on_pushButton_clicked()
{
    LoadEterPackAPI();

    EINSTANCE pEixInstance = LoadEterIndex("test2.eix", NULL, MODE_READ);
    EINSTANCE pEpkInstance = LoadEterPack("test2.epk", NULL, MODE_READ);

    if(!pEixInstance || !pEpkInstance) return;

    PLIST pItemList   = GetFilesList(pEixInstance);
    UINT32 pFileCount = GetFileCount(pEixInstance);

    ui->tableWidget->setRowCount(pFileCount);

    for(UINT32 i = 0; i < pFileCount; i++)
    {
        ui->tableWidget->setItem(i, 0, new QTableWidgetItem(QString::number(pItemList[i]->Index)));
        ui->tableWidget->setItem(i, 1, new QTableWidgetItem(pItemList[i]->VirtualPath));
        ui->tableWidget->setItem(i, 2, new QTableWidgetItem(QString::number(pItemList[i]->UnkZeroDword)));
        ui->tableWidget->setItem(i, 3, new QTableWidgetItem(QString::number(pItemList[i]->VirtualPathCRC32)));
        ui->tableWidget->setItem(i, 4, new QTableWidgetItem(QString::number(pItemList[i]->StoredSize)));
        ui->tableWidget->setItem(i, 5, new QTableWidgetItem(QString::number(pItemList[i]->Size)));
        ui->tableWidget->setItem(i, 6, new QTableWidgetItem(QString::number(pItemList[i]->CRC32)));
        ui->tableWidget->setItem(i, 7, new QTableWidgetItem(QString::number(pItemList[i]->Offset)));
        ui->tableWidget->setItem(i, 8, new QTableWidgetItem(QString::number(pItemList[i]->StorageType)));

        UINT8 *pEterData = GetFileData(pEpkInstance, pItemList[i]);
        UINT32 iEterSize = GetDataSize(pEpkInstance);

        if(!pEterData || !iEterSize) break;

        FILE *pEterFile = fopen(pItemList[i]->VirtualPath, "wb");
        fwrite(pEterData, 1, iEterSize, pEterFile);
        fclose(pEterFile);
    }

    FreeEterIndex(pEixInstance);
    FreeEterPack(pEpkInstance);

    FreeEterPackAPI();
}

void Test::on_pushButton_2_clicked()
{
    LoadEterPackAPI();

    EINSTANCE pEixInstance = LoadEterIndex("test2.eix", NULL, MODE_WRITE);
    EINSTANCE pEpkInstance = LoadEterPack("test2.epk", NULL, MODE_WRITE);

    UINT32 iOutSize = 0, iHashCRC32 = 0;

    if(!pEixInstance || !pEpkInstance) return;

    InitPack(pEixInstance, 3);

    PutFileData(pEpkInstance, "moc_test.cpp", LZO_COMPRESSION_NO_ENCODING, &iOutSize, &iHashCRC32);
    PutFile(pEixInstance, "test1", LZO_COMPRESSION_NO_ENCODING, iOutSize, iHashCRC32);
    PutFileData(pEpkInstance, "moc_test.o", LZO_COMPRESSION_NO_ENCODING, &iOutSize, &iHashCRC32);
    PutFile(pEixInstance, "test2", LZO_COMPRESSION_NO_ENCODING, iOutSize, iHashCRC32);
    PutFileData(pEpkInstance, "main.o", LZO_COMPRESSION_NO_ENCODING, &iOutSize, &iHashCRC32);
    PutFile(pEixInstance, "test3", LZO_COMPRESSION_NO_ENCODING, iOutSize, iHashCRC32);

    WritePack(pEixInstance);

    FreeEterIndex(pEixInstance);
    FreeEterPack(pEpkInstance);

    FreeEterPackAPI();
}
