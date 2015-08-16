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

#ifndef KORADSCPI_H
#define KORADSCPI_H

#include <memory>
#include <mutex>
#include <thread>
#include <exception>

#include <QObject>
#include <QDebug>
#include <QtSerialPort/QSerialPortInfo>
#include <QtSerialPort/QSerialPort>
#include <QString>
#include <QThread>

#include "serialqueue.h"
#include "serialcommand.h"

namespace KoradSCPI_constants
{
enum COMMANDS {
    SETCURRENT = 1,
    GETCURRENT,
    SETVOLTAGE,
    GETVOLTAGE,
    GETACTUALCURRENT,
    GETACTUALVOLTAGE,
    SETOPMODE,
    SETBEEP,
    SETOUT,
    GETSTATUS,
    GETIDN,
    GETSAVEDSETTINGS,
    SAVESETTINGS,
    SETOCP,
    SETOVP,
    SETDUMMY = 100
};

const std::map<int, QString> SERIALCOMMANDMAP = {
    {SETCURRENT, "ISET%1:%2"},     // Set current
    {GETCURRENT, "ISET%1?"},       // Get current that has been set
    {SETVOLTAGE, "VSET%1:%2"},     // Set voltage
    {GETVOLTAGE, "VSET%1?"},       // Get voltage that has been set
    {GETACTUALCURRENT, "IOUT%1?"}, // Get actual current
    {GETACTUALVOLTAGE, "VOUT%1?"}, // Get actual Voltage
    {SETOPMODE, "TRACK%1"},        // Selects the operation mode: independent,
                                   // trackingseries, or tracking parallel.
    {SETBEEP, "BEEP%1"},           // turn beep on or off
    {SETOUT, "OUT%1"},             // turn output on or off
    {GETSTATUS, "STATUS?"},        // request status
    {GETIDN, "*IDN?"},             // get device identification string
    {GETSAVEDSETTINGS, "RCL%1"},   // set device to memorized settings
    {SAVESETTINGS, "SAV%1"},       // save current settings on memory position
    {SETOCP, "OCP%1"},             // switch over current protection
    {SETOVP, "OVP%1"},             // switch over voltage protection
    {SETDUMMY, "DUMMY"},           // just some dummy command
};
}

class KoradSCPI : public QObject
{

    Q_OBJECT

public:
    /**
     * @brief KoradSPIC
     * @param serialPortName
     * @throw std::runtime_error when Serial Port is not open
     */
    KoradSCPI(const QString serialPortName);
    ~KoradSCPI();

    QString getserialPortName();

    void getIdentification();
    void getStatus();

signals:

    void requestFinished(std::shared_ptr<SerialCommand>);

private:
    const QSerialPort::BaudRate BAUDRATE = QSerialPort::BaudRate::Baud9600;
    const QSerialPort::FlowControl FLOWCONTROL =
        QSerialPort::FlowControl::NoFlowControl;
    const QSerialPort::DataBits DATABITS = QSerialPort::DataBits::Data8;
    const QSerialPort::Parity PARITY = QSerialPort::Parity::NoParity;
    const QSerialPort::StopBits STOPBITS = QSerialPort::StopBits::OneStop;

    std::map<int, QString> serialCommandMap;

    QString serialPortName;
    QSerialPort *serialPort;
    std::mutex serialPortGuard;

    bool backgroundWorkerThreadRun;
    std::thread backgroundWorkerThread;

    SerialQueue serQueue;

    void threadFunc();

    void readWriteData(std::shared_ptr<SerialCommand> com);
};

#endif // KORADSCPI_H
