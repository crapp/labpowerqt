#include "settingsdialog.h"
#include "ui_settingsdialog.h"

SettingsDialog::SettingsDialog(QWidget *parent)
    : QDialog(parent), ui(new Ui::SettingsDialog)
{
    ui->setupUi(this);
    ui->gridLayout_2->setColumnStretch(1, 100);
}

SettingsDialog::~SettingsDialog() { delete ui; }
