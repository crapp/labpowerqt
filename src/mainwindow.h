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

#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QSettings>
#include <QString>
#include <QTextStream>
// QUrl and QDesktopServices to open Webbrowser (file a bug report)
#include <QDesktopServices>
#include <QDir>
#include <QPropertyAnimation>
#include <QSignalMapper>
#include <QUrl>

#include <QMessageBox>

#include <iostream>
#include <memory>
#include <utility>
#include <vector>

#include <ealogger/ealogger.h>

#include <config.h>
#include "global.h"
#include "labpowercontroller.h"
#include "labpowermodel.h"
#include "settingsdefault.h"
#include "settingsdefinitions.h"

#include "aboutme.h"
#include "floatingvaluesdialog.h"
#include "plottingarea.h"
#include "settingsdialog.h"

namespace Ui
{
class MainWindow;
}

/**
 * @brief Mainwindow class for the main window
 *
 * @details
 * This class is the central place for labpowerqt. Important objects like the
 * controller or model are instantiated here. The main window is controlled
 * from here.
 */
class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

public slots:
    /**
     * @brief Connected to the application model, distributes new data to display
     * and plot area
     */
    void dataUpdated();
    /**
     * @brief Slot that gets called whenever the device connection changes
     *
     * @param connected
     */
    void deviceConnectionUpdated(bool connected);
    /**
     * @brief When the id command was send to the device this slot will be invoked
     * in response
     */
    void deviceIDUpdated();

private:
    Ui::MainWindow *ui;

    std::unique_ptr<QPropertyAnimation> showVoltCurrentSpinner;
    std::unique_ptr<QPropertyAnimation> hideVoltCurrentSpinner;

    std::shared_ptr<ealogger::Logger> log;

    std::unique_ptr<LabPowerController> controller;
    std::shared_ptr<LabPowerModel> applicationModel;

    std::shared_ptr<FloatingValuesDialogData> valuesDialogData;
    std::shared_ptr<FloatingValuesDialog> valuesDialog;

    /**
     * @brief setupMenuBarActions
     *
     * Connect MenuBar Actions
     */
    void setupMenuBarActions();

    void setupModelConnections();
    void setupValuesDialog();
    void setupControlConnections();

private slots:
    /**
     * @brief File a bug on github.com using your default browser
     */
    void fileBugReport();
    void showAbout();
    void showAboutQt();
    /**
     * @brief Open settings dialog
     */
    void showSettings();

    void tabWidgetChangedIndex(int index);

    void displayWidgetDoubleResult(double val, int dt, int channel);
    void deviceControl(int control, int channel);
    void recordToggle(bool status, QString name);

    // QWidget interface
protected:
    void closeEvent(QCloseEvent *event);
};

#endif  // MAINWINDOW_H
