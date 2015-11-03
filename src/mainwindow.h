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

#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QString>
#include <QTextStream>
#include <QSettings>
// QUrl and QDesktopServices to open Webbrowser (file a bug report)
#include <QUrl>
#include <QDesktopServices>
#include <QPropertyAnimation>
#include <QSignalMapper>

#include <QMessageBox>

#include <memory>
#include <vector>

#include "config.h"
#include "settingsdefinitions.h"
#include "global.h"
#include "labpowercontroller.h"
#include "labpowermodel.h"

#include "aboutme.h"
#include "settingsdialog.h"
#include "floatingvaluesdialog.h"
#include "plottingarea.h"

namespace Ui
{
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

public slots:
    void dataUpdated();
    void deviceConnectionUpdated(bool connected);
    void deviceIDUpdated();
    void disableControls(bool status);

private:
    enum CONTROL { CONNECT = 0, SOUND, LOCK, OCP, OVP, OTP };

    Ui::MainWindow *ui;

    std::vector<QLabel *> setVoltageLabels;
    std::vector<QLabel *> actualVoltageLabels;
    std::vector<QLabel *> setCurrentLabels;
    std::vector<QLabel *> actualCurrentLabels;
    std::vector<QLabel *> wattageLabels;
    std::vector<QLabel *> outputLabels;
    std::vector<QLabel *> channelModeLabels;

    std::unique_ptr<QPropertyAnimation> showVoltCurrentSpinner;
    std::unique_ptr<QPropertyAnimation> hideVoltCurrentSpinner;

    /**
     * @brief boolDeviceControls Map an integer value to all "on/off" switches for the device ui.
     */
    std::unique_ptr<QSignalMapper> boolDeviceControls;
    std::unique_ptr<QSignalMapper> channelOutputControls;

    std::unique_ptr<LabPowerController> controller;
    std::shared_ptr<LabPowerModel> applicationModel;

    std::unique_ptr<FloatingValuesDialog> valuesDialog;

    /**
     * @brief setupMenuBarActions
     *
     * Connect MenuBar Actions
     */
    void setupMenuBarActions();
    void setupAnimations();

    void setupModelConnections();
    void setupValuesDialog();
    void setupControlConnections();

private slots:
    void fileBugReport();
    void showAbout();
    void showAboutQt();
    void showSettings();

    void valuesDialogDoubleResult(const double &val, const int &sourceWidget,
                                  const int &sourceChannel);
    void setVoltageCurrent(const QPoint &pos, const double &value,
                           const int &iWidget, const int &channel);
    void deviceControl(int control);
    void channelOutputControl(int control);

    void showHideVoltCurrentSpinners();

    // QWidget interface
protected:
    void closeEvent(QCloseEvent *event);
};

#endif // MAINWINDOW_H
