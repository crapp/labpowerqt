#ifndef POWERSUPPLYSCPI_H
#define POWERSUPPLYSCPI_H

#include <thread>
#include <mutex>
#include <memory>
#include <vector>
#include <exception>

#include <QObject>
#include <QByteArray>
#include <QtSerialPort/QtSerialPort>

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
    SETDUMMY = 100 /**< A dummy command intended for internal use. */
};
}

class PowerSupplySCPI : public QObject
{
    Q_OBJECT
public:
    PowerSupplySCPI(QString serialPortName, QObject *parent = 0);
    virtual ~PowerSupplySCPI();

    /**
     * @brief getserialPortName
     * @return
     */
    QString getserialPortName();
    virtual void getIdentification() = 0;
    virtual void getStatus() = 0;
    virtual void setVoltage(const int &channel, const double &value) = 0;
    virtual void setCurrent(const int &channel, const double &value) = 0;
    virtual void setOCP(bool status) = 0;
    virtual void setOVP(bool status) = 0;
    virtual void setLocked(bool status) = 0;
    virtual void setBeep(bool status) = 0;
    virtual void setOutput(bool status) = 0;

signals:

    // TODO: Sending this as const reference would be great.
    void requestFinished(std::shared_ptr<SerialCommand>);
    void statusReady(std::shared_ptr<PowerSupplyStatus>);

public slots:

protected:
    QSerialPort::BaudRate BAUDRATE = QSerialPort::BaudRate::Baud9600;
    QSerialPort::FlowControl FLOWCONTROL =
        QSerialPort::FlowControl::NoFlowControl;
    QSerialPort::DataBits DATABITS = QSerialPort::DataBits::Data8;
    QSerialPort::Parity PARITY = QSerialPort::Parity::NoParity;
    QSerialPort::StopBits STOPBITS = QSerialPort::StopBits::OneStop;

    /**
     * @brief serQueue This Queue holds all commands we want to send to the
     * device
     */
    SerialQueue serQueue;

    QString serialPortName;
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
    processStatusCommands(std::shared_ptr<PowerSupplyStatus> &status,
                          const std::shared_ptr<SerialCommand> &com) = 0;
};

#endif // POWERSUPPLYSCPI_H
