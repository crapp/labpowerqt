// labpowerqt is a Gui application to control programmable lab power supplies
// Copyright © 2015, 2016 Christian Rapp <0x2a at posteo dot org>

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
namespace dbutil = database_utils;

SettingsDialog::SettingsDialog(QWidget *parent)
    : QDialog(parent), ui(new Ui::SettingsDialog)
{
    ui->setupUi(this);
    ui->gridLayout_2->setColumnStretch(1, 100);

    // dynamically set width for listWidget
    ui->listWidgetSettings->setMinimumWidth(
        ui->listWidgetSettings->sizeHintForColumn(0));

    this->initGeneral();
    this->initDevice();
    this->initPlot();
    this->initRecord();

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

    // update overview label
    QObject::connect(
        ui->comboBoxDeviceActive, &QComboBox::currentTextChanged,
        [this](const QString &currentText) {
            if (ui->comboBoxDeviceActive->count() == 0) {
                ui->labelDeviceOverview->setText("No device selected");
                return;
            }
            QSettings settings;
            settings.beginGroup(setcon::DEVICE_GROUP);
            settings.beginGroup(currentText);
            QString overview = "";
            overview += "Device ID: " +
                        settings.value(setcon::DEVICE_NAME).toString() + "\n";
            overview += "Protocol: " +
                        settings.value(setcon::DEVICE_PROTOCOL).toString() +
                        "\n";
            overview +=
                "Port: " + settings.value(setcon::DEVICE_PORT).toString() + "\n";
            overview +=
                "Voltage Range: " +
                QString::number(
                    settings.value(setcon::DEVICE_VOLTAGE_MIN).toDouble(), 'f',
                    settings.value(setcon::DEVICE_VOLTAGE_ACCURACY).toInt()) +
                " V - " +
                QString::number(
                    settings.value(setcon::DEVICE_VOLTAGE_MAX).toDouble(), 'f',
                    settings.value(setcon::DEVICE_VOLTAGE_ACCURACY).toInt()) +
                " V \n";
            overview +=
                "Current Range: " +
                QString::number(
                    settings.value(setcon::DEVICE_CURRENT_MIN).toDouble(), 'f',
                    settings.value(setcon::DEVICE_CURRENT_ACCURACY).toInt()) +
                " A - " +
                QString::number(
                    settings.value(setcon::DEVICE_CURRENT_MAX).toDouble(), 'f',
                    settings.value(setcon::DEVICE_CURRENT_ACCURACY).toInt()) +
                " A \n";
            ui->labelDeviceOverview->setText(overview);
        });
    // add device
    QObject::connect(ui->pushButtonDeviceAdd, &QPushButton::clicked, [this]() {
        DeviceWizard wiz;
        if (wiz.exec()) {
            this->devicesComboBoxUpdate();
        }
    });
    // remove device
    QObject::connect(
        ui->pushButtonDeviceRemove, &QPushButton::clicked, [this]() {
            QSettings settings;
            settings.beginGroup(setcon::DEVICE_GROUP);
            if (settings.childGroups().contains(
                    ui->comboBoxDeviceActive->currentText())) {
                settings.remove(ui->comboBoxDeviceActive->currentText());
                settings.setValue(setcon::DEVICE_ACTIVE, "");
                this->devicesComboBoxUpdate();
            }
        });
}

SettingsDialog::~SettingsDialog() { delete ui; }

void SettingsDialog::initGeneral()
{
    QSettings settings;
    settings.beginGroup(setcon::GENERAL_GROUP);
    ui->checkBoxGeneralAskExit->setChecked(
        settings.value(setcon::GENERAL_EXIT, QVariant(true)).toBool());
    ui->checkBoxGeneralAskBeforeDis->setChecked(
        settings.value(setcon::GENERAL_DISC, QVariant(false)).toBool());
}

void SettingsDialog::initDevice() { this->devicesComboBoxUpdate(); }

void SettingsDialog::initPlot() {}

void SettingsDialog::initRecord()
{
    QSettings settings;
    settings.beginGroup(setcon::RECORD_GROUP);
    QString defaultSqlFile =
        QStandardPaths::writableLocation(QStandardPaths::DataLocation) +
        QDir::separator() + "labpowerqt.sqlite";
    // ui->checkBoxRecordDefault->setChecked(settings.value(setcon::r))
    ui->lineEditSqlitePath->setText(
        settings.value(setcon::RECORD_SQLPATH, defaultSqlFile).toString());
    ui->lineEditRecordTablePrefix->setText(
        settings.value(setcon::RECORD_TBLPRE).toString());
    ui->spinBoxRecordBuffer->setValue(
        settings.value(setcon::RECORD_BUFFER, 60).toInt());
}

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
        if (ui->comboBoxDeviceActive->currentText() !=
            settings.value(setcon::DEVICE_ACTIVE)) {
            somethingChanged = true;
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
            if (ui->spinBoxRecordBuffer->value() !=
                settings.value(setcon::RECORD_BUFFER).toInt()) {
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
        settings.setValue(setcon::DEVICE_ACTIVE,
                          ui->comboBoxDeviceActive->currentText());
    }
    if (currentRow == 2) {
    };
    if (currentRow == 3) {
        settings.beginGroup(setcon::RECORD_GROUP);
        settings.setValue(setcon::RECORD_SQLPATH,
                          ui->lineEditSqlitePath->text());
        settings.setValue(setcon::RECORD_TBLPRE,
                          ui->lineEditRecordTablePrefix->text());
        QSqlDatabase db = QSqlDatabase::database();
        if (db.databaseName() != ui->lineEditSqlitePath->text()) {
            db.close();
            db.setDatabaseName(ui->lineEditSqlitePath->text());
            if (!db.open()) {
                QMessageBox::warning(this, "Could not open Database",
                                     "Could not open " +
                                         ui->lineEditSqlitePath->text() + "\n" +
                                         db.lastError().text(),
                                     QMessageBox::StandardButton::Ok);
            } else {
                dbutil::setDBOptimizations();
                dbutil::initTables();
            }
        }
        settings.setValue(setcon::RECORD_BUFFER,
                          ui->spinBoxRecordBuffer->value());
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

    switch (currentRow) {
    case 0: // general section
        this->initGeneral();
        ui->stackedWidget->setCurrentIndex(0);
        break;
    case 1: // device section
        this->initDevice();
        ui->stackedWidget->setCurrentIndex(1);
        break;
    case 2: // graph section
        this->initPlot();
        ui->stackedWidget->setCurrentIndex(2);
        break;
    case 3:
        this->initRecord();
        ui->stackedWidget->setCurrentIndex(3);
        break;
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
        for (int i = 0; i <= 3; i++) {
            this->saveSettings(i);
        }
        break;
    default:
        break;
    }
}

void SettingsDialog::devicesComboBoxUpdate()
{
    QSettings settings;
    ui->comboBoxDeviceActive->clear();
    settings.beginGroup(setcon::DEVICE_GROUP);
    ui->comboBoxDeviceActive->addItems(settings.childGroups());
    ui->comboBoxDeviceActive->setCurrentText(
        settings.value(setcon::DEVICE_ACTIVE).toString());
    if (ui->comboBoxDeviceActive->count() > 0) {
        ui->pushButtonDeviceRemove->setEnabled(true);
    } else {
        ui->pushButtonDeviceRemove->setEnabled(false);
    }
}

void SettingsDialog::accept() { this->done(1); }

void SettingsDialog::reject() { this->done(0); }
