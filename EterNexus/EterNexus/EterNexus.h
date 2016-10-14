#ifndef ETERNEXUS_H
#define ETERNEXUS_H

#include <QMainWindow>
#include <QFileDialog>
#include <QFile>
#include <QWebView>
#include <QLayout>
#include <QDesktopServices>

#define WINVER 0x0500
#include <Windows.h>
//#include <Wininet.h>
#include <direct.h>

#include "API/EterPackAPI.hpp"
#include "MultiThread.h"
#include "Global/Global.h"
#include "UI/Settings.h"
#include "UI/About.h"

extern bool s_processByArg;
extern char **s_argv;
extern INT32 s_argc;

namespace Ui {
class EterNexus;
}

class EterNexus : public QMainWindow
{
    Q_OBJECT
    
public:
    explicit EterNexus(QWidget *parent = 0);
    ~EterNexus();

    void RoutineIndexes();
    
private slots:
    void LinkHandler(QUrl url);
    void UpdateAds(const char *szUpdateMode, const char *szFilename = NULL);

    void OpenSettings();
    void OpenAbout();

    void ExtractArchive();
    void ElaborateIndexes();
    void RoutinePacks();

    void PackArchive();
    void RoutinePackArchive();

private:
    Ui::EterNexus *ui;
};

#endif // ETERNEXUS_H
