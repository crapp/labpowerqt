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

namespace setcon = settings_constants;

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent), ui(new Ui::MainWindow)
{
    ui->setupUi(this);

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

    this->setupMenuBarActions();
    this->setupAnimations();
    this->setupValuesDialog();

    this->ui->frame_2->setMaximumHeight(0);

    // Connect signal slots
    QObject::connect(ui->pushButton_2, SIGNAL(clicked()), this,
                     SLOT(showHideVoltCurrentSpinners()));

    qRegisterMetaType<std::shared_ptr<SerialCommand>>();
    qRegisterMetaType<std::shared_ptr<PowerSupplyStatus>>();

    // create model and controller
    this->applicationModel = std::make_shared<LabPowerModel>();
    this->controller = std::unique_ptr<LabPowerController>(
        new LabPowerController(this->applicationModel));
}

MainWindow::~MainWindow() { delete ui; }

void MainWindow::dataUpdated() {}

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
    this->showVoltCurrentSpinner = std::unique_ptr<QPropertyAnimation>(
        new QPropertyAnimation(this->ui->frame_2, "maximumHeight"));
    this->showVoltCurrentSpinner->setDuration(500);
    this->showVoltCurrentSpinner->setStartValue(0);
    this->showVoltCurrentSpinner->setEndValue(ui->frame_2->height());

    this->hideVoltCurrentSpinner = std::unique_ptr<QPropertyAnimation>(
        new QPropertyAnimation(this->ui->frame_2, "maximumHeight"));
    this->hideVoltCurrentSpinner->setDuration(500);
    this->hideVoltCurrentSpinner->setStartValue(
        this->showVoltCurrentSpinner->endValue());
    this->hideVoltCurrentSpinner->setEndValue(0);
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

        QObject::connect(
            this->valuesDialog.get(),
            SIGNAL(doubleValueAccepted(const double &, const int &)), this,
            SLOT(valuesDialogDoubleResult(const double &, const int &)));

        QObject::connect(ui->labelCH1SetVoltage,
                         SIGNAL(doubleClick(const QPoint &, const double &)),
                         this, SLOT(setVoltage(const QPoint &, const double &)));
        QObject::connect(ui->labelCH1SetCurrent,
                         SIGNAL(doubleClick(const QPoint &, const double &)),
                         this, SLOT(setCurrent(const QPoint &, const double &)));
    } else {
        this->valuesDialog = nullptr;
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
    this->controller->disconnectDevice();
    SettingsDialog sd;
    sd.exec();

    this->controller->connectDevice();

    // Udate the values for the valuesDialog floating widget
    QSettings settings;
    settings.beginGroup(setcon::DEVICE_GROUP);
    this->valuesDialog->updateDeviceSpecs(
        settings.value(setcon::DEVICE_VOLTAGE_MIN).toUInt(),
        settings.value(setcon::DEVICE_VOLTAGE_MAX).toUInt(),
        settings.value(setcon::DEVICE_VOLTAGE_ACCURACY).toUInt(),
        settings.value(setcon::DEVICE_CURRENT_MIN).toUInt(),
        settings.value(setcon::DEVICE_CURRENT_MAX).toUInt(),
        settings.value(setcon::DEVICE_CURRENT_ACCURACY).toUInt(),
        settings.value(setcon::DEVICE_CHANNELS).toUInt());
}

void MainWindow::valuesDialogDoubleResult(const double &val, const int &w)
{
    qDebug() << Q_FUNC_INFO << "Received " << val << " " << w << " from Dialog";
}

void MainWindow::showHideVoltCurrentSpinners()
{
    if (ui->frame_2->maximumHeight() == 0) {
        this->showVoltCurrentSpinner->start();
    } else {
        this->hideVoltCurrentSpinner->start();
    }
    this->controller->getIdentification();
}

void MainWindow::setVoltage(const QPoint &pos, const double &value)
{
    // map widget cursor position to global position (top left)
    QPoint globalPos = ui->labelCH1SetVoltage->mapToGlobal(pos);
    this->valuesDialog->move(globalPos.x(), globalPos.y());
    this->valuesDialog->setWidget(FloatingValuesDialog::INPUTWIDGETS::VOLTAGE);
    this->valuesDialog->setWidgetValue(value);
    this->valuesDialog->exec();
    // this->controller->setVoltage(voltage);
}

void MainWindow::setCurrent(const QPoint &pos, const double &value)
{
    QPoint globalPos = ui->labelCH1SetCurrent->mapToGlobal(pos);
    this->valuesDialog->move(globalPos.x(), globalPos.y());
    this->valuesDialog->setWidget(FloatingValuesDialog::INPUTWIDGETS::CURRENT);
    this->valuesDialog->setWidgetValue(value);
    this->valuesDialog->exec();
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
