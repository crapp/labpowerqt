// labpowerqt is a Gui application to control programmable lab power supplies
// Copyright © 2015 Christian Rapp <0x2a at posteo.org>

// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.

// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.

// You should have received a copy of the GNU General Public License
// along with this program.  If not, see <http://www.gnu.org/licenses/>.

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
