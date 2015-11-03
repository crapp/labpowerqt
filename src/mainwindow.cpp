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

    this->setVoltageLabels = {ui->labelCH1SetVoltage, ui->labelCH2SetVoltage};
    this->setCurrentLabels = {ui->labelCH1SetCurrent, ui->labelCH2SetCurrent};
    this->actualVoltageLabels = {ui->labelCH1CurrVoltage,
                                 ui->labelCH2CurrVoltage};
    this->actualCurrentLabels = {ui->labelCH1CurrCurrent,
                                 ui->labelCH2CurrCurrent};
    this->wattageLabels = {ui->labelCH1Wattage, ui->labelCH2Wattage};
    this->outputLabels = {ui->labelCH1OutputStatus, ui->labelCH2OutputStatus};
    this->channelModeLabels = {ui->labelCH1ModeStatus, ui->labelCH2ModeStatus};

    QString titleString;
    QTextStream titleStream(&titleString, QIODevice::WriteOnly);
    titleStream << "LabPowerQt " << VERSION_MAJOR << "." << VERSION_MINOR << "."
                << VERSION_PATCH;
    this->setWindowTitle(titleString);

    // Restore saved geometry and state
    QSettings settings;
    settings.beginGroup(setcon::MAINWINDOW_GROUP);
    this->restoreGeometry(settings.value(setcon::MAINWINDOW_GEO).toByteArray());
    this->restoreState(settings.value(setcon::MAINWINDOW_STATE).toByteArray());
    settings.endGroup();

    this->ui->labelCH1SetVoltage->setChannel(globcon::CHANNEL1);
    this->ui->labelCH1SetVoltage->setInputwidget(
        ClickableLabel::INPUTWIDGETS::VOLTAGE);
    this->ui->labelCH1SetCurrent->setChannel(globcon::CHANNEL1);
    this->ui->labelCH1SetCurrent->setInputwidget(ClickableLabel::CURRENT);
    this->ui->labelCH1OutputStatus->setNoReturnValue(true);

    this->ui->labelCH2SetVoltage->setChannel(globcon::CHANNEL2);
    this->ui->labelCH2SetVoltage->setInputwidget(
        ClickableLabel::INPUTWIDGETS::VOLTAGE);
    this->ui->labelCH2SetCurrent->setChannel(globcon::CHANNEL2);
    this->ui->labelCH2SetCurrent->setInputwidget(
        ClickableLabel::INPUTWIDGETS::CURRENT);
    this->ui->labelCH2OutputStatus->setNoReturnValue(true);

    this->ui->labelDisplayHeaderPlug->setNoReturnValue(true);
    this->ui->labelDisplayHeaderMute->setNoReturnValue(true);
    this->ui->labelDisplayHeaderLock->setNoReturnValue(true);

    // create model and controller
    this->applicationModel = std::make_shared<LabPowerModel>();
    this->controller = std::unique_ptr<LabPowerController>(
        new LabPowerController(this->applicationModel));

    this->setupMenuBarActions();
    this->setupAnimations();
    this->setupModelConnections();
    this->setupValuesDialog();
    this->setupControlConnections();
}

MainWindow::~MainWindow() { delete ui; }

void MainWindow::dataUpdated()
{
    // this->ui->labelCH1SetVoltage->setText(this->applicationModel->get)
    QSettings settings;
    settings.beginGroup(setcon::DEVICE_GROUP);
    if (settings.contains(setcon::DEVICE_PORT)) {
        for (int i = 1; i <= settings.value(setcon::DEVICE_CHANNELS).toInt();
             i++) {
            double voltage = this->applicationModel->getVoltage(
                static_cast<globcon::CHANNEL>(i));
            double actualVoltage = this->applicationModel->getActualVoltage(
                static_cast<globcon::CHANNEL>(i));
            double current = this->applicationModel->getCurrent(
                static_cast<globcon::CHANNEL>(i));
            double actualCurrent = this->applicationModel->getActualCurrent(
                static_cast<globcon::CHANNEL>(i));
            double wattage = this->applicationModel->getWattage(
                static_cast<globcon::CHANNEL>(i));

            this->ui->widgetGraph->addData(i, voltage,
                                           this->applicationModel->getTime(),
                                           globcon::DATATYPE::VOLTAGE);
            this->ui->widgetGraph->addData(i, actualVoltage,
                                           this->applicationModel->getTime(),
                                           globcon::DATATYPE::ACTUALVOLTAGE);
            this->ui->widgetGraph->addData(i, current,
                                           this->applicationModel->getTime(),
                                           globcon::DATATYPE::CURRENT);
            this->ui->widgetGraph->addData(i, actualCurrent,
                                           this->applicationModel->getTime(),
                                           globcon::DATATYPE::ACTUALCURRENT);
            this->ui->widgetGraph->addData(i, wattage,
                                           this->applicationModel->getTime(),
                                           globcon::DATATYPE::WATTAGE);

            this->setVoltageLabels.at(i - 1)->setText(QString::number(
                voltage, 'f',
                settings.value(setcon::DEVICE_VOLTAGE_ACCURACY).toInt()));
            this->actualVoltageLabels.at(i - 1)->setText(QString::number(
                actualVoltage, 'f',
                settings.value(setcon::DEVICE_VOLTAGE_ACCURACY).toInt()));
            this->setCurrentLabels.at(i - 1)->setText(QString::number(
                current, 'f',
                settings.value(setcon::DEVICE_CURRENT_ACCURACY).toInt()));
            this->actualCurrentLabels.at(i - 1)->setText(QString::number(
                actualCurrent, 'f',
                settings.value(setcon::DEVICE_CURRENT_ACCURACY).toInt()));
            this->wattageLabels.at(i - 1)
                ->setText(QString::number(wattage, 'f', 3));
            this->applicationModel->getOutput(static_cast<globcon::CHANNEL>(i))
                ? this->outputLabels.at(i - 1)->setText("On")
                : this->outputLabels.at(i - 1)->setText("Off");
            this->applicationModel->getChannelMode(static_cast<globcon::CHANNEL>(
                i)) == globcon::MODE::CONSTANT_CURRENT
                ? this->channelModeLabels.at(i - 1)->setText("CC")
                : this->channelModeLabels.at(i - 1)->setText("CV");
        }
    }
}

void MainWindow::deviceConnectionUpdated(bool connected)
{
    if (connected) {
        this->ui->labelDisplayHeaderPlug->setPixmap(
            QPixmap(":/icons/plug_in_orange.png"));
    } else {
        this->ui->labelDisplayHeaderPlug->setPixmap(
            QPixmap(":/icons/plug_out_orange.png"));
    }
}

void MainWindow::deviceIDUpdated()
{
    this->ui->labelDisplayDeviceName->setText(
        this->applicationModel->getDeviceIdentification());
}

void MainWindow::disableControls(bool status) {}

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

void MainWindow::setupAnimations()
{
    //    this->showVoltCurrentSpinner = std::unique_ptr<QPropertyAnimation>(
    //        new QPropertyAnimation(this->ui->frame_2, "maximumHeight"));
    //    this->showVoltCurrentSpinner->setDuration(500);
    //    this->showVoltCurrentSpinner->setStartValue(0);
    //    this->showVoltCurrentSpinner->setEndValue(ui->frame_2->height());

    //    this->hideVoltCurrentSpinner = std::unique_ptr<QPropertyAnimation>(
    //        new QPropertyAnimation(this->ui->frame_2, "maximumHeight"));
    //    this->hideVoltCurrentSpinner->setDuration(500);
    //    this->hideVoltCurrentSpinner->setStartValue(
    //        this->showVoltCurrentSpinner->endValue());
    //    this->hideVoltCurrentSpinner->setEndValue(0);
}

void MainWindow::setupModelConnections()
{
    QObject::connect(this->applicationModel.get(),
                     SIGNAL(deviceConnectionStatus(bool)), this,
                     SLOT(deviceConnectionUpdated(bool)));
    QObject::connect(this->applicationModel.get(), SIGNAL(deviceID()), this,
                     SLOT(deviceIDUpdated()));
    QObject::connect(this->applicationModel.get(), SIGNAL(statusUpdate()), this,
                     SLOT(dataUpdated()));
}

void MainWindow::setupValuesDialog()
{
    QSettings settings;
    settings.beginGroup(setcon::DEVICE_GROUP);
    if (settings.contains(setcon::DEVICE_PORT)) {
        this->valuesDialog =
            std::unique_ptr<FloatingValuesDialog>(new FloatingValuesDialog(
                nullptr, Qt::Dialog | Qt::FramelessWindowHint));

        this->valuesDialog->updateDeviceSpecs(
            settings.value(setcon::DEVICE_VOLTAGE_MIN).toDouble(),
            settings.value(setcon::DEVICE_VOLTAGE_MAX).toDouble(),
            settings.value(setcon::DEVICE_VOLTAGE_ACCURACY).toUInt(),
            settings.value(setcon::DEVICE_CURRENT_MIN).toDouble(),
            settings.value(setcon::DEVICE_CURRENT_MAX).toDouble(),
            settings.value(setcon::DEVICE_CURRENT_ACCURACY).toUInt(),
            settings.value(setcon::DEVICE_CHANNELS).toUInt());

    } else {
        this->valuesDialog = nullptr;
    }
}

void MainWindow::setupControlConnections()
{
    QSettings settings;
    settings.beginGroup(setcon::DEVICE_GROUP);
    if (settings.contains(setcon::DEVICE_PORT)) {

        QObject::connect(this->valuesDialog.get(),
                         SIGNAL(doubleValueAccepted(const double &, const int &,
                                                    const int &)),
                         this, SLOT(valuesDialogDoubleResult(
                                   const double &, const int &, const int &)));

        QObject::connect(ui->labelCH1SetVoltage,
                         SIGNAL(doubleClick(const QPoint &, const double &,
                                            const int &, const int &)),
                         this,
                         SLOT(setVoltageCurrent(const QPoint &, const double &,
                                                const int &, const int &)));
        QObject::connect(ui->labelCH1SetCurrent,
                         SIGNAL(doubleClick(const QPoint &, const double &,
                                            const int &, const int &)),
                         this,
                         SLOT(setVoltageCurrent(const QPoint &, const double &,
                                                const int &, const int &)));

        // Setup a signal mapper that will map all simpel on off controls to an
        // integer based on the CONTROLS enum.
        this->boolDeviceControls =
            std::unique_ptr<QSignalMapper>(new QSignalMapper());
        QObject::connect(this->boolDeviceControls.get(), SIGNAL(mapped(int)),
                         this, SLOT(deviceControl(int)));

        // header controls
        this->boolDeviceControls->setMapping(
            ui->labelDisplayHeaderPlug,
            static_cast<int>(MainWindow::CONTROL::CONNECT));
        QObject::connect(ui->labelDisplayHeaderPlug,
                         SIGNAL(doubleClickNoValue()),
                         this->boolDeviceControls.get(), SLOT(map()));
        this->boolDeviceControls->setMapping(
            ui->labelDisplayHeaderMute,
            static_cast<int>(MainWindow::CONTROL::SOUND));
        QObject::connect(ui->labelDisplayHeaderMute,
                         SIGNAL(doubleClickNoValue()),
                         this->boolDeviceControls.get(), SLOT(map()));
        this->boolDeviceControls->setMapping(
            ui->labelDisplayHeaderLock,
            static_cast<int>(MainWindow::CONTROL::LOCK));
        QObject::connect(ui->labelDisplayHeaderLock,
                         SIGNAL(doubleClickNoValue()),
                         this->boolDeviceControls.get(), SLOT(map()));

        // footer controls

        // channel output
        this->channelOutputControls =
            std::unique_ptr<QSignalMapper>(new QSignalMapper());
        QObject::connect(this->channelOutputControls.get(), SIGNAL(mapped(int)),
                         this, SLOT(channelOutputControl(int)));

        for (int i = 1; i <= settings.value(setcon::DEVICE_CHANNELS).toInt();
             i++) {
            QLabel *label = this->outputLabels.at(i - 1);
            this->channelOutputControls->setMapping(label, i);
            QObject::connect(label, SIGNAL(doubleClickNoValue()),
                             this->channelOutputControls.get(), SLOT(map()));
        }
    }
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
    // release the serial port
    // this->controller->disconnectDevice();
    SettingsDialog sd;
    sd.exec();

    // Udate the values for the valuesDialog floating widget
    QSettings settings;
    settings.beginGroup(setcon::DEVICE_GROUP);
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

void MainWindow::valuesDialogDoubleResult(const double &val,
                                          const int &sourceWidget,
                                          const int &sourceChannel)
{
    switch (static_cast<ClickableLabel::INPUTWIDGETS>(sourceWidget)) {
    case ClickableLabel::INPUTWIDGETS::VOLTAGE:
        this->controller->setVoltage(sourceChannel, val);
        break;
    case ClickableLabel::INPUTWIDGETS::CURRENT:
        this->controller->setCurrent(sourceChannel, val);
        break;
    case ClickableLabel::INPUTWIDGETS::TRACKING:
        // TODO: Implement tracking Mode
        // this->controller->setTrackingMode(0);
        break;
    default:
        break;
    }

    qDebug() << Q_FUNC_INFO << "Received " << val << " Source: " << sourceWidget
             << " "
             << "Channel: " << sourceChannel;
}

void MainWindow::setVoltageCurrent(const QPoint &pos, const double &value,
                                   const int &iWidget, const int &channel)
{
    // map widget cursor position to global position (top left)
    QPoint globalPos = ui->labelCH1SetVoltage->mapToGlobal(pos);
    this->valuesDialog->move(globalPos.x(), globalPos.y());
    this->valuesDialog->setInputWidget(iWidget);
    this->valuesDialog->setSourceChannel(channel);
    // TODO: Why is this sourceWidget needed?
    this->valuesDialog->setSourceWidget(iWidget);
    this->valuesDialog->setInputWidgetValue(value);
    this->valuesDialog->exec();
}

void MainWindow::deviceControl(int control)
{
    switch (static_cast<MainWindow::CONTROL>(control)) {
    case CONTROL::CONNECT:
        if (this->applicationModel->getDeviceConnected()) {
            this->controller->disconnectDevice();
        } else {
            this->controller->connectDevice();
        }
        break;
    case CONTROL::SOUND:
        if (this->applicationModel->getDeviceMute()) {
            this->controller->setAudio(false);
        } else {
            this->controller->setAudio(true);
        }
    case CONTROL::LOCK:
        if (this->applicationModel->getDeviceLocked()) {
            this->controller->setLock(false);
        } else {
            this->controller->setLock(true);
        }
    default:
        break;
    }
}

void MainWindow::channelOutputControl(int control)
{
    this->applicationModel->getOutput(static_cast<globcon::CHANNEL>(control))
        ? this->controller->setOutput(control - 1, false)
        : this->controller->setOutput(control - 1, true);
}

void MainWindow::showHideVoltCurrentSpinners()
{
    //    if (ui->frame_2->maximumHeight() == 0) {
    //        this->showVoltCurrentSpinner->start();
    //    } else {
    //        this->hideVoltCurrentSpinner->start();
    //    }
    //    this->controller->getIdentification();
}

void MainWindow::closeEvent(QCloseEvent *event)
{
    QSettings settings;
    settings.beginGroup(setcon::MAINWINDOW_GROUP);
    settings.setValue(setcon::MAINWINDOW_GEO, this->saveGeometry());
    settings.setValue(setcon::MAINWINDOW_STATE, this->saveState());
    settings.endGroup();
    QWidget::closeEvent(event);
}
