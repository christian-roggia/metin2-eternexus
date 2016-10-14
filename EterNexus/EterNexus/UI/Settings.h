#ifndef SETTINGS_H
#define SETTINGS_H

#include <QWidget>
#include <QAbstractButton>

#include <assert.h>

#include "Global/Global.h"
#include "Global/IO.h"
#include "Global/Utils.h"

namespace Ui {
class Settings;
}

class Settings : public QWidget
{
    Q_OBJECT
    
public:
    explicit Settings(QWidget *parent = 0);
    ~Settings();

protected:
    void Init();

    void LoadSettings();
    void SaveSettings();

    void UpdateKeys();
    void ResetKeys();

    void UpdateExtensions();
    void ResetExtensions();
    
private slots:
    void on_buttonBox_clicked(QAbstractButton *button);
    void on_pushButton_clicked();

    void on_pushButton_7_clicked();

private:
    Ui::Settings *ui;
};

#endif // SETTINGS_H
