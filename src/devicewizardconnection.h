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

#ifndef DEVICEWIZARDCONNECTION_H
#define DEVICEWIZARDCONNECTION_H

#include <QWidget>
#include <QWizardPage>
#include <QVBoxLayout>
#include <QGridLayout>
#include <QPushButton>
#include <QPlainTextEdit>

#include <QDebug>

#include <memory>

#include "global.h"
#include "koradscpi.h"
#include "serialcommand.h"

class DeviceWizardConnection : public QWizardPage
{
    Q_OBJECT
    Q_PROPERTY(QString deviceID READ getDeviceID)

public:
    DeviceWizardConnection(QWidget *parent = 0);

    QString getDeviceID() const;
    void initializePage() Q_DECL_OVERRIDE;
    // return if connection was successfull
    bool isComplete() const Q_DECL_OVERRIDE;

private:
    QPushButton *startTest;
    QPlainTextEdit *txt;
    QString devID;

    bool connectionSuccessfull;

    std::unique_ptr<PowerSupplySCPI> powerSupplyConnector;

private slots:
    void testConnection();
    void dataAvailable(std::shared_ptr<SerialCommand> command);
    void deviceError(QString errorString);
};

#endif // DEVICEWIZARDCONNECTION_H
