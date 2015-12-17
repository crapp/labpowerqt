// labpowerqt is a Gui application to control programmable lab power supplies
// Copyright © 2015 Christian Rapp <0x2a@posteo.org>

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

#include "mainwindow.h"
#include "ui_mainwindow.h"

namespace globcon = global_constants;
namespace setcon = settings_constants;

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent), ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    qRegisterMetaType<std::shared_ptr<SerialCommand>>();
    qRegisterMetaType<std::shared_ptr<PowerSupplyStatus>>();

    QString titleString;
    QTextStream titleStream(&titleString, QIODevice::WriteOnly);
    titleStream << "LabPowerQt " << LABPOWERQT_VERSION_MAJOR << "."
                << LABPOWERQT_VERSION_MINOR << "." << LABPOWERQT_VERSION_PATCH;
    this->setWindowTitle(titleString);

    // Restore saved geometry and state
    QSettings settings;
    settings.beginGroup(setcon::MAINWINDOW_GROUP);
    this->restoreGeometry(settings.value(setcon::MAINWINDOW_GEO).toByteArray());
    this->restoreState(settings.value(setcon::MAINWINDOW_STATE).toByteArray());
    ui->tabWidgetMainWindow->setCurrentIndex(
        settings.value(setcon::MAINWINDOW_ACTIVE_TAB, 0).toInt());
    settings.endGroup();

    // create model and controller
    this->applicationModel = std::make_shared<LabPowerModel>();
    this->controller = std::unique_ptr<LabPowerController>(
        new LabPowerController(this->applicationModel));

    QObject::connect(ui->tabWidgetMainWindow, &QTabWidget::currentChanged, this,
                     &MainWindow::tabWidgetChangedIndex);

    this->setupMenuBarActions();
    this->setupModelConnections();
    this->setupValuesDialog();
    this->setupControlConnections();
}

MainWindow::~MainWindow() { delete ui; }

void MainWindow::dataUpdated()
{
    QSettings settings;
    settings.beginGroup(setcon::DEVICE_GROUP);
    settings.beginGroup(settings.value(setcon::DEVICE_ACTIVE).toString());
    for (int i = 1; i <= settings.value(setcon::DEVICE_CHANNELS).toInt(); i++) {
        double voltage =
            this->applicationModel->getVoltage(static_cast<globcon::CHANNEL>(i));
        double actualVoltage = this->applicationModel->getActualVoltage(
            static_cast<globcon::CHANNEL>(i));
        double current =
            this->applicationModel->getCurrent(static_cast<globcon::CHANNEL>(i));
        double actualCurrent = this->applicationModel->getActualCurrent(
            static_cast<globcon::CHANNEL>(i));
        double wattage =
            this->applicationModel->getWattage(static_cast<globcon::CHANNEL>(i));

        this->ui->widgetGraph->addData(i, voltage,
                                       this->applicationModel->getTime(),
                                       globcon::DATATYPE::SETVOLTAGE);
        this->ui->widgetGraph->addData(i, actualVoltage,
                                       this->applicationModel->getTime(),
                                       globcon::DATATYPE::VOLTAGE);
        this->ui->widgetGraph->addData(i, current,
                                       this->applicationModel->getTime(),
                                       globcon::DATATYPE::SETCURRENT);
        this->ui->widgetGraph->addData(i, actualCurrent,
                                       this->applicationModel->getTime(),
                                       globcon::DATATYPE::CURRENT);
        this->ui->widgetGraph->addData(i, wattage,
                                       this->applicationModel->getTime(),
                                       globcon::DATATYPE::WATTAGE);

        ui->widgetDisplay->dataUpdate(
            std::move(QVariant(QString::number(
                voltage, 'f',
                settings.value(setcon::DEVICE_VOLTAGE_ACCURACY).toInt()))),
            globcon::DATATYPE::SETVOLTAGE, i);
        ui->widgetDisplay->dataUpdate(
            std::move(QVariant(QString::number(
                actualVoltage, 'f',
                settings.value(setcon::DEVICE_VOLTAGE_ACCURACY).toInt()))),
            globcon::DATATYPE::VOLTAGE, i);

        ui->widgetDisplay->dataUpdate(
            std::move(QVariant(QString::number(
                current, 'f',
                settings.value(setcon::DEVICE_CURRENT_ACCURACY).toInt()))),
            globcon::DATATYPE::SETCURRENT, i);
        ui->widgetDisplay->dataUpdate(
            std::move(QVariant(QString::number(
                actualCurrent, 'f',
                settings.value(setcon::DEVICE_CURRENT_ACCURACY).toInt()))),
            globcon::DATATYPE::CURRENT, i);

        ui->widgetDisplay->dataUpdate(
            std::move(QVariant(QString::number(wattage, 'f', 3))),
            globcon::DATATYPE::WATTAGE, i);

        this->applicationModel->getOutput(static_cast<globcon::CHANNEL>(i))
            ? ui->widgetDisplay->dataUpdate(std::move(QVariant("On")),
                                            globcon::CONTROL::OUTPUT, i)
            : ui->widgetDisplay->dataUpdate(std::move(QVariant("Off")),
                                            globcon::CONTROL::OUTPUT, i);
        this->applicationModel->getChannelMode(
            static_cast<globcon::CHANNEL>(i)) == globcon::MODE::CONSTANT_CURRENT
            ? ui->widgetDisplay->dataUpdate(globcon::CONSTANT_CURRENT, i)
            : ui->widgetDisplay->dataUpdate(globcon::CONSTANT_VOLTAGE, i);
    }

    this->applicationModel->getOVP()
        ? ui->widgetDisplay->dataUpdate(std::move(QVariant("On")),
                                        globcon::CONTROL::OVP, 0)
        : ui->widgetDisplay->dataUpdate(std::move(QVariant("Off")),
                                        globcon::CONTROL::OVP, 0);
    this->applicationModel->getOCP()
        ? ui->widgetDisplay->dataUpdate(std::move(QVariant("On")),
                                        globcon::CONTROL::OCP, 0)
        : ui->widgetDisplay->dataUpdate(std::move(QVariant("Off")),
                                        globcon::CONTROL::OCP, 0);
    this->applicationModel->getOTP()
        ? ui->widgetDisplay->dataUpdate(std::move(QVariant("On")),
                                        globcon::CONTROL::OTP, 0)
        : ui->widgetDisplay->dataUpdate(std::move(QVariant("Off")),
                                        globcon::CONTROL::OTP, 0);
}

void MainWindow::deviceConnectionUpdated(bool connected)
{
    if (connected) {
        ui->widgetDisplay->dataUpdate(connected, globcon::CONTROL::CONNECT, 0);
        this->statusBar()->showMessage("Connected");
    } else {
        this->statusBar()->showMessage("Disconnected");
        ui->widgetDisplay->dataUpdate(connected, globcon::CONTROL::CONNECT, 0);
    }
}

void MainWindow::deviceIDUpdated()
{
    ui->widgetDisplay->dataUpdate(
        this->applicationModel->getDeviceIdentification(),
        globcon::CONTROL::DEVICEID, 0);
}

void MainWindow::setupMenuBarActions()
{
    // File menu
    QObject::connect(ui->actionSettings, SIGNAL(triggered()), this,
                     SLOT(showSettings()));
    QObject::connect(ui->actionExit, SIGNAL(triggered()), this, SLOT(close()));

    // Help menu
    QObject::connect(ui->actionReport_Bug, SIGNAL(triggered()), this,
                     SLOT(fileBugReport()));
    QObject::connect(ui->actionAbout_LabPowerQt, SIGNAL(triggered()), this,
                     SLOT(showAbout()));
    QObject::connect(ui->actionAbout_Qt, SIGNAL(triggered()), this,
                     SLOT(showAboutQt()));
}

void MainWindow::setupModelConnections()
{
    QObject::connect(
        this->applicationModel.get(), &LabPowerModel::deviceConnectionStatus,
        this, &MainWindow::deviceConnectionUpdated,
        static_cast<Qt::ConnectionType>(Qt::ConnectionType::AutoConnection |
                                        Qt::ConnectionType::UniqueConnection));
    QObject::connect(
        this->applicationModel.get(), &LabPowerModel::deviceID, this,
        &MainWindow::deviceIDUpdated,
        static_cast<Qt::ConnectionType>(Qt::ConnectionType::AutoConnection |
                                        Qt::ConnectionType::UniqueConnection));
    QObject::connect(
        this->applicationModel.get(), &LabPowerModel::statusUpdate, this,
        &MainWindow::dataUpdated,
        static_cast<Qt::ConnectionType>(Qt::ConnectionType::AutoConnection |
                                        Qt::ConnectionType::UniqueConnection));
}

void MainWindow::setupValuesDialog()
{
    this->valuesDialogData = std::make_shared<FloatingValuesDialogData>();
    this->valuesDialog = std::make_shared<FloatingValuesDialog>(
        nullptr, Qt::Dialog | Qt::FramelessWindowHint);
    this->valuesDialog->setValuesDialogData(this->valuesDialogData);
    ui->widgetDisplay->setValuesDialog(this->valuesDialogData,
                                       this->valuesDialog);

    QSettings settings;
    settings.beginGroup(setcon::DEVICE_GROUP);
    settings.beginGroup(settings.value(setcon::DEVICE_ACTIVE).toString());
    if (settings.contains(setcon::DEVICE_PORT)) {
        this->valuesDialog->updateDeviceSpecs(
            settings.value(setcon::DEVICE_VOLTAGE_MIN).toDouble(),
            settings.value(setcon::DEVICE_VOLTAGE_MAX).toDouble(),
            settings.value(setcon::DEVICE_VOLTAGE_ACCURACY).toUInt(),
            settings.value(setcon::DEVICE_CURRENT_MIN).toDouble(),
            settings.value(setcon::DEVICE_CURRENT_MAX).toDouble(),
            settings.value(setcon::DEVICE_CURRENT_ACCURACY).toUInt(),
            settings.value(setcon::DEVICE_CHANNELS).toUInt());
    }
}

void MainWindow::setupControlConnections()
{
    QSettings settings;
    settings.beginGroup(setcon::DEVICE_GROUP);
    settings.beginGroup(settings.value(setcon::DEVICE_ACTIVE).toString());
    if (settings.contains(setcon::DEVICE_PORT)) {
        QObject::connect(ui->widgetDisplay, &DisplayArea::doubleValueChanged,
                         this, &MainWindow::displayWidgetDoubleResult,
                         static_cast<Qt::ConnectionType>(
                             Qt::ConnectionType::AutoConnection |
                             Qt::ConnectionType::UniqueConnection));
        QObject::connect(ui->widgetDisplay,
                         &DisplayArea::deviceControlValueChanged, this,
                         &MainWindow::deviceControl,
                         static_cast<Qt::ConnectionType>(
                             Qt::ConnectionType::AutoConnection |
                             Qt::ConnectionType::UniqueConnection));
        if (this->applicationModel->getDeviceConnected()) {
            this->statusBar()->showMessage("Connected");
        } else {
            this->statusBar()->showMessage("Disconnected");
        }
    } else {
        this->statusBar()->showMessage("No Device configuration found");
    }

    QObject::connect(
        ui->widgetRecord, &RecordArea::record, this, &MainWindow::recordToggle,
        static_cast<Qt::ConnectionType>(Qt::ConnectionType::AutoConnection |
                                        Qt::ConnectionType::UniqueConnection));
}

void MainWindow::fileBugReport()
{
    QDesktopServices::openUrl(QUrl("https://github.com/crapp/labpowerqt"));
}

void MainWindow::showAbout()
{
    AboutMe abm;
    abm.exec(); // show it application modal
}

void MainWindow::showAboutQt() { QMessageBox::aboutQt(this, tr("About Qt")); }

void MainWindow::showSettings()
{
    QSettings settings;
    settings.beginGroup(setcon::DEVICE_GROUP);
    settings.beginGroup(settings.value(setcon::DEVICE_ACTIVE).toString());
    QByteArray hash = settings.value(setcon::DEVICE_HASH).toByteArray();
    settings.endArray();
    // release the serial port
    // this->controller->disconnectDevice();
    SettingsDialog sd;
    sd.exec();

    settings.beginGroup(settings.value(setcon::DEVICE_ACTIVE).toString());
    QByteArray newHash = settings.value(setcon::DEVICE_HASH).toByteArray();
    // FIXME: In fact this hole thing here is useless. When the user changes the
    // active device in the settings dialog and a recording or whatever is
    // running it will have some nasty effects. So we need some sort signal
    // Update the values for the valuesDialog floating widget
    if (newHash != hash) {
        this->valuesDialog->updateDeviceSpecs(
            settings.value(setcon::DEVICE_VOLTAGE_MIN).toDouble(),
            settings.value(setcon::DEVICE_VOLTAGE_MAX).toDouble(),
            settings.value(setcon::DEVICE_VOLTAGE_ACCURACY).toUInt(),
            settings.value(setcon::DEVICE_CURRENT_MIN).toDouble(),
            settings.value(setcon::DEVICE_CURRENT_MAX).toDouble(),
            settings.value(setcon::DEVICE_CURRENT_ACCURACY).toUInt(),
            settings.value(setcon::DEVICE_CHANNELS).toUInt());
        // disconnect here or the application behaves very strange
        QObject::disconnect(this->applicationModel.get(),
                            &LabPowerModel::statusUpdate, this, 0);

        // stop recording
        ui->widgetRecord->recordExternal(false, "");
        this->controller->toggleRecording(false, "");
        this->controller->disconnectDevice();
        // refresh the ui in case number of channels changed.
        ui->widgetDisplay->setupChannels();
        ui->widgetGraph->setupGraph();
        QObject::connect(
            this->applicationModel.get(), &LabPowerModel::statusUpdate, this,
            &MainWindow::dataUpdated, static_cast<Qt::ConnectionType>(
                                          Qt::ConnectionType::AutoConnection |
                                          Qt::ConnectionType::UniqueConnection));
        this->setupControlConnections();
    }
}

void MainWindow::tabWidgetChangedIndex(int index)
{
    QSettings settings;
    settings.beginGroup(setcon::MAINWINDOW_GROUP);
    settings.setValue(setcon::MAINWINDOW_ACTIVE_TAB, index);
}

void MainWindow::displayWidgetDoubleResult(double val, int dt, int channel)
{
    switch (static_cast<globcon::DATATYPE>(dt)) {
    case globcon::DATATYPE::SETVOLTAGE:
        this->controller->setVoltage(channel, val);
        break;
    case globcon::DATATYPE::SETCURRENT:
        this->controller->setCurrent(channel, val);
        break;
    default:
        break;
    }

    qDebug() << Q_FUNC_INFO << "Received " << val << " "
             << "Channel: " << channel;
}

void MainWindow::deviceControl(int control, int channel)
{
    switch (static_cast<globcon::CONTROL>(control)) {
    case globcon::CONTROL::CONNECT: {
        if (this->applicationModel->getDeviceConnected()) {
            QSettings settings;
            settings.beginGroup(setcon::GENERAL_GROUP);
            if (settings.value(setcon::GENERAL_DISC).toBool()) {
                if (QMessageBox::question(this, "Disconnect Device",
                                          "Do you really want to disconnect?",
                                          QMessageBox::StandardButton::Yes |
                                              QMessageBox::StandardButton::No,
                                          QMessageBox::StandardButton::No) ==
                    static_cast<int>(QMessageBox::StandardButton::Yes))
                    this->controller->disconnectDevice();
            } else {
                this->controller->disconnectDevice();
            }
        } else {
            this->controller->connectDevice();
        }
        break;
    }
    case globcon::CONTROL::SOUND:
        this->applicationModel->getDeviceMute()
            ? this->controller->setAudio(false)
            : this->controller->setAudio(true);
        break;
    case globcon::CONTROL::LOCK:
        this->applicationModel->getDeviceLocked()
            ? this->controller->setLock(false)
            : this->controller->setLock(true);
        break;
    case globcon::CONTROL::OUTPUT:
        this->applicationModel->getOutput(static_cast<globcon::CHANNEL>(channel))
            ? this->controller->setOutput(channel - 1, false)
            : this->controller->setOutput(channel - 1, true);
        break;
    // TODO OCP, OVP, OTP controls missing. Also missing in model and controller.
    case globcon::CONTROL::OVP:
        this->applicationModel->getOVP() ? this->controller->setOVP(false)
                                         : this->controller->setOVP(true);
        break;
    case globcon::CONTROL::OCP:
        this->applicationModel->getOCP() ? this->controller->setOCP(false)
                                         : this->controller->setOCP(true);
        break;
    case globcon::CONTROL::OTP:
        this->applicationModel->getOTP() ? this->controller->setOTP(false)
                                         : this->controller->setOTP(true);
        break;
    default:
        break;
    }
}

void MainWindow::recordToggle(bool status, QString name)
{
    this->controller->toggleRecording(status, std::move(name));
    ui->tabHistory->updateModel();
}

void MainWindow::closeEvent(QCloseEvent *event)
{
    QSettings settings;
    settings.beginGroup(setcon::GENERAL_GROUP);
    if (settings.value(setcon::GENERAL_EXIT).toBool()) {
        QMessageBox box;
        // TODO: Can't set parent. Messagebox transparent after this :/??
        // box.setParent(this);
        box.setStandardButtons(QMessageBox::Cancel | QMessageBox::Ok);
        box.setDefaultButton(QMessageBox::Cancel);
        box.setIcon(QMessageBox::Icon::Question);
        box.setText("Do you really want to quit labpowerqt?");
        box.exec();
        if (box.result() == QMessageBox::StandardButton::Cancel) {
            event->ignore();
            return;
        }
    }
    settings.endGroup();
    settings.beginGroup(setcon::MAINWINDOW_GROUP);
    settings.setValue(setcon::MAINWINDOW_GEO, this->saveGeometry());
    settings.setValue(setcon::MAINWINDOW_STATE, this->saveState());
    settings.endGroup();
    QWidget::closeEvent(event);
}
