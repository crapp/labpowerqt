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

#ifndef LABPOWERMODEL_H
#define LABPOWERMODEL_H

#include <QObject>
#include <QDebug>
#include <QString>

#include <memory>
#include <chrono>
#include <vector>

#include "global.h"
#include "powersupplystatus.h"

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

    bool getOutput(global_constants::CHANNEL c);
    global_constants::MODE getChannelMode(global_constants::CHANNEL c);

    std::chrono::system_clock::time_point getTime();

    double getVoltage(global_constants::CHANNEL c);
    double getActualVoltage(global_constants::CHANNEL c);
    double getCurrent(global_constants::CHANNEL c);
    double getActualCurrent(global_constants::CHANNEL c);
    double getWattage(global_constants::CHANNEL c);

    bool getOVP();
    bool getOCP();
    bool getOTP();

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
    void clearBuffer();

private:
    std::vector<std::shared_ptr<PowerSupplyStatus>> statusBuffer;
    std::shared_ptr<PowerSupplyStatus> status;

    bool deviceConnected;
    QString deviceIdentification;
    bool record;

};

#endif // LABPOWERMODEL_H
