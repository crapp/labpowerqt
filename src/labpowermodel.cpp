// labpowerqt is a Gui application to control programmable lab power supplies
// Copyright © 2015, 2016, 2022 Christian Rapp <0x2a at posteo dot org>

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

#include <utility>

LabPowerModel::LabPowerModel() : QObject()
{
    this->status = std::make_shared<PowerSupplyStatus>();
    this->deviceConnected = false;
    this->deviceIdentification = "";
    this->record = false;
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

void LabPowerModel::setDeviceIdentification(QString id)
{
    this->deviceIdentification = std::move(id);
    emit this->deviceID();
}

bool LabPowerModel::getDeviceLocked() { return this->status->getLocked(); }
bool LabPowerModel::getDeviceMute() { return this->status->getBeeper(); }
bool LabPowerModel::getOutput(global_constants::LPQ_CHANNEL c)
{
    try {
        return this->status->getChannelOutput(static_cast<int>(c));
    } catch (const std::out_of_range &ex) {
        LogInstance::get_instance().eal_error(ex.what());
    }
    return false;
}

global_constants::LPQ_MODE LabPowerModel::getChannelMode(
    global_constants::LPQ_CHANNEL c)
{
    return static_cast<global_constants::LPQ_MODE>(
        this->status->getChannelMode(static_cast<int>(c)));
}

std::chrono::system_clock::time_point LabPowerModel::getTime()
{
    return this->status->getTime();
}

void LabPowerModel::setVoltageSet(global_constants::LPQ_CHANNEL c, double val)
{
    this->status->setVoltageSet(std::make_pair(static_cast<int>(c), val));
}

double LabPowerModel::getVoltageSet(global_constants::LPQ_CHANNEL c)
{
    return this->status->getVoltageSet(static_cast<int>(c));
}

double LabPowerModel::getVoltage(global_constants::LPQ_CHANNEL c)
{
    return this->status->getVoltage(static_cast<int>(c));
}

void LabPowerModel::setCurrentSet(global_constants::LPQ_CHANNEL c, double val)
{
    this->status->setCurrentSet(std::make_pair(static_cast<int>(c), val));
}

double LabPowerModel::getCurrentSet(global_constants::LPQ_CHANNEL c)
{
    return this->status->getCurrentSet(static_cast<int>(c));
}

double LabPowerModel::getCurrent(global_constants::LPQ_CHANNEL c)
{
    return this->status->getCurrent(static_cast<int>(c));
}

double LabPowerModel::getWattage(global_constants::LPQ_CHANNEL c)
{
    return this->status->getWattage(static_cast<int>(c));
}

void LabPowerModel::setOVP(bool status) { this->status->setOvp(status); }
bool LabPowerModel::getOVP() { return this->status->getOvp(); }
void LabPowerModel::setOCP(bool status) { this->status->setOcp(status); }
bool LabPowerModel::getOCP() { return this->status->getOcp(); }
void LabPowerModel::setOTP(bool status) { this->status->setOtp(status); }
bool LabPowerModel::getOTP() { return this->status->getOtp(); }
long long LabPowerModel::getDuration() { return this->status->getDuration(); }
std::vector<std::shared_ptr<PowerSupplyStatus>> LabPowerModel::getBuffer()
{
    return this->statusBuffer;
}

int LabPowerModel::getBufferSize()
{
    return static_cast<int>(this->statusBuffer.size());
}

bool LabPowerModel::getRecord() { return this->record; }
void LabPowerModel::setRecord(bool status) { this->record = status; }
void LabPowerModel::updatePowerSupplyStatus(
    std::shared_ptr<PowerSupplyStatus> status)
{
    this->status = std::move(status);
    if (this->record)
        this->statusBuffer.push_back(this->status);
    emit this->statusUpdate();
}

void LabPowerModel::clearBuffer() { this->statusBuffer.clear(); }
