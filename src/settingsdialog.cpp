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

    QObject::connect(ui->listWidgetSettings, &QListWidget::currentRowChanged,
                     this, &SettingsDialog::settingCategoryChanged);

    this->setupSettingsList();

    QObject::connect(ui->buttonBox, SIGNAL(clicked(QAbstractButton *)), this,
                     SLOT(buttonBoxClicked(QAbstractButton *)));

    QObject::connect(ui->pushButtonSqlitePath, &QPushButton::clicked, [this]() {
        QFileInfo sqlDBFile(ui->lineEditSqlitePath->text());
        QString sqlFile = QFileDialog::getSaveFileName(
            this, "Choose SQLite Database file", sqlDBFile.absolutePath(),
            "SQLite DB (*.sqlite)");
        if (sqlFile != "") {
            ui->lineEditSqlitePath->setText(sqlFile);
        }
    });

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
    ui->listWidgetSettings->addItem(tr("Plot"));
    ui->listWidgetSettings->addItem(tr("Record"));
    this->lastItem = ui->listWidgetSettings->item(0);
    ui->listWidgetSettings->setCurrentRow(0);
}

bool SettingsDialog::checkSettingsChanged(QListWidgetItem *lastItem)
{
    QSettings settings;
    bool somethingChanged = false;
    bool returnVal = false;

    QMessageBox msgBox;
    msgBox.setText("The settings have been modified.");
    msgBox.setInformativeText("Do you want to save your changes?");
    msgBox.setStandardButtons(QMessageBox::Save | QMessageBox::Discard |
                              QMessageBox::Cancel);
    msgBox.setDefaultButton(QMessageBox::Save);
    switch (ui->listWidgetSettings->row(lastItem)) {
    case 0: // general section
        settings.beginGroup(setcon::GENERAL_GROUP);
        if (settings.contains(setcon::GENERAL_DISC)) {
            if (!ui->checkBoxGeneralAskExit->isChecked() ==
                settings.value(setcon::GENERAL_EXIT, QVariant(true)).toBool()) {
                somethingChanged = true;
            }
            if (!ui->checkBoxGeneralAskBeforeDis->isChecked() ==
                settings.value(setcon::GENERAL_DISC, QVariant(false)).toBool()) {
                somethingChanged = true;
            }
        }
        break;
    case 1:
        settings.beginGroup(setcon::DEVICE_GROUP);
        if (settings.contains(setcon::DEVICE_PORT)) {
            // FIXME: Really strange things happen here. Have to store the values
            // from gui and config in local variables, otherwise the following
            // checks all evaluate to false.
            int protGui = ui->comboBoxDeviceProtocoll->currentIndex();
            int protSet = settings.value(setcon::DEVICE_PROTOCOL).toInt();
            if (protGui != protSet) {
                somethingChanged = true;
                break;
            }
            QString guiPort = ui->comboBoxDeviceComPort->currentText();
            QString setPort = settings.value(setcon::DEVICE_PORT).toString();
            if (guiPort != setPort) {
                somethingChanged = true;
                break;
            }
            int chanGui = ui->spinBoxDeviceChannels->value();
            int chanSet = settings.value(setcon::DEVICE_CHANNELS).toInt();
            if (chanGui != chanSet) {
                somethingChanged = true;
                break;
            }
            double voltMinGui = ui->doubleSpinBoxDeviceVoltageMin->value();
            double voltMinSet =
                settings.value(setcon::DEVICE_VOLTAGE_MIN).toDouble();
            if (voltMinGui != voltMinSet) {
                somethingChanged = true;
                break;
            }
            double voltMaxGui = ui->doubleSpinBoxdeviceVoltageMax->value();
            double voltMaxSet =
                settings.value(setcon::DEVICE_VOLTAGE_MAX).toDouble();
            if (voltMaxGui != voltMaxSet) {
                somethingChanged = true;
                break;
            }
            int voltAGui = ui->comboBoxDeviceVoltageAccu->currentIndex();
            int voltASet =
                settings.value(setcon::DEVICE_VOLTAGE_ACCURACY).toInt();
            if (voltAGui != voltASet) {
                somethingChanged = true;
                break;
            }
            double curMinGui = ui->doubleSpinBoxDeviceCurrentMin->value();
            double curMinSet =
                settings.value(setcon::DEVICE_CURRENT_MIN).toDouble();
            if (curMinGui != curMinSet) {
                somethingChanged = true;
                break;
            }
            double curMaxGui = ui->doubleSpinBoxDeviceCurrentMax->value();
            double curMaxSet =
                settings.value(setcon::DEVICE_CURRENT_MAX).toDouble();
            if (curMaxGui != curMaxSet) {
                somethingChanged = true;
                break;
            }
            int curAGui = ui->comboBoxDeviceCurrentAccu->currentIndex();
            int curASet =
                settings.value(setcon::DEVICE_CURRENT_ACCURACY).toInt();
            if (curAGui != curASet) {
                somethingChanged = true;
                break;
            }
        }
        break;
    case 2:
        break;
    case 3:
        settings.beginGroup(setcon::RECORD_GROUP);
        if (settings.contains(setcon::RECORD_SQLPATH)) {
            if (ui->lineEditSqlitePath->text() !=
                settings.value(setcon::RECORD_SQLPATH).toString()) {
                somethingChanged = true;
            }
            if (ui->lineEditRecordTablePrefix->text() !=
                settings.value(setcon::RECORD_TBLPRE).toString()) {
                somethingChanged = true;
            }
        }
        break;
    }

    if (somethingChanged) {
        msgBox.exec();
        if (msgBox.result() == QMessageBox::StandardButton::Save) {
            this->saveSettings(ui->listWidgetSettings->row(lastItem));
        }
        if (msgBox.result() == QMessageBox::StandardButton::Cancel) {
            returnVal = true;
        }
    }

    return returnVal;
}

void SettingsDialog::saveSettings(int currentRow)
{
    QSettings settings;
    if (currentRow == 0) {
        settings.beginGroup(setcon::GENERAL_GROUP);
        settings.setValue(setcon::GENERAL_EXIT,
                          ui->checkBoxGeneralAskExit->isChecked());
        settings.setValue(setcon::GENERAL_DISC,
                          ui->checkBoxGeneralAskBeforeDis->isChecked());
    }
    if (currentRow == 1) {
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
    if (currentRow == 2) {
    };
    if (currentRow == 3) {
        settings.beginGroup(setcon::RECORD_GROUP);
        settings.setValue(setcon::RECORD_SQLPATH,
                          ui->lineEditSqlitePath->text());
        settings.setValue(setcon::RECORD_TBLPRE,
                          ui->lineEditRecordTablePrefix->text());
    }
}

void SettingsDialog::settingCategoryChanged(int currentRow)
{
    if (this->checkSettingsChanged(this->lastItem)) {
        // disconnect signal or this slot gets called again.
        QObject::disconnect(ui->listWidgetSettings,
                            &QListWidget::currentRowChanged, this,
                            &SettingsDialog::settingCategoryChanged);
        ui->listWidgetSettings->setCurrentItem(this->lastItem,
                                               QItemSelectionModel::Select);
        ui->listWidgetSettings->currentItem()->setSelected(true);
        QObject::connect(ui->listWidgetSettings, &QListWidget::currentRowChanged,
                         this, &SettingsDialog::settingCategoryChanged);
        return;
    }

    this->lastItem = ui->listWidgetSettings->currentItem();

    QSettings settings;
    switch (currentRow) {
    case 0: // general section
        ui->stackedWidget->setCurrentIndex(0);
        ui->buttonBox->button(QDialogButtonBox::Apply)->setEnabled(true);
        settings.beginGroup(setcon::GENERAL_GROUP);
        ui->checkBoxGeneralAskExit->setChecked(
            settings.value(setcon::GENERAL_EXIT, QVariant(true)).toBool());
        ui->checkBoxGeneralAskBeforeDis->setChecked(
            settings.value(setcon::GENERAL_DISC, QVariant(false)).toBool());
        break;
    case 1: // device section
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
    case 2: // graph section
        ui->stackedWidget->setCurrentIndex(2);
        ui->buttonBox->button(QDialogButtonBox::Apply)->setEnabled(true);
        break;
    case 3:
        ui->stackedWidget->setCurrentIndex(3);
        ui->buttonBox->button(QDialogButtonBox::Apply)->setEnabled(true);
        {
            settings.beginGroup(setcon::RECORD_GROUP);
            QString defaultSqlFile =
                QStandardPaths::writableLocation(QStandardPaths::DataLocation) +
                QDir::separator() + "labpowerqt.sqlite";
            ui->lineEditSqlitePath->setText(
                settings.value(setcon::RECORD_SQLPATH, defaultSqlFile)
                    .toString());
            ui->lineEditRecordTablePrefix->setText(
                settings.value(setcon::RECORD_TBLPRE).toString());
            break;
        }
    default:
        break;
    }
}

void SettingsDialog::buttonBoxClicked(QAbstractButton *button)
{
    switch (ui->buttonBox->buttonRole(button)) {
    case QDialogButtonBox::ApplyRole:
        this->saveSettings(ui->stackedWidget->currentIndex());
        break;
    case QDialogButtonBox::AcceptRole:
        for(int i = 0; i <=3 ;i++) {
            this->saveSettings(i);
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
