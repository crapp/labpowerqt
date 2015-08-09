#include "settingsdialog.h"
#include "ui_settingsdialog.h"

SettingsDialog::SettingsDialog(QWidget *parent)
    : QDialog(parent), ui(new Ui::SettingsDialog)
{
    ui->setupUi(this);
    ui->gridLayout_2->setColumnStretch(1, 100);

    ui->listWidgetSettings->setMinimumWidth(
        ui->listWidgetSettings->sizeHintForColumn(0));

    QObject::connect(
        ui->listWidgetSettings,
        SIGNAL(currentItemChanged(QListWidgetItem *, QListWidgetItem *)), this,
        SLOT(settingChanged(QListWidgetItem *, QListWidgetItem *)));

    this->setupSettingsList();

    for(const QSerialPortInfo &port : QSerialPortInfo::availablePorts()) {
        ui->comboBoxDeviceComPort->addItem(port.portName());
        qDebug() << "Info: " << port.portName();
        qDebug() << "Desc "  << port.description();
        qDebug() << "Manu: " << port.manufacturer();
    }



}

SettingsDialog::~SettingsDialog() { delete ui; }

void SettingsDialog::setupSettingsList()
{
    ui->listWidgetSettings->addItem(tr("General"));
    ui->listWidgetSettings->addItem(tr("Device"));
    ui->listWidgetSettings->setCurrentRow(0);
}

void SettingsDialog::settingChanged(QListWidgetItem *current,
                                    QListWidgetItem *previous)
{
    switch (ui->listWidgetSettings->row(current)) {
    case 0:
        ui->stackedWidget->setCurrentIndex(0);
        break;
    case 1:
        ui->stackedWidget->setCurrentIndex(1);
        break;
    default:
        break;
    }
}
