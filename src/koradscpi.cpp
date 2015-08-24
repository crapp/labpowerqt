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

KoradSCPI::KoradSCPI(const QString serialPortName)
    : PowerSupplySCPI(serialPortName)
{
    this->BAUDRATE = QSerialPort::BaudRate::Baud9600;
    this->FLOWCONTROL = QSerialPort::FlowControl::NoFlowControl;
    this->DATABITS = QSerialPort::DataBits::Data8;
    this->PARITY = QSerialPort::Parity::NoParity;
    this->STOPBITS = QSerialPort::StopBits::OneStop;

    this->statusCommands = {powcon::GETSTATUS, powcon::GETACTUALCURRENT,
                            powcon::GETACTUALVOLTAGE};
}

KoradSCPI::~KoradSCPI() {}

void KoradSCPI::getIdentification()
{
    // parameters two and three are irrelevant here.
    this->serQueue.push(static_cast<int>(powcon::COMMANDS::GETIDN), 1, 0, true);
}

void KoradSCPI::getStatus()
{
    this->serQueue.push(static_cast<int>(powcon::COMMANDS::GETSTATUS), 1, 0,
                        true);
}

void KoradSCPI::processStatusCommands(std::shared_ptr<PowerSupplyStatus> &status,
                                      const std::shared_ptr<SerialCommand> &com)
{
    if (com->getCommand() == powcon::COMMANDS::GETSTATUS) {
        QByteArray val = com->getValue().toByteArray();
        uint8_t blas = val[0];
        if ((val[0] & 0x80) == 0x80) {
            // status->set
        }
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
