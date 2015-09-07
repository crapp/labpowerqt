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

    // dynamically set width for listWidget
    ui->listWidgetSettings->setMinimumWidth(
        ui->listWidgetSettings->sizeHintForColumn(0));

    QObject::connect(
        ui->listWidgetSettings,
        SIGNAL(currentItemChanged(QListWidgetItem *, QListWidgetItem *)), this,
        SLOT(settingChanged(QListWidgetItem *, QListWidgetItem *)));

    this->setupSettingsList();

    QObject::connect(ui->buttonBox, SIGNAL(clicked(QAbstractButton *)), this,
                     SLOT(buttonBoxClicked(QAbstractButton *)));
    QObject::connect(ui->pushButtonDeviceTestConnection, SIGNAL(clicked()), this,
                     SLOT(testClicked()));

    for (const QSerialPortInfo &port : QSerialPortInfo::availablePorts()) {
        ui->comboBoxDeviceComPort->addItem(port.portName());
        qDebug() << "Info: " << port.portName();
        qDebug() << "Desc " << port.description();
        qDebug() << "Manu: " << port.manufacturer();
    }

    this->deviceConnected = false;
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
    QSettings settings;
    switch (ui->listWidgetSettings->row(current)) {
    case 0:
        ui->stackedWidget->setCurrentIndex(0);
        ui->buttonBox->button(QDialogButtonBox::Apply)->setEnabled(true);
        break;
    case 1:
        ui->stackedWidget->setCurrentIndex(1);
        settings.beginGroup(setcon::DEVICE_GROUP);
        if (settings.contains(setcon::DEVICE_PORT)) {
            ui->comboBoxDeviceProtocoll->setCurrentIndex(
                settings.value(setcon::DEVICE_PROTOCOL).toInt());

            int indexPort = ui->comboBoxDeviceComPort->findText(
                settings.value(setcon::DEVICE_PROTOCOL).toString());
            if (indexPort != -1) {
                ui->comboBoxDeviceComPort->setCurrentIndex(indexPort);
            }

            ui->spinBoxDeviceChannels->setValue(
                settings.value(setcon::DEVICE_CHANNELS).toInt());

            ui->doubleSpinBoxDeviceVoltageMin->setValue(
                settings.value(setcon::DEVICE_VOLTAGE_MIN).toDouble());
            ui->doubleSpinBoxdeviceVoltageMax->setValue(
                settings.value(setcon::DEVICE_VOLTAGE_MAX).toDouble());
            ui->comboBoxDeviceVoltageAccu->setCurrentIndex(
                settings.value(setcon::DEVICE_VOLTAGE_ACCURACY).toInt());

            ui->doubleSpinBoxDeviceCurrentMin->setValue(
                settings.value(setcon::DEVICE_CURRENT_MIN).toDouble());
            ui->doubleSpinBoxDeviceCurrentMax->setValue(
                settings.value(setcon::DEVICE_CURRENT_MAX).toDouble());
            ui->comboBoxDeviceCurrentAccu->setCurrentIndex(
                settings.value(setcon::DEVICE_CURRENT_ACCURACY).toInt());
        }

        ui->buttonBox->button(QDialogButtonBox::Apply)->setEnabled(false);
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
                              ui->comboBoxDeviceProtocoll->currentIndex());
            // TODO: Add a text widget to specify a device Name
            // settings.setValue(setcon::DEVICE_NAME, "Foo");
            // DeviceCurrentMax
            settings.setValue(setcon::DEVICE_CHANNELS,
                              ui->spinBoxDeviceChannels->value());
            settings.setValue(setcon::DEVICE_CURRENT_MIN,
                              ui->doubleSpinBoxDeviceCurrentMin->value());
            settings.setValue(setcon::DEVICE_CURRENT_MAX,
                              ui->doubleSpinBoxDeviceCurrentMax->value());
            settings.setValue(setcon::DEVICE_CURRENT_ACCURACY,
                              ui->comboBoxDeviceCurrentAccu->currentIndex());

            settings.setValue(setcon::DEVICE_VOLTAGE_MIN,
                              ui->doubleSpinBoxDeviceVoltageMin->value());
            settings.setValue(setcon::DEVICE_VOLTAGE_MAX,
                              ui->doubleSpinBoxdeviceVoltageMax->value());
            settings.setValue(setcon::DEVICE_VOLTAGE_ACCURACY,
                              ui->comboBoxDeviceVoltageAccu->currentIndex());
        }
        break;
    default:
        break;
    }
}

void SettingsDialog::testClicked()
{
    ui->plainTextEditDeviceTest->setPlainText("");
    ui->scrollArea->ensureWidgetVisible(ui->plainTextEditDeviceTest);
    QString statustext = "Connecting to device on port " +
                         ui->comboBoxDeviceComPort->currentText();
    ui->plainTextEditDeviceTest->appendPlainText(statustext);

    if (ui->comboBoxDeviceProtocoll->currentText() == "Korad SCPI V2") {
        this->powerSupplyConnector = std::unique_ptr<KoradSCPI>(
            new KoradSCPI(ui->comboBoxDeviceComPort->currentText(),
                          ui->spinBoxDeviceChannels->value(),
                          ui->comboBoxDeviceVoltageAccu->currentIndex(),
                          ui->comboBoxDeviceCurrentAccu->currentIndex()));
    }

    ui->plainTextEditDeviceTest->appendPlainText(
        "Using " + ui->comboBoxDeviceProtocoll->currentText() + " protocoll");

    QObject::connect(this->powerSupplyConnector.get(),
                     SIGNAL(requestFinished(std::shared_ptr<SerialCommand>)),
                     this,
                     SLOT(deviceIdentified(std::shared_ptr<SerialCommand>)));
    QObject::connect(this->powerSupplyConnector.get(),
                     SIGNAL(errorOpen(QString)), this,
                     SLOT(deviceOpenError(QString)));
    this->powerSupplyConnector->startPowerSupplyBackgroundThread();
    this->powerSupplyConnector->getIdentification();
}

void SettingsDialog::deviceIdentified(std::shared_ptr<SerialCommand> command)
{
    if (static_cast<PowerSupplySCPI_constants::COMMANDS>(
            command->getCommand()) ==
        PowerSupplySCPI_constants::COMMANDS::GETIDN) {
        QString idString = command->getValue().toString();
        if (idString == "") {
            QString statustext = "Connection successfull but Device send back "
                                 "an empty identification String. Check the "
                                 "chosen protocoll and port.";
            ui->plainTextEditDeviceTest->appendPlainText(statustext);
        } else {
            QString statustext = "Connection successfull";
            ui->plainTextEditDeviceTest->appendPlainText(statustext);
            statustext = "Device identified as " + idString;
            ui->plainTextEditDeviceTest->appendPlainText(statustext);
            ui->buttonBox->button(QDialogButtonBox::Apply)->setEnabled(true);
        }
    }
}

void SettingsDialog::deviceOpenError(QString errorString)
{
    QString statustext = "Failed to open device.\nError message: " + errorString;
    ui->plainTextEditDeviceTest->appendPlainText(statustext);
    ui->plainTextEditDeviceTest->appendPlainText(
        "Ensure your device is powered on and you have chosen the right "
        "communications "
        "protocoll as well as the correct device port.");
}

void SettingsDialog::accept()
{
    this->powerSupplyConnector.reset(nullptr);
    this->done(1);
}

void SettingsDialog::reject()
{
    this->powerSupplyConnector.reset(nullptr);
    this->done(0);
}
