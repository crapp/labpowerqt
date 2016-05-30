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
namespace setdef = settings_default;
namespace dbutil = database_utils;

SettingsDialog::SettingsDialog(QWidget *parent)
    : QDialog(parent), ui(new Ui::SettingsDialog)
{
    ui->setupUi(this);
    ui->gridLayout_2->setColumnStretch(1, 100);

    // dynamically set width for listWidget
    ui->listWidgetSettings->setMinimumWidth(
        ui->listWidgetSettings->sizeHintForColumn(0));

    // TODO: DataLocation is deprecated but the newer constant AppDataLocation is
    // provided from 5.4 onwards
    if (!QDir(QStandardPaths::writableLocation(QStandardPaths::DataLocation))
             .exists()) {
        // TODO: Do I have to check the return value of QDir::mkpath?
        QDir().mkpath(
            QStandardPaths::writableLocation(QStandardPaths::DataLocation));
    }
    this->defaultSqlFile =
        QStandardPaths::writableLocation(QStandardPaths::DataLocation) +
        QDir::separator() + "labpowerqt.sqlite";
    this->defaultLogDir =
        QStandardPaths::writableLocation(QStandardPaths::CacheLocation);

    this->initGeneral();
    this->initDevice();
    this->initPlot();
    this->initRecord();
    this->initLog();

    QObject::connect(ui->listWidgetSettings, &QListWidget::currentRowChanged,
                     this, &SettingsDialog::settingCategoryChanged);

    this->setupSettingsList();

    QObject::connect(ui->buttonBox, SIGNAL(clicked(QAbstractButton *)), this,
                     SLOT(buttonBoxClicked(QAbstractButton *)));

    // select database file
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
            overview += "Poll frequency: " +
                        settings.value(setcon::DEVICE_POLL_FREQ).toString() +
                        "ms" + "\n";
            overview += "Timeout: " +
                        settings.value(setcon::DEVICE_PORT_TIMEOUT).toString() +
                        "ms" + "\n";
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
    // sync zoom values
    QObject::connect(
        ui->spinBoxPlotZoomMin,
        static_cast<void (QSpinBox::*)(int)>(&QSpinBox::valueChanged), this,
        &SettingsDialog::zoomMinMaxSync);
    QObject::connect(
        ui->spinBoxPlotZoomMax,
        static_cast<void (QSpinBox::*)(int)>(&QSpinBox::valueChanged), this,
        &SettingsDialog::zoomMinMaxSync);

    // select log files directory
    QObject::connect(
        ui->pushButtonLogDirectory, &QPushButton::clicked, [this]() {
            QString logfile_dir = ui->lineEditLogDirectory->text();
            QString new_dir = QFileDialog::getExistingDirectory(
                this, "Choose a location for the log files", logfile_dir,
                QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks);
            if (new_dir != "") {
                ui->lineEditLogDirectory->setText(new_dir);
            }
        });

    QSettings settings;
    settings.beginGroup(setcon::GENERAL_GROUP);
    if (!settings.value(
                     setcon::GENERAL_INFO_SETTINGS,
                     setdef::general_defaults.at(setcon::GENERAL_INFO_SETTINGS))
             .toBool()) {
        // using a qtimer here is very useful. The Timer will fire as soon as
        // the event queue is processed and the GUI is visible.
        QTimer::singleShot(400, this, []() {
            QCheckBox *msgCB = new QCheckBox();
            msgCB->setText("Don't show this message again");
            msgCB->setChecked(false);
            QMessageBox box;
            box.setIcon(QMessageBox::Icon::Information);
            box.setStandardButtons(QMessageBox::StandardButton::Ok);
            box.setDefaultButton(QMessageBox::StandardButton::Ok);
            box.setCheckBox(msgCB);
            box.setWindowTitle("LabPowerQt Settings");
            box.setTextFormat(Qt::RichText);
            box.setText("Configure LabPowerQt as you wish and add devices");
            box.setInformativeText(
                "<p>You have to add at least one Device using the Device Wizard "
                "which is located in the Device section</p>"
                "<p>In the Record section you can set the path for the SQLite "
                "Database file LabPowerQt uses to store the recorded data.</p>"
                "<p>If you have problems with the application please activate "
                "the logging mechanism that will write log messages to log "
                "files.</p>");
            box.exec();
            if (msgCB->isChecked()) {
                QSettings settings;
                settings.beginGroup(setcon::GENERAL_GROUP);
                settings.setValue(setcon::GENERAL_INFO_SETTINGS, true);
            }
        });
    }
}

SettingsDialog::~SettingsDialog() { delete ui; }
void SettingsDialog::initGeneral()
{
    QSettings settings;
    settings.beginGroup(setcon::GENERAL_GROUP);
    ui->checkBoxGeneralAskExit->setChecked(
        settings.value(setcon::GENERAL_ASK_EXIT,
                       setdef::general_defaults.at(setcon::GENERAL_ASK_EXIT))
            .toBool());
    ui->checkBoxGeneralAskBeforeDis->setChecked(
        settings.value(setcon::GENERAL_ASK_DISC,
                       setdef::general_defaults.at(setcon::GENERAL_ASK_DISC))
            .toBool());
}

void SettingsDialog::initDevice() { this->devicesComboBoxUpdate(); }
void SettingsDialog::initPlot()
{
    QSettings settings;
    settings.beginGroup(setcon::PLOT_GROUP);
    ui->spinBoxPlotZoomMin->setValue(
        settings.value(setcon::PLOT_ZOOM_MIN,
                       setdef::general_defaults.at(setcon::PLOT_ZOOM_MIN))
            .toInt());
    ui->spinBoxPlotZoomMax->setValue(
        settings.value(setcon::PLOT_ZOOM_MAX,
                       setdef::general_defaults.at(setcon::PLOT_ZOOM_MAX))
            .toInt());
}
void SettingsDialog::initRecord()
{
    QSettings settings;
    settings.beginGroup(setcon::RECORD_GROUP);
    ui->lineEditSqlitePath->setText(
        settings.value(setcon::RECORD_SQLPATH, this->defaultSqlFile).toString());
    ui->lineEditRecordTablePrefix->setText(
        settings.value(setcon::RECORD_TBLPRE, "").toString());
    ui->spinBoxRecordBuffer->setValue(
        settings.value(setcon::RECORD_BUFFER,
                       setdef::general_defaults.at(setcon::RECORD_BUFFER))
            .toInt());
}

void SettingsDialog::initLog()
{
    QSettings settings;
    settings.beginGroup(setcon::LOG_GROUP);
    ui->checkBoxLogEnabled->setChecked(
        settings.value(setcon::LOG_ENABLED,
                       setdef::general_defaults.at(setcon::LOG_ENABLED))
            .toBool());
    ui->comboBoxLogLoglevel->setCurrentIndex(
        settings.value(setcon::LOG_MIN_SEVERITY,
                       setdef::general_defaults.at(setcon::LOG_MIN_SEVERITY))
            .toInt());
    ui->lineEditLogDirectory->setText(
        settings.value(setcon::LOG_DIRECTORY, this->defaultLogDir).toString());
    ui->checkBoxLogFlush->setChecked(
        settings.value(setcon::LOG_FLUSH,
                       setdef::general_defaults.at(setcon::LOG_FLUSH))
            .toBool());
}

void SettingsDialog::setupSettingsList()
{
    ui->listWidgetSettings->addItem(tr("General"));
    ui->listWidgetSettings->addItem(tr("Device"));
    ui->listWidgetSettings->addItem(tr("Plot"));
    ui->listWidgetSettings->addItem(tr("Record"));
    ui->listWidgetSettings->addItem(tr("Logging"));
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
    case 0:  // general section
        settings.beginGroup(setcon::GENERAL_GROUP);
        if (settings.contains(setcon::GENERAL_ASK_DISC)) {
            if (!ui->checkBoxGeneralAskExit->isChecked() ==
                settings.value(setcon::GENERAL_ASK_EXIT,
                               setdef::general_defaults.at(
                                   setcon::GENERAL_ASK_EXIT))
                    .toBool()) {
                somethingChanged = true;
            }
            if (!ui->checkBoxGeneralAskBeforeDis->isChecked() ==
                settings.value(setcon::GENERAL_ASK_DISC,
                               setdef::general_defaults.at(
                                   setcon::GENERAL_ASK_DISC))
                    .toBool()) {
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
        settings.beginGroup(setcon::PLOT_GROUP);
        if (ui->spinBoxPlotZoomMin->value() !=
            settings.value(setcon::PLOT_ZOOM_MIN,
                           setdef::general_defaults.at(setcon::PLOT_ZOOM_MIN))
                .toInt()) {
            somethingChanged = true;
        }
        if (ui->spinBoxPlotZoomMax->value() !=
            settings.value(setcon::PLOT_ZOOM_MAX,
                           setdef::general_defaults.at(setcon::PLOT_ZOOM_MAX))
                .toInt()) {
            somethingChanged = true;
        }
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
                settings.value(
                            setcon::RECORD_BUFFER,
                            setdef::general_defaults.at(setcon::RECORD_BUFFER))
                    .toInt()) {
                somethingChanged = true;
            }
        }
        break;
    case 4:
        settings.beginGroup(setcon::LOG_GROUP);
        if (ui->checkBoxLogEnabled->isChecked() !=
            settings.value(setcon::LOG_ENABLED,
                           setdef::general_defaults.at(setcon::LOG_ENABLED))
                .toBool()) {
            somethingChanged = true;
        }
        if (ui->comboBoxLogLoglevel->currentIndex() !=
            settings.value(setcon::LOG_MIN_SEVERITY,
                           setdef::general_defaults.at(setcon::LOG_MIN_SEVERITY))
                .toInt()) {
            somethingChanged = true;
        }
        if (ui->lineEditLogDirectory->text() !=
            settings.value(setcon::LOG_DIRECTORY, this->defaultLogDir)
                .toString()) {
            somethingChanged = true;
        }
        if (ui->checkBoxLogFlush->isChecked() !=
            settings.value(setcon::LOG_FLUSH,
                           setdef::general_defaults.at(setcon::LOG_FLUSH))
                .toBool()) {
            somethingChanged = true;
        };
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
        settings.setValue(setcon::GENERAL_ASK_EXIT,
                          ui->checkBoxGeneralAskExit->isChecked());
        settings.setValue(setcon::GENERAL_ASK_DISC,
                          ui->checkBoxGeneralAskBeforeDis->isChecked());
    }
    if (currentRow == 1) {
        settings.beginGroup(setcon::DEVICE_GROUP);
        settings.setValue(setcon::DEVICE_ACTIVE,
                          ui->comboBoxDeviceActive->currentText());
    }
    if (currentRow == 2) {
        settings.beginGroup(setcon::PLOT_GROUP);
        settings.setValue(setcon::PLOT_ZOOM_MIN,
                          ui->spinBoxPlotZoomMin->value());
        settings.setValue(setcon::PLOT_ZOOM_MAX,
                          ui->spinBoxPlotZoomMax->value());
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

    if (currentRow == 4) {
        settings.beginGroup(setcon::LOG_GROUP);
        settings.setValue(setcon::LOG_ENABLED,
                          ui->checkBoxLogEnabled->isChecked());
        settings.setValue(setcon::LOG_DIRECTORY,
                          ui->lineEditLogDirectory->text());
        settings.setValue(setcon::LOG_MIN_SEVERITY,
                          ui->comboBoxLogLoglevel->currentIndex());
        settings.setValue(setcon::LOG_FLUSH, ui->checkBoxLogFlush->isChecked());
    }
}

void SettingsDialog::restoreSettings(int currentRow)
{
    if (currentRow == 0) {
        ui->checkBoxGeneralAskExit->setChecked(
            setdef::general_defaults.at(setcon::GENERAL_ASK_EXIT).toBool());
        ui->checkBoxGeneralAskBeforeDis->setChecked(
            setdef::general_defaults.at(setcon::GENERAL_ASK_DISC).toBool());
    }
    if (currentRow == 1) {
        // nothing to reset here
    }
    if (currentRow == 2) {
        ui->spinBoxPlotZoomMin->setValue(
            setdef::general_defaults.at(setcon::PLOT_ZOOM_MIN).toInt());
        ui->spinBoxPlotZoomMax->setValue(
            setdef::general_defaults.at(setcon::PLOT_ZOOM_MAX).toInt());
    }
    if (currentRow == 3) {
        ui->lineEditSqlitePath->setText(this->defaultSqlFile);
        ui->lineEditRecordTablePrefix->setText("");
        ui->spinBoxRecordBuffer->setValue(
            setdef::general_defaults.at(setcon::RECORD_BUFFER).toInt());
    }
    if (currentRow == 4) {
        ui->checkBoxLogEnabled->setChecked(
            setdef::general_defaults.at(setcon::LOG_ENABLED).toBool());
        ui->lineEditLogDirectory->setText(this->defaultLogDir);
        ui->comboBoxLogLoglevel->setCurrentIndex(
            setdef::general_defaults.at(setcon::LOG_MIN_SEVERITY).toInt());
        ui->checkBoxLogFlush->setChecked(
            setdef::general_defaults.at(setcon::LOG_FLUSH).toBool());
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
    case 0:  // general section
        this->initGeneral();
        ui->stackedWidget->setCurrentIndex(0);
        break;
    case 1:  // device section
        this->initDevice();
        ui->stackedWidget->setCurrentIndex(1);
        break;
    case 2:  // graph section
        this->initPlot();
        ui->stackedWidget->setCurrentIndex(2);
        break;
    case 3:
        this->initRecord();
        ui->stackedWidget->setCurrentIndex(3);
        break;
    case 4:
        this->initLog();
        ui->stackedWidget->setCurrentIndex(4);
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
    case QDialogButtonBox::ButtonRole::ResetRole:
        this->restoreSettings(ui->stackedWidget->currentIndex());
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

void SettingsDialog::zoomMinMaxSync(int value)
{
    int zoom_min = ui->spinBoxPlotZoomMin->value();
    int zoom_max = ui->spinBoxPlotZoomMax->value();
    if (zoom_max - zoom_min < 110) {
        if (value == zoom_min) {
            ui->spinBoxPlotZoomMax->setValue(zoom_min + 110);
        }
        if (value == zoom_max) {
            ui->spinBoxPlotZoomMin->setValue(zoom_max - 110);
        }
    }
}
void SettingsDialog::accept() { this->done(1); }
void SettingsDialog::reject() { this->done(0); }
