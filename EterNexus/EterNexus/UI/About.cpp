#include "About.h"
#include "ui_About.h"

#include "Global/Version.h"
#include "Global/Global.h"

#include <stdio.h>

About::About(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::About)
{
    ui->setupUi(this);

    this->setFixedSize(this->width(), this->height());
    this->setWindowFlags(Qt::WindowSystemMenuHint);

    char _about[200];
    snprintf(_about, 200, "EterNexus Version:%s\r\n\r\nWebsite: %s", g_version, g_website);

    ui->label->setText(_about);
}

About::~About()
{
    delete ui;
}
