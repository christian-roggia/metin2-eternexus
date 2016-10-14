#include "EterNexus.h"
#include "ui_EterNexus.h"

bool s_processByArg = false;
char **s_argv       = NULL;
INT32 s_argc        = 0;

EterNexus::EterNexus(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::EterNexus)
{
    ui->setupUi(this);

#ifndef ADWARE_VERSION
    if(g_show_ads)
#endif
        this->setFixedSize(410, 260);
#ifndef ADWARE_VERSION
    else
        this->setFixedSize(410, 180);
#endif

    this->setWindowFlags(Qt::WindowSystemMenuHint);

    connect(ui->action_Extract, SIGNAL(triggered()), this, SLOT(ExtractArchive()));
    connect(ui->action_Pack_Archive, SIGNAL(triggered()), this, SLOT(PackArchive()));
    connect(ui->action_Settings, SIGNAL(triggered()), this, SLOT(OpenSettings()));
    connect(ui->action_About, SIGNAL(triggered()), this, SLOT(OpenAbout()));
    connect(ui->action_Exit, SIGNAL(triggered()), this, SLOT(close()));

    this->UpdateAds("STARTUP");

    if(s_processByArg && s_argv && s_argc >= 3 && *s_argv[1] == '-' && strlen(s_argv[1]) > 1)
    {
        switch(s_argv[1][1])
        {
        case 'E':
            this->ExtractArchive();
            break;
        case 'P':
            this->PackArchive();
            break;
        default:
            s_processByArg = false; // Prevent errors
            break;
        }
    }
}

Settings *uiSettings = 0;
About    *uiAbout    = 0;

EterNexus::~EterNexus()
{
    if(uiSettings) delete uiSettings;
    if(uiAbout)    delete uiAbout;

    delete ui;
}

void EterNexus::LinkHandler(QUrl url)
{
    QDesktopServices::openUrl(url);
}

void EterNexus::UpdateAds(const char *szUpdateMode, const char *szFilename)
{
    OSVERSIONINFO _os = {};

    _os.dwOSVersionInfoSize = sizeof(OSVERSIONINFO);
    GetVersionEx(&_os);

    char szWebData[MAX_PATH] = {};
    char szWebInfoData[MAX_PATH] = {};

    _snprintf(szWebData, MAX_PATH, "%s%s/%s/%s/%d/",
              g_protocol,
              g_website,
              g_ads_addr,
              g_version,
              GetUserDefaultUILanguage());
    _snprintf(szWebInfoData, MAX_PATH, "%s%s/%s/%s/%s/%s/%s/%d/%d-%d-%d-%d-%s/",
              g_protocol,
              g_website,
              g_info_addr,
              szUpdateMode,
              g_version,
              "lzo",
              "-",
              GetUserDefaultUILanguage(),
              _os.dwMajorVersion,
              _os.dwMinorVersion,
              _os.dwBuildNumber,
              _os.dwPlatformId,
              _os.szCSDVersion);

    if((!strcmp(szUpdateMode, "FILEOPEN") || !strcmp(szUpdateMode, "FILECREATE")) && szFilename && strlen(szFilename) > 0)
    {
        char szTempURL[MAX_PATH]   = {};
        char szFilename_[MAX_PATH] = {}; // filename without extension

        strcpy(szFilename_, szFilename);

        for(size_t i = 0; i < strlen(szFilename_); i++)
            if(szFilename_[i] == '.') szFilename_[i] = 0;

        _snprintf(szTempURL, MAX_PATH, "%s/%s/%s/", szFilename_, g_index_ext, g_pack_ext);
        strncat(szWebInfoData, szTempURL, MAX_PATH);
    }

    // Get only information about the client
    static QWebView *webView_ = NULL;
    if(webView_ == NULL)
    {
        webView_ = new QWebView(this);

        webView_->setVisible(false);
        webView_->setGeometry(QRect(0, 0, 1, 1));
    }

    webView_->load(QUrl(QString(szWebInfoData)));

    // Stop here if is only information gathering
    if(!strcmp(szUpdateMode, "FILEOPEN") || !strcmp(szUpdateMode, "FILECREATE"))
        return;

#ifndef ADWARE_VERSION
    if(g_show_ads)
    {
#endif
            QWebView *webView = new QWebView(this);

            if(webView != NULL)
            {
                webView->setGeometry(QRect(10, 180, 390, 70));
                webView->load(QUrl(QString(szWebData)));
                webView->show();

                webView->page()->setLinkDelegationPolicy(QWebPage::DelegateAllLinks);
                connect(webView, SIGNAL(linkClicked(QUrl)), this, SLOT(LinkHandler(QUrl)));
                this->layout()->addWidget(webView);
            }

#ifndef ADWARE_VERSION
    }
#endif
}

void EterNexus::OpenSettings()
{
    if(uiSettings) delete uiSettings;
    uiSettings = new Settings();

    uiSettings->show();
}

void EterNexus::OpenAbout()
{
    if(uiAbout) delete uiAbout;
    uiAbout = new About();

    uiAbout->show();
}
