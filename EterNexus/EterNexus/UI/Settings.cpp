#include "Settings.h"
#include "ui_Settings.h"

Settings::Settings(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Settings)
{
    ui->setupUi(this);

    this->setFixedSize(this->width(), this->height());
    this->setWindowFlags(Qt::WindowSystemMenuHint);

    this->Init();
}

Settings::~Settings()
{
    delete ui;
}

void Settings::Init()
{
    this->LoadSettings();

    ui->lStorageType->setEnabled(true);
    ui->cStorageType->setEnabled(true);

    ui->cStorageType->setCurrentIndex(g_storage_type);

    ui->lIndexExt->setEnabled(true);
    ui->lPackExt->setEnabled(true);

    ui->eIndexExt->setEnabled(true);
    ui->ePackExt->setEnabled(true);

    this->UpdateExtensions();

    ui->lIndexKey->setEnabled(true);
    ui->lPackKey->setEnabled(true);

    ui->eIndexKey1->setEnabled(true);
    ui->eIndexKey2->setEnabled(true);
    ui->eIndexKey3->setEnabled(true);
    ui->eIndexKey4->setEnabled(true);
    ui->ePackKey1->setEnabled(true);
    ui->ePackKey2->setEnabled(true);
    ui->ePackKey3->setEnabled(true);
    ui->ePackKey4->setEnabled(true);

    this->UpdateKeys();

#ifndef ADWARE_VERSION
    ui->cAds->setEnabled(true);
    ui->cAds->setChecked(g_show_ads);
#endif
}

typedef struct _ETER_SETTINGS {
    unsigned char StorageType;
    char IndexExtension[5];
    char PackExtension[5];
    UINT32 IndexKey[4];
    UINT32 PackKey[4];
    bool AdsEnabled;
    bool GenerateLogs;
} ETER_SETTINGS, *PETER_SETTINGS;

void Settings::LoadSettings()
{
    char *pSettings = 0;
    UINT32 iSettingsSize = 0;
    PETER_SETTINGS pEterSettings;

    FastIO::FileRead("CONFIG", "rb", (void **)&pSettings, &iSettingsSize);

    if(!pSettings || !pSettings || iSettingsSize != sizeof(ETER_SETTINGS))
    {
        if(iSettingsSize != sizeof(ETER_SETTINGS))
            sys_err(__FILE__, __LINE__, "Settings file is malformed!");

        this->ResetExtensions();
        this->UpdateExtensions();

        this->ResetKeys();
        this->UpdateKeys();

        this->SaveSettings();
        return;
    }

    pEterSettings = (PETER_SETTINGS)pSettings;

    g_storage_type = pEterSettings->StorageType > 1 ? (STORAGE_TYPE)1 : (STORAGE_TYPE)pEterSettings->StorageType;

    memcpy(g_index_ext, pEterSettings->IndexExtension, 5);
    g_index_ext[4] = 0;

    memcpy(g_pack_ext , pEterSettings->PackExtension , 5);
    g_pack_ext[4]  = 0;

    memcpy(g_index_key, pEterSettings->IndexKey, sizeof(UINT32) * 4);
    memcpy(g_pack_key , pEterSettings->PackKey , sizeof(UINT32) * 4);

#ifndef ADWARE_VERSION
    g_show_ads = pEterSettings->AdsEnabled;
#endif
}

void Settings::SaveSettings()
{
    ETER_SETTINGS pEterSettings;

    g_storage_type = (STORAGE_TYPE)ui->cStorageType->currentIndex();
    pEterSettings.StorageType = g_storage_type;

    strncpy(g_index_ext, ui->eIndexExt->text().toAscii().data(), 4);
    g_index_ext[4] = 0;

    strncpy(g_pack_ext, ui->ePackExt->text().toAscii().data(), 4);
    g_pack_ext[4]  = 0;

    if(!IsValidExtension(g_index_ext) || !IsValidExtension(g_pack_ext))
    {
        sys_err(__FILE__, __LINE__, "Invalid extension found: %s %s", g_index_ext, g_pack_ext);

        this->ResetExtensions();
        this->UpdateExtensions();
    }

    strncpy(pEterSettings.IndexExtension, g_index_ext, 4);
    strncpy(pEterSettings.PackExtension , g_pack_ext , 4);

    memcpyi(&g_index_key[0], strtol(ui->eIndexKey1->text().toAscii().data(), NULL, 16), sizeof(UINT32));
    memcpyi(&g_index_key[1], strtol(ui->eIndexKey2->text().toAscii().data(), NULL, 16), sizeof(UINT32));
    memcpyi(&g_index_key[2], strtol(ui->eIndexKey3->text().toAscii().data(), NULL, 16), sizeof(UINT32));
    memcpyi(&g_index_key[3], strtol(ui->eIndexKey4->text().toAscii().data(), NULL, 16), sizeof(UINT32));

    memcpyi(&g_pack_key[0], strtol(ui->ePackKey1->text().toAscii().data(), NULL, 16), sizeof(UINT32));
    memcpyi(&g_pack_key[1], strtol(ui->ePackKey2->text().toAscii().data(), NULL, 16), sizeof(UINT32));
    memcpyi(&g_pack_key[2], strtol(ui->ePackKey3->text().toAscii().data(), NULL, 16), sizeof(UINT32));
    memcpyi(&g_pack_key[3], strtol(ui->ePackKey4->text().toAscii().data(), NULL, 16), sizeof(UINT32));

    memcpy(pEterSettings.IndexKey, g_index_key, sizeof(UINT32) * 4);
    memcpy(pEterSettings.PackKey , g_pack_key , sizeof(UINT32) * 4);

#ifndef ADWARE_VERSION
    g_show_ads = pEterSettings.AdsEnabled = ui->cAds->isChecked();
#endif

    FastIO::FileWrite("CONFIG", "wb", &pEterSettings, sizeof(ETER_SETTINGS));
}

void Settings::UpdateKeys()
{
    ui->eIndexKey1->setText(hprintf(g_index_key[0]));
    ui->eIndexKey2->setText(hprintf(g_index_key[1]));
    ui->eIndexKey3->setText(hprintf(g_index_key[2]));
    ui->eIndexKey4->setText(hprintf(g_index_key[3]));

    ui->ePackKey1->setText(hprintf(g_pack_key[0]));
    ui->ePackKey2->setText(hprintf(g_pack_key[1]));
    ui->ePackKey3->setText(hprintf(g_pack_key[2]));
    ui->ePackKey4->setText(hprintf(g_pack_key[3]));
}

void Settings::ResetKeys()
{
    UINT32 default_index_key[4] = {0x02b09eb9, 0x0581696f, 0x289b9863, 0x001a1879};
    UINT32 default_pack_key[4]  = {0x04b4b822, 0x1f6eb264, 0x0018eaae, 0x1cfbf6a6};

    memcpy(g_index_key, default_index_key, 16);
    memcpy(g_pack_key , default_pack_key , 16);
}

void Settings::UpdateExtensions()
{
    ui->eIndexExt->setText(g_index_ext);
    ui->ePackExt->setText(g_pack_ext);
}

void Settings::ResetExtensions()
{
    memcpy(g_index_ext, ".eix", 5);
    memcpy(g_pack_ext , ".epk" , 5);
}

void Settings::on_buttonBox_clicked(QAbstractButton *button)
{
    if(button->text().toLower() == "save")
    {
        this->SaveSettings();

        this->UpdateExtensions();
        this->UpdateKeys();
    }
    else if(button->text().toLower() == "restore defaults")
    {
        this->ResetKeys();
        this->UpdateKeys();

        this->ResetExtensions();
        this->UpdateExtensions();

        ui->cStorageType->setCurrentIndex(1);
        g_storage_type = LZO_COMPRESSION_NO_ENCODING;

        ui->cAds->setChecked(g_show_ads = true);
    }
    else if(button->text().toLower() == "close")
    {
        this->close();
    }
}

void Settings::on_pushButton_clicked()
{
    this->ResetKeys();
    this->UpdateKeys();
}

void Settings::on_pushButton_7_clicked()
{
    this->ResetExtensions();
    this->UpdateExtensions();
}
