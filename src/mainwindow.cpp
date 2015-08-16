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
    settings.beginGroup(SETTINGS_MAINWINDOW_GROUP);
    this->restoreGeometry(settings.value(SETTINGS_MAINWINDOW_GEO).toByteArray());
    this->restoreState(settings.value(SETTINGS_MAINWINDOW_STATE).toByteArray());
    settings.endGroup();

    this->setupMenuBarActions();
    this->setupAnimations();

    this->ui->frame_2->setMaximumHeight(0);

    // Connect signal slots
    QObject::connect(ui->Knob, SIGNAL(valueChanged(double)), this,
                     SLOT(setLCDDisplay(double)));
    QObject::connect(
        ui->pushButton_2,
        SIGNAL(clicked()), this, SLOT(showHideVoltCurrentSpinners()));
}

MainWindow::~MainWindow() { delete ui; }

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
    this->hideVoltCurrentSpinner->setStartValue(this->showVoltCurrentSpinner->endValue());
    this->hideVoltCurrentSpinner->setEndValue(0);
}

void MainWindow::fileBugReport()
{
    // TODO: Change URL as soon as github repo is created
    QDesktopServices::openUrl(QUrl("https://github.com/crapp"));
}

void MainWindow::showAbout()
{
    AboutMe abm;
    abm.exec(); // show it application modal
}

void MainWindow::showAboutQt() { QMessageBox::aboutQt(this, tr("About Qt")); }

void MainWindow::showSettings()
{
    SettingsDialog sd;
    sd.exec();
}

void MainWindow::showHideVoltCurrentSpinners()
{
    if (ui->frame_2->maximumHeight() == 0) {
        this->showVoltCurrentSpinner->start();
    } else {
        this->hideVoltCurrentSpinner->start();
    }
}

void MainWindow::setLCDDisplay(double val)
{
    ui->lcdNumber->display(QString::number(val, 'f', 2));
}

void MainWindow::closeEvent(QCloseEvent *event)
{
    QSettings settings;
    settings.beginGroup(SETTINGS_MAINWINDOW_GROUP);
    settings.setValue(SETTINGS_MAINWINDOW_GEO, this->saveGeometry());
    settings.setValue(SETTINGS_MAINWINDOW_STATE, this->saveState());
    settings.endGroup();
    QWidget::closeEvent(event);
}
