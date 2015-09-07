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

namespace globcon = global_constants;
namespace powcon = PowerSupplySCPI_constants;
namespace korcon = KoradSCPI_constants;
namespace statuscon = PowerSupplyStatus_constants;

KoradSCPI::KoradSCPI(const QString &serialPortName, const int &noOfChannels,
                     const int &voltageAccuracy, const int &currentAccuracy)
    : PowerSupplySCPI(serialPortName, noOfChannels, voltageAccuracy,
                      currentAccuracy)
{
    this->canCalculateWattage = true;
    this->port_baudraute = QSerialPort::BaudRate::Baud9600;
    this->port_flowControl = QSerialPort::FlowControl::NoFlowControl;
    this->port_databits = QSerialPort::DataBits::Data8;
    this->port_parity = QSerialPort::Parity::NoParity;
    this->port_stopbits = QSerialPort::StopBits::OneStop;

    this->statusCommands = {powcon::GETSTATUS, powcon::GETCURRENT,
                            powcon::GETACTUALCURRENT, powcon::GETVOLTAGE,
                            powcon::GETACTUALVOLTAGE};

    QObject::connect(this, SIGNAL(deviceOpen()), this,
                     SLOT(deviceInitialization()));
}

KoradSCPI::~KoradSCPI()
{
    // We must make sure our thread has finished PowerSupplySCPI::readWriteData
    // before an Ibject of this class gets destroyed. The Problem is every
    // derived class must do this.
    // TODO: Document all special implementations needed by a derived class of
    // PowerSupplySCPI
    std::lock_guard<std::mutex> lock(this->serialPortGuard);
}

void KoradSCPI::getIdentification()
{
    // parameters two and three are irrelevant here.
    this->serQueue.push(static_cast<int>(powcon::COMMANDS::GETIDN), 0,
                        QVariant(0), true);
}

void KoradSCPI::getStatus()
{
    this->serQueue.push(static_cast<int>(powcon::COMMANDS::GETSTATUS), 0,
                        QVariant(0), true);
}

void KoradSCPI::changeChannel(const int &channel) {}

void KoradSCPI::setVoltage(const int &channel, const double &value)
{
    this->serQueue.push(
        static_cast<int>(powcon::COMMANDS::SETVOLTAGE), channel,
        QVariant(QString::number(value, 'f', this->voltageAccuracy)), false);
}

void KoradSCPI::setCurrent(const int &channel, const double &value)
{
    this->serQueue.push(
        static_cast<int>(powcon::COMMANDS::SETCURRENT), channel,
        QVariant(QString::number(value, 'f', this->currentAccuracy)), false);
}

void KoradSCPI::setOCP(bool status)
{
    QVariant val = 0;
    if (status) {
        val = 1;
    }
    this->serQueue.push(static_cast<int>(powcon::COMMANDS::SETOCP), 0, val,
                        false);
}

void KoradSCPI::setOVP(bool status)
{
    QVariant val = 0;
    if (status) {
        val = 1;
    }
    this->serQueue.push(static_cast<int>(powcon::COMMANDS::SETOVP), 0, val,
                        false);
}

void KoradSCPI::setOTP(bool status) {}

void KoradSCPI::setLocked(bool status) {}

void KoradSCPI::setBeep(bool status)
{
    QVariant val = 0;
    if (status) {
        val = 1;
    }
    this->serQueue.push(static_cast<int>(powcon::COMMANDS::SETBEEP), 0, val,
                        false);
}

void KoradSCPI::setTracking(const globcon::TRACKING &trMode)
{
    // TODO: Implement Tracking in Korad SCPI Class.
}

void KoradSCPI::setOutput(const int &channel, bool status)
{
    QVariant val = 0;
    if (status) {
        val = 1;
    }
    // sending 0 as Korad SCPI interface does not support different channels
    this->serQueue.push(static_cast<int>(powcon::COMMANDS::SETOUT), 0, val,
                        false);
}

void KoradSCPI::processStatusCommands(
    const std::shared_ptr<PowerSupplyStatus> &status,
    const std::shared_ptr<SerialCommand> &com)
{
    if (com->getCommand() == powcon::COMMANDS::GETSTATUS) {
        /*
         * Decoding the Korad Status Byte is pretty simple.
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
        qDebug() << Q_FUNC_INFO << "Status byte: " << val[0];
        // Unfortunately Korad SCPI does not seem to be able to determine
        // between different channels regarding ouput setting.
        if (val[0] & (1 << 6)) {
            for (int i = 1; i <= this->noOfChannels; i++) {
                status->setChannelOutput(std::make_pair(i, true));
            }
        } else {
            for (int i = 1; i <= this->noOfChannels; i++) {
                status->setChannelOutput(std::make_pair(i, false));
            }
        }
        if (val[0] & (1 << 5)) {
            status->setLocked(true);
        }
        if (val[0] & (1 << 4)) {
            status->setBeeper(true);
        }

        // TODO: Add check for tracking mode

        if (val[0] & (1 << 1)) {
            status->setChannelMode(
                std::make_pair(2, globcon::MODE::CONSTANT_VOLTAGE));
        } else {
            status->setChannelMode(
                std::make_pair(2, globcon::MODE::CONSTANT_CURRENT));
        }
        if (val[0] & (1 << 0)) {
            status->setChannelMode(
                std::make_pair(1, globcon::MODE::CONSTANT_VOLTAGE));
        } else {
            status->setChannelMode(
                std::make_pair(1, globcon::MODE::CONSTANT_CURRENT));
        }
    }

    if (com->getCommand() == powcon::COMMANDS::GETCURRENT) {

        QString val = com->getValue().toString();

        qDebug() << Q_FUNC_INFO << "Current: " << val;
        // strangely current values seem to end with a "K". Therefor it is not
        // possible to get a double with toDouble() directly. This fimrware is
        // really buggy.
        if (val.endsWith("K", Qt::CaseSensitivity::CaseInsensitive)) {
            val = val.left(val.length() - 1);
        }
        status->setAdjustedCurrent(
            std::make_pair(com->getPowerSupplyChannel(), val.toDouble()));
    }

    if (com->getCommand() == powcon::COMMANDS::GETACTUALCURRENT) {
        qDebug() << Q_FUNC_INFO << "Current Actual: " << com->getValue();
        status->setActualCurrent(std::make_pair(com->getPowerSupplyChannel(),
                                                com->getValue().toDouble()));
    }

    if (com->getCommand() == powcon::COMMANDS::GETVOLTAGE) {
        qDebug() << Q_FUNC_INFO << "Voltage: " << com->getValue();
        status->setAdjustedVoltage(std::make_pair(com->getPowerSupplyChannel(),
                                                  com->getValue().toDouble()));
    }

    if (com->getCommand() == powcon::COMMANDS::GETACTUALVOLTAGE) {
        qDebug() << Q_FUNC_INFO << "Voltage Actual: " << com->getValue();
        status->setActualVoltage(std::make_pair(com->getPowerSupplyChannel(),
                                                com->getValue().toDouble()));
    }
}

void KoradSCPI::calculateWattage(
    const std::shared_ptr<PowerSupplyStatus> &status)
{
    for (int i = 1; i <= this->noOfChannels; i++) {
        // P = U*I :)
        double wattValue =
            status->getActualVoltage(i) * status->getActualCurrent(i);
        qDebug() << Q_FUNC_INFO << "Calculated Watt for channel " << i << " = "
                 << wattValue;
        statuscon::CHANNELVALUE watt = std::make_pair(i, wattValue);
        status->setWattage(watt);
    }
}

void KoradSCPI::deviceInitialization()
{
    // The Korad firmware does not allow to query the status of OCP and OVP. So
    // we have to disable them both at the beginning. Otherwise our Application
    // could be in an undefined state.
    // this->setOCP(false);
    // this->setOVP(false);
}

QByteArray KoradSCPI::prepareCommand(const std::shared_ptr<SerialCommand> &com)
{
    // First job create the command
    QString commandString = korcon::SERIALCOMMANDMAP.at(
        static_cast<powcon::COMMANDS>(com->getCommand()));
    /*
     * We have four different command types.
     * 1. Command that does something with a channel
     * 2. Command that does something with a channel and a value
     * 3. Command that does something with a value
     * 4. Command that does something
     */
    if (commandString.indexOf("%") > 0) {
        if (com->getPowerSupplyChannel() == 0) {
            commandString = commandString.arg(com->getValue().toString());
        } else {
            com->getValue().isNull() == true
                ? commandString = commandString.arg(com->getPowerSupplyChannel())
                : commandString = commandString.arg(com->getPowerSupplyChannel())
                                      .arg(com->getValue().toString());
        }
    }

    qDebug() << Q_FUNC_INFO << "CommandString: " << commandString;

    QByteArray commandByte = commandString.toLocal8Bit();
    return commandByte;
}
