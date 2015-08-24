// labpowerqt is a Gui application to control programmable lab power supplies
// Copyright © 2015 Christian Rapp <0x2a at posteo dot org>

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

namespace setcon = settings_constants;

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

    QObject::connect(ui->buttonBox, SIGNAL(clicked(QAbstractButton *)), this,
                     SLOT(buttonBoxClicked(QAbstractButton *)));

    for (const QSerialPortInfo &port : QSerialPortInfo::availablePorts()) {
        ui->comboBoxDeviceComPort->addItem(port.portName());
        qDebug() << "Info: " << port.portName();
        qDebug() << "Desc " << port.description();
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

void SettingsDialog::buttonBoxClicked(QAbstractButton *button)
{
    QSettings settings;
    switch (ui->buttonBox->buttonRole(button)) {
    case QDialogButtonBox::ApplyRole:
        if (ui->stackedWidget->currentIndex() == 1) {
            settings.beginGroup(setcon::DEVICE_GROUP);
            settings.setValue(setcon::DEVICE_PORT,
                              ui->comboBoxDeviceComPort->currentText());
            settings.setValue(setcon::DEVICE_PROTOCOL,
                              ui->comboBoxDeviceProtocoll->currentText());
            // TODO: Add a text widget to specify a device Name
            // settings.setValue(setcon::DEVICE_NAME, "Foo");
            settings.setValue(setcon::DEVICE_CHANNELS,
                              ui->spinBoxDeviceChannels->value());
            settings.setValue(setcon::DEVICE_CURRENT_MIN,
                              ui->spinBoxDeviceCurrentMin->value());
            settings.setValue(setcon::DEVICE_CURRENT_MAX,
                              ui->spinBoxDeviceCurrentMax->value());
            settings.setValue(setcon::DEVICE_CURRENT_ACCURACY,
                              ui->comboBoxDeviceCurrentAccu->currentText());
            settings.setValue(setcon::DEVICE_VOLTAGE_MIN,
                              ui->spinBoxDeviceVoltageMin->value());
            settings.setValue(setcon::DEVICE_VOLTAGE_MAX,
                              ui->spinBoxdeviceVoltageMax->value());
            settings.setValue(setcon::DEVICE_VOLTAGE_ACCURACY,
                              ui->comboBoxDeviceVoltageAccu->currentText());
        }
        break;
    default:
        break;
    }
}
