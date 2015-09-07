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

#include "serialqueue.h"
#include "serialcommand.h"
#include "powersupplystatus.h"

namespace PowerSupplySCPI_constants
{
enum COMMANDS {
    SETCURRENT = 1,
    GETCURRENT,
    SETVOLTAGE,
    GETVOLTAGE,
    GETACTUALCURRENT,
    GETACTUALVOLTAGE,
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

class PowerSupplySCPI : public QObject
{
    Q_OBJECT
public:
    PowerSupplySCPI(const QString &serialPortName, const int &noOfChannels,
                    const int &voltageAccuracy, const int &currentAccuracy,
                    QObject *parent = 0);
    virtual ~PowerSupplySCPI();

    void startPowerSupplyBackgroundThread();

    /**
     * @brief getserialPortName
     * @return
     */
    QString getserialPortName();
    virtual void getIdentification() = 0;
    virtual void getStatus() = 0;
    virtual void changeChannel(const int &channel) = 0;
    virtual void setVoltage(const int &channel, const double &value) = 0;
    virtual void setCurrent(const int &channel, const double &value) = 0;
    virtual void setOCP(bool status) = 0;
    virtual void setOVP(bool status) = 0;
    // TODO Not sure if this actually supported by any device.
    virtual void setOTP(bool status) = 0;
    virtual void setLocked(bool status) = 0;
    virtual void setBeep(bool status) = 0;
    virtual void setTracking(const global_constants::TRACKING &trMode) = 0;
    virtual void setOutput(const int &channel, bool status) = 0;

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

public slots:

protected:
    /**
     * @brief serQueue This Queue holds all commands we want to send to the
     * device
     */
    SerialQueue serQueue;

    QString serialPortName;
    QSerialPort::BaudRate port_baudraute = QSerialPort::BaudRate::Baud9600;
    QSerialPort::FlowControl port_flowControl =
        QSerialPort::FlowControl::NoFlowControl;
    QSerialPort::DataBits port_databits = QSerialPort::DataBits::Data8;
    QSerialPort::Parity port_parity = QSerialPort::Parity::NoParity;
    QSerialPort::StopBits port_stopbits = QSerialPort::StopBits::OneStop;

    int noOfChannels;
    int voltageAccuracy;
    int currentAccuracy;

    /**
     * @brief canCalculateWattage Devices that can measure actual current but not power can calculate power usage.
     */
    bool canCalculateWattage;

    // TODO: Change this raw pointer to a smart pointer
    QSerialPort *serialPort;
    std::mutex serialPortGuard;

    bool backgroundWorkerThreadRun;
    std::thread backgroundWorkerThread;

    /**
     * @brief statusCommands The commands needed to get the Power Supply status
     */
    std::vector<PowerSupplySCPI_constants::COMMANDS> statusCommands;

    void threadFunc();

    virtual void readWriteData(std::shared_ptr<SerialCommand> com);
    virtual QByteArray
    prepareCommand(const std::shared_ptr<SerialCommand> &com) = 0;
    virtual std::vector<std::shared_ptr<SerialCommand>> prepareStatusCommands();
    virtual void
    processStatusCommands(const std::shared_ptr<PowerSupplyStatus> &status,
                          const std::shared_ptr<SerialCommand> &com) = 0;
    virtual void
    calculateWattage(const std::shared_ptr<PowerSupplyStatus> &status) = 0;

protected slots:
    /**
     * @brief deviceInitialization Use this slot in derived classes to connect it with the deviceOpen signal
     */
    virtual void deviceInitialization() = 0;
};

#endif // POWERSUPPLYSCPI_H
