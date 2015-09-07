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

#include "labpowermodel.h"

LabPowerModel::LabPowerModel()
{
    this->status = std::make_shared<PowerSupplyStatus>();
    this->deviceConnected = false;
    this->deviceIdentification = "";
}

bool LabPowerModel::getDeviceConnected() { return this->deviceConnected; }

void LabPowerModel::setDeviceConnected(bool connected)
{
    this->deviceConnected = connected;
    emit this->deviceConnectionStatus(this->deviceConnected);
}

QString LabPowerModel::getDeviceIdentification()
{
    return this->deviceIdentification;
}

void LabPowerModel::setDeviceIdentification(const QString &id)
{
    this->deviceIdentification = id;
    emit this->deviceID();
}

bool LabPowerModel::getDeviceLocked() { return this->status->getLocked(); }

bool LabPowerModel::getDeviceMute() { return this->status->getBeeper(); }

bool LabPowerModel::getOutput(const global_constants::CHANNEL &c)
{
    try {
        return this->status->getChannelOutput(static_cast<int>(c));
    } catch (const std::out_of_range &ex) {
        qDebug() << Q_FUNC_INFO << ex.what();
    }
    return false;
}

global_constants::MODE
LabPowerModel::getChannelMode(const global_constants::CHANNEL &c)
{
    return static_cast<global_constants::MODE>(
        this->status->getChannelMode(static_cast<int>(c)));
}

double LabPowerModel::getVoltage(const global_constants::CHANNEL &c)
{
    return this->status->getAdjustedVoltage(static_cast<int>(c));
}

double LabPowerModel::getActualVoltage(const global_constants::CHANNEL &c)
{
    return this->status->getActualVoltage(static_cast<int>(c));
}

double LabPowerModel::getCurrent(const global_constants::CHANNEL &c)
{
    return this->status->getAdjustedCurrent(static_cast<int>(c));
}

double LabPowerModel::getActualCurrent(const global_constants::CHANNEL &c)
{
    return this->status->getActualCurrent(static_cast<int>(c));
}

double LabPowerModel::getWattage(const global_constants::CHANNEL &c)
{
    return this->status->getWattage(static_cast<int>(c));
}

void LabPowerModel::updatePowerSupplyStatus(
    std::shared_ptr<PowerSupplyStatus> status)
{
    this->status = status;
    emit this->statusUpdate();
}
