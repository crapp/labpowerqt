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

#include "koradscpi.h"

namespace korcon = KoradSCPI_constants;

KoradSCPI::KoradSCPI(QString seriaPortName)
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
    backgroundWorkerThread = std::thread(&KoradSCPI::threadFunc, this);
}

KoradSCPI::~KoradSCPI()
{
    if (this->serialPort->isOpen()) {
        this->serialPort->close();
    }
    this->serQueue.push(static_cast<int>(korcon::COMMANDS::SETDUMMY));
    this->backgroundWorkerThread.join();
}

void KoradSCPI::getIdentification()
{
    this->serQueue.push(static_cast<int>(korcon::COMMANDS::GETIDN));
}

void KoradSCPI::threadFunc()
{
    while (this->backgroundWorkerThreadRun) {
        this->readWriteData(this->serQueue.pop());
    }
}

void KoradSCPI::readWriteData(std::shared_ptr<SerialCommand> com)
{
    // First job create the command
    QString commandString = korcon::SERIALCOMMANDMAP.at(
        static_cast<korcon::COMMANDS>(com->getCommand()));
    com->getValue().isNull() == true
        ? commandString = commandString.arg(com->getPowerSupplyChannel())
        : commandString = commandString.arg(com->getPowerSupplyChannel())
                              .arg(com->getValue().toString());

    QByteArray commandByte = commandString.toLocal8Bit();
    serialPort->write(commandByte);
    // wait for ouur bytes to be written
    if (serialPort->waitForBytesWritten(1000)) {
        // is this command with a feedback?
        if (com->getCommandWithReply()) {
            // wait until port is ready to read
            this->serialPort->waitForReadyRead(1000);
            QByteArray reply = serialPort->readAll();
            while (this->serialPort->waitForReadyRead(10)) {
                reply += serialPort->readAll();
            }
            com->setReply(reply);
            // send our com object with the signal
            emit this->requestFinished(com);
        }
    } else {
        qDebug() << Q_FUNC_INFO
                 << "Could not write to serial port. Error number: "
                 << this->serialPort->error();
        ;
        // TODO emit an error
    }
}
