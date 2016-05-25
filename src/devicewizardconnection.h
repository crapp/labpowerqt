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

#ifndef DEVICEWIZARDCONNECTION_H
#define DEVICEWIZARDCONNECTION_H

#include <QGridLayout>
#include <QPlainTextEdit>
#include <QPushButton>
#include <QThread>
#include <QVBoxLayout>
#include <QWidget>
#include <QWizardPage>

#include <memory>

#include "global.h"
#include "koradscpi.h"
#include "log_instance.h"
#include "serialcommand.h"

/**
 * @brief This class is part of the device wizard where we test the connection
 *
 * @details
 * The class mimics the behaviour of the controller class.
 */
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

    std::unique_ptr<QThread>
        t; /**< QThread that is used by the PowerSupplySCPI object to handle its queue */

    bool connectionSuccessfull;

    std::unique_ptr<PowerSupplySCPI> powerSupplyConnector;

private slots:

    /**
     * @brief Creates a PowerSupplySCPI derived object and sends an identification command
     */
    void testConnection();

    /**
     * @brief Method that receives the reply from the hardware device
     *
     * @param command
     *
     * @details
     *
     * Checks if reply is within defined parameters and the device the user
     * provided is usable by this application
     */
    void dataAvailable(std::shared_ptr<SerialCommand> command);

    /**
     * @brief Slot that receives error states
     *
     * @param errorString
     */
    void deviceError(QString errorString);
};

#endif  // DEVICEWIZARDCONNECTION_H
