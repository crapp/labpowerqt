// labpowerqt is a Gui application to control programmable lab power supplies
// Copyright © 2015 Christian Rapp <0x2a at posteo.org>

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

#include "koradspic.h"

namespace korcon = KoradSPIC_constants;

KoradSPIC::KoradSPIC(QString seriaPortName)
{
    this->serialPort =
        std::unique_ptr<QSerialPort>(new QSerialPort(seriaPortName));

    if (!this->serialPort->open(QIODevice::ReadWrite)) {
        throw std::runtime_error(this->serialPort->errorString().toStdString());
    }

    this->serialPort->setBaudRate(this->BAUDRATE);
    this->serialPort->setFlowControl(this->FLOWCONTROL);
    this->serialPort->setDataBits(this->DATABITS);
    this->serialPort->setParity(this->PARITY);
    this->serialPort->setStopBits(this->STOPBITS);

    this->backgroundWorkerThreadRun = true;
}

KoradSPIC::~KoradSPIC()
{
    if (this->serialPort->isOpen()) {
        this->serialPort->close();
    }
    this->serQueue.push(korcon::COMMANDS::SETDUMMY, 1);
}

void KoradSPIC::getIdentification() {
    this->serQueue.push(korcon::COMMANDS::GETIDN, 1);
}

void KoradSPIC::threadFunc()
{
    while (this->backgroundWorkerThreadRun) {
        this->readWriteData(this->serQueue.pop());
    }
}

void KoradSPIC::readWriteData(std::shared_ptr<SerialCommand> com) {

}
