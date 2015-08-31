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

#include "koradscpi.h"

namespace powcon = PowerSupplySCPI_constants;
namespace korcon = KoradSCPI_constants;
namespace statuscon = PowerSupplyStatus_constants;

KoradSCPI::KoradSCPI(const QString &serialPortName, const int &noOfChannels)
    : PowerSupplySCPI(serialPortName, noOfChannels)
{
    this->BAUDRATE = QSerialPort::BaudRate::Baud9600;
    this->FLOWCONTROL = QSerialPort::FlowControl::NoFlowControl;
    this->DATABITS = QSerialPort::DataBits::Data8;
    this->PARITY = QSerialPort::Parity::NoParity;
    this->STOPBITS = QSerialPort::StopBits::OneStop;

    this->statusCommands = {powcon::GETSTATUS, powcon::GETCURRENT,
                            powcon::GETACTUALCURRENT, powcon::GETVOLTAGE,
                            powcon::GETACTUALVOLTAGE};
}

KoradSCPI::~KoradSCPI() {}

void KoradSCPI::getIdentification()
{
    // parameters two and three are irrelevant here.
    this->serQueue.push(static_cast<int>(powcon::COMMANDS::GETIDN), 1,
                        QVariant(0), true);
}

void KoradSCPI::getStatus()
{
    this->serQueue.push(static_cast<int>(powcon::COMMANDS::GETSTATUS), 1,
                        QVariant(0), true);
}

void KoradSCPI::setVoltage(const int &channel, const double &value)
{
    this->serQueue.push(static_cast<int>(powcon::COMMANDS::SETVOLTAGE), channel,
                        QVariant(value), false);
}

void KoradSCPI::setCurrent(const int &channel, const double &value)
{
    this->serQueue.push(static_cast<int>(powcon::COMMANDS::SETCURRENT), channel,
                        QVariant(value), false);
}

void KoradSCPI::setOCP(bool status)
{
    QVariant val = 0;
    if (status) {
        val = 1;
    }
    this->serQueue.push(static_cast<int>(powcon::COMMANDS::SETOCP), 1,
                        val, false);
}

void KoradSCPI::setOVP(bool status)
{
    QVariant val = 0;
    if (status) {
        val = 1;
    }
    this->serQueue.push(static_cast<int>(powcon::COMMANDS::SETOVP), 1,
                        val, false);
}

void KoradSCPI::setLocked(bool status)
{

}

void KoradSCPI::setBeep(bool status)
{
    QVariant val = 0;
    if (status) {
        val = 1;
    }
    this->serQueue.push(static_cast<int>(powcon::COMMANDS::SETBEEP), 1,
                        val, false);
}

void KoradSCPI::setOutput(bool status)
{
    QVariant val = 0;
    if (status) {
        val = 1;
    }
    this->serQueue.push(static_cast<int>(powcon::COMMANDS::SETOUT), 1,
                        val, false);
}

void KoradSCPI::processStatusCommands(std::shared_ptr<PowerSupplyStatus> &status,
                                      const std::shared_ptr<SerialCommand> &com)
{
    if (com->getCommand() == powcon::COMMANDS::GETSTATUS) {
        /*
         * Decoding the Korad Status Byte is pretty simpel.
         * MSB -> LSB
         * 7   not defined
         * 6   Output
         * 5   Lock
         * 4   Beep
         * 2,3 Channel Tracking Mode
         * 1   CH2 CC|CV mode
         * 0   CH1 CC|CV mode
         */
        QByteArray val = com->getValue().toByteArray();
        uint8_t blas = val[0];
        if (val[0] & (1 << 6)) {
            status->setOutput(true);
        }
        if (val[0] & (1 << 5)) {
            status->setLocked(true);
        }
        if (val[0] & (1 << 4)) {
            status->setBeeper(true);
        }
        // TODO: Add check for tracking mode
        if (val[0] & (1 << 1)) {
            status->setChannelMode(std::make_pair(
                2, PowerSupplyStatus_constants::MODE::CONSTANT_VOLTAGE));
        } else {
            status->setChannelMode(std::make_pair(
                2, PowerSupplyStatus_constants::MODE::CONSTANT_CURRENT));
        }
        if (val[0] & (1 << 0)) {
            status->setChannelMode(std::make_pair(
                1, PowerSupplyStatus_constants::MODE::CONSTANT_VOLTAGE));
        } else {
            status->setChannelMode(std::make_pair(
                1, PowerSupplyStatus_constants::MODE::CONSTANT_CURRENT));
        }
    }

    if (com->getCommand() == powcon::COMMANDS::GETCURRENT) {
        status->setAdjustedCurrent(std::make_pair(com->getPowerSupplyChannel(),
                                                  com->getValue().toDouble()));
    }

    if (com->getCommand() == powcon::COMMANDS::GETACTUALCURRENT) {
        status->setActualCurrent(std::make_pair(com->getPowerSupplyChannel(),
                                                com->getValue().toDouble()));
    }

    if (com->getCommand() == powcon::COMMANDS::GETVOLTAGE) {
        status->setAdjustedVoltage(std::make_pair(com->getPowerSupplyChannel(),
                                                  com->getValue().toDouble()));
    }

    if (com->getCommand() == powcon::COMMANDS::GETACTUALVOLTAGE) {
        status->setActualVoltage(std::make_pair(com->getPowerSupplyChannel(),
                                                com->getValue().toDouble()));
    }
}

QByteArray KoradSCPI::prepareCommand(const std::shared_ptr<SerialCommand> &com)
{
    // First job create the command
    QString commandString = korcon::SERIALCOMMANDMAP.at(
        static_cast<powcon::COMMANDS>(com->getCommand()));
    if (commandString.indexOf("%") > 0) {
        com->getValue().isNull() == true
            ? commandString = commandString.arg(com->getPowerSupplyChannel())
            : commandString = commandString.arg(com->getPowerSupplyChannel())
                                  .arg(com->getValue().toString());
    }

    QByteArray commandByte = commandString.toLocal8Bit();
    return commandByte;
}
