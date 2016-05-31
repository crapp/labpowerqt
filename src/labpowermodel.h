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

#ifndef LABPOWERMODEL_H
#define LABPOWERMODEL_H

#include <QObject>
#include <QString>

#include <chrono>
#include <memory>
#include <vector>

#include "global.h"
#include "log_instance.h"
#include "powersupplystatus.h"

/**
 * @brief Class that models the state of a lab power supply
 *
 * @details
 * This class holds all the information a device can provide. It does not store
 * the characteristics the user provided with the device wizard.
 * The information will be updated by the application controller. There are
 * several signals available that will notify potentional listeners.
 */
class LabPowerModel : public QObject
{
    Q_OBJECT

public:
    LabPowerModel();

    bool getDeviceConnected();
    void setDeviceConnected(bool connected);

    QString getDeviceIdentification();
    void setDeviceIdentification(QString id);

    bool getDeviceLocked();
    bool getDeviceMute();

    bool getOutput(global_constants::LPQ_CHANNEL c);
    global_constants::LPQ_MODE getChannelMode(global_constants::LPQ_CHANNEL c);

    std::chrono::system_clock::time_point getTime();

    void setVoltageSet(global_constants::LPQ_CHANNEL c, double val);
    double getVoltageSet(global_constants::LPQ_CHANNEL c);
    double getVoltage(global_constants::LPQ_CHANNEL c);
    void setCurrentSet(global_constants::LPQ_CHANNEL c, double val);
    double getCurrentSet(global_constants::LPQ_CHANNEL c);
    double getCurrent(global_constants::LPQ_CHANNEL c);
    double getWattage(global_constants::LPQ_CHANNEL c);

    void setOVP(bool status);
    bool getOVP();
    void setOCP(bool status);
    bool getOCP();
    void setOTP(bool status);
    bool getOTP();

    long long getDuration();

    std::vector<std::shared_ptr<PowerSupplyStatus>> getBuffer();
    int getBufferSize();

    bool getRecord();
    void setRecord(bool status);

signals:

    void statusUpdate();
    void deviceConnectionStatus(bool connected);
    void deviceID();

public slots:

    void updatePowerSupplyStatus(std::shared_ptr<PowerSupplyStatus> status);
    /**
     * @brief Clear interal buffer of PowerSupplyStatus objects
     */
    void clearBuffer();

private:
    std::vector<std::shared_ptr<PowerSupplyStatus>> statusBuffer;
    std::shared_ptr<PowerSupplyStatus> status;

    bool deviceConnected;
    QString deviceIdentification;
    bool record;
};

#endif  // LABPOWERMODEL_H
