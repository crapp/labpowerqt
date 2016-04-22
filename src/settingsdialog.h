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

#ifndef SETTINGSDIALOG_H
#define SETTINGSDIALOG_H

#include <QDialog>
#include <QListWidgetItem>
#include <QQuickView>
#include <QAbstractButton>
#include <QDialogButtonBox>
#include <QStandardPaths>
#include <QMessageBox>
#include <QFileDialog>

#include <QtSerialPort/QSerialPort>
#include <QtSerialPort/QSerialPortInfo>

#include <QSqlDatabase>
#include <QSqlError>

#include <QDebug>
#include <QSettings>

#include <iostream>
#include <memory>

#include "settingsdefinitions.h"
#include "settingsdefault.h"
#include "databasedef.h"
#include "devicewizard.h"

namespace Ui
{
class SettingsDialog;
}

class SettingsDialog : public QDialog
{
    Q_OBJECT

public:
    explicit SettingsDialog(QWidget *parent = 0);
    ~SettingsDialog();

private:
    Ui::SettingsDialog *ui;

    QListWidgetItem *lastItem;
    QString defaultSqlFile; /**< Path to default data location on users system */

    void initGeneral();
    void initDevice();
    void initPlot();
    void initRecord();
    void setupSettingsList();
    bool checkSettingsChanged(QListWidgetItem *lastItem);
    void saveSettings(int currentRow);
    void restoreSettings(int currentRow);

private slots:
    void settingCategoryChanged(int currentRow);

    void buttonBoxClicked(QAbstractButton *button);
    void devicesComboBoxUpdate();

    /**
     * @brief Slot that enforces a minimum zoom range
     *
     * @param value
     */
    void zoomMinMaxSync(int value);

    void accept();
    void reject();
};

#endif  // SETTINGSDIALOG_H
