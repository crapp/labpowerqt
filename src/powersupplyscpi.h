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

#ifndef POWERSUPPLYSCPI_H
#define POWERSUPPLYSCPI_H

#include <thread>
#include <mutex>
#include <memory>
#include <vector>
#include <chrono>
#include <exception>

#include <QObject>
#include <QByteArray>
#include <QtSerialPort/QtSerialPort>
#include <QString>
#include <QMutex>
#include <QMutexLocker>

#include "serialqueue.h"
#include "serialcommand.h"
#include "powersupplystatus.h"

namespace PowerSupplySCPI_constants
{
/**
 * @brief An enum that holds all SCPI commands this application currently
 * understands
 */
enum COMMANDS {
    SETCURRENTSET = 1,
    GETCURRENTSET,
    SETVOLTAGESET,
    GETVOLTAGESET,
    GETCURRENT,
    GETVOLTAGE,
    GETOPMODE, /**< CC or CV mode */
    SETCHANNELTRACKING,
    GETCHANNELTRACKING,
    SETBEEP,
    GETBEEP,
    SETLOCKED,
    GETLOCKED,
    SETOUT,
    GETOUT,
    GETSTATUS,
    GETIDN,
    GETSAVEDSETTINGS,
    SAVESETTINGS,
    SETOCP,
    GETOCP,
    SETOVP,
    GETOVP,
    SETOTP,
    GETOTP,
    SETDUMMY = 100 /**< A dummy command intended for internal use. */
};
}

/**
 * @brief Base class for power supplies based on the SCPI protocol
 */
class PowerSupplySCPI : public QObject
{
    Q_OBJECT

public:
    PowerSupplySCPI(QString serialPortName, QByteArray deviceHash,
                    int noOfChannels, int voltageAccuracy, int currentAccuracy,
                    QSerialPort::BaudRate brate,
                    QSerialPort::FlowControl flowctl,
                    QSerialPort::DataBits dbits, QSerialPort::Parity parity,
                    QSerialPort::StopBits sbits, int portTimeOut,
                    QObject *parent = 0);
    virtual ~PowerSupplySCPI();

    /**
     * @brief Background Thread that checks a queue for new commands to send to the device
     */
    void startPowerSupplyBackgroundThread();
    void stopPowerSupplyBackgroundThread();

    /**
     * @brief Get the name of the serial port
     * @return
     */
    QString getserialPortName();
    /**
     * @brief Every Device has a device name.
     * @return
     */
    QByteArray getDeviceHash();
    virtual void getIdentification() = 0;
    virtual void getStatus() = 0;
    virtual void changeChannel(int channel) = 0;
    virtual void setVoltage(int channel, double value) = 0;
    virtual void getVoltage(int channel) = 0;
    virtual void getActualVoltage(int channel) = 0;
    virtual void setCurrent(int channel, double value) = 0;
    virtual void getCurrent(int channel) = 0;
    virtual void getActualCurrent(int channel) = 0;
    virtual void setOCP(bool status) = 0;
    virtual void setOVP(bool status) = 0;
    virtual void setOTP(bool status) = 0;
    virtual void setLocked(bool status) = 0;
    virtual void setBeep(bool status) = 0;
    virtual void setTracking(global_constants::TRACKING trMode) = 0;
    virtual void setOutput(int channel, bool status) = 0;

signals:

    // TODO: Sending this as const reference would be great.
    void requestFinished(std::shared_ptr<SerialCommand>);
    void statusReady(std::shared_ptr<PowerSupplyStatus>);

    /**
     * @brief errorReadWrite Signal emitted when we could not read or write the device node
     * @param errorString
     */
    void errorReadWrite(const QString &errorString);
    /**
     * @brief errorOpen Signal that indicates that the device port could not be opened
     * @param errorString String from QIODevice
     */
    void errorOpen(const QString &errorString);
    /**
     * @brief deviceOpen Connection to device successfully established
     */
    void deviceOpen();

    void backgroundThreadStopped();

public slots:

protected:
    /**
     * @brief serQueue This Queue holds all commands we want to send to the
     * device
     */
    SerialQueue serQueue;

    QString serialPortName;
    QByteArray deviceHash;

    int noOfChannels;
    int voltageAccuracy;
    int currentAccuracy;

    QSerialPort::BaudRate port_baudraute;
    QSerialPort::FlowControl port_flowControl;
    QSerialPort::DataBits port_databits;
    QSerialPort::Parity port_parity;
    QSerialPort::StopBits port_stopbits;

    int portTimeOut;

    /**
     * @brief canCalculateWattage Devices that can measure actual current but not
     * power can calculate power usage.
     */
    bool canCalculateWattage;

    // TODO: Change this raw pointer to a smart pointer
    QSerialPort *serialPort;
    // Unfortunately we have Qt Threads and Mutexes as QSerialPort does not like
    // to be used with std::thread. It worked quite okay on Linux but other
    // platforms had major problems with it.
    QMutex qserialPortGuard;

    bool backgroundWorkerThreadRun;

    /**
     * @brief statusCommands The commands needed to get the Power Supply status
     */
    std::vector<PowerSupplySCPI_constants::COMMANDS> statusCommands;

    std::shared_ptr<PowerSupplyStatus> powStatus;

    /**
     * @brief This method is run by a external QThread instance
     *
     * @details
     * The QSerialPort has to be created inside this method as it needs to be in
     * the same thread that accesses it later.
     */
    void threadFunc();

    virtual void readWriteData(std::shared_ptr<SerialCommand> com);
    virtual QByteArray prepareCommandByteArray(
        const std::shared_ptr<SerialCommand> &com) = 0;
    virtual std::vector<std::shared_ptr<SerialCommand>>
    prepareStatusCommands() = 0;
    virtual void processCommands(
        const std::shared_ptr<PowerSupplyStatus> &status,
        const std::shared_ptr<SerialCommand> &com) = 0;
    virtual void updateNewPStatus(
        const std::shared_ptr<PowerSupplyStatus> &status) = 0;
    virtual void calculateWattage(
        const std::shared_ptr<PowerSupplyStatus> &status) = 0;

protected slots:
    /**
     * @brief deviceInitialization Use this slot in derived classes to connect it
     * with the deviceOpen signal
     */
    virtual void deviceInitialization() = 0;
};

#endif  // POWERSUPPLYSCPI_H
