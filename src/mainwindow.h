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

#include <QMessageBox>

#include <memory>

#include "config.h"
#include "settingsdefinitions.h"
#include "labpowercontroller.h"
#include "labpowermodel.h"

#include "aboutme.h"
#include "settingsdialog.h"
#include "floatingvaluesdialog.h"

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
    void disableControls(bool status);

private:
    Ui::MainWindow *ui;

    std::unique_ptr<QPropertyAnimation> showVoltCurrentSpinner;
    std::unique_ptr<QPropertyAnimation> hideVoltCurrentSpinner;

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

    void setupValuesDialog();

private slots:
    void fileBugReport();
    void showAbout();
    void showAboutQt();
    void showSettings();

    void valuesDialogDoubleResult(const double &val,
                                  const int &w);

    void showHideVoltCurrentSpinners();

    void setVoltage(const QPoint &pos, const double &value);
    void setCurrent(const QPoint &pos, const double &value);

    // QWidget interface
protected:
    void closeEvent(QCloseEvent *event);
};

#endif // MAINWINDOW_H
