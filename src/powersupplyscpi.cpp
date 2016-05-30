#include "powersupplyscpi.h"

namespace powcon = PowerSupplySCPI_constants;

PowerSupplySCPI::PowerSupplySCPI(
    QString serialPortName, QByteArray deviceHash, int noOfChannels,
    int voltageAccuracy, int currentAccuracy, QSerialPort::BaudRate brate,
    QSerialPort::FlowControl flowctl, QSerialPort::DataBits dbits,
    QSerialPort::Parity parity, QSerialPort::StopBits sbits, int portTimeOut)
    : QObject(),
      serialPortName(std::move(serialPortName)),
      deviceHash(std::move(deviceHash)),
      noOfChannels(noOfChannels),
      voltageAccuracy(voltageAccuracy),
      currentAccuracy(currentAccuracy),
      port_baudraute(brate),
      port_flowControl(flowctl),
      port_databits(dbits),
      port_parity(parity),
      port_stopbits(sbits),
      portTimeOut(portTimeOut)
{
    this->serialPort = nullptr;
    this->canCalculateWattage = false;

    this->powStatus = std::make_shared<PowerSupplyStatus>();
}

PowerSupplySCPI::~PowerSupplySCPI() {}
void PowerSupplySCPI::startPowerSupplyBackgroundThread()
{
    this->backgroundWorkerThreadRun = true;
    this->threadFunc();
}

void PowerSupplySCPI::stopPowerSupplyBackgroundThread()
{
    this->backgroundWorkerThreadRun = false;
    this->serQueue.push(static_cast<int>(powcon::COMMANDS::SETDUMMY));
}

QString PowerSupplySCPI::getserialPortName() { return this->serialPortName; }
QByteArray PowerSupplySCPI::getDeviceHash() { return this->deviceHash; }
void PowerSupplySCPI::threadFunc()
{
    this->serialPort = new QSerialPort(this->serialPortName);

    // this->serialPort = new QSerialPort(this->serialPortName);
    if (!this->serialPort->open(QIODevice::ReadWrite)) {
        emit errorOpen(this->serialPort->errorString());
        return;
    }

    this->serialPort->setBaudRate(this->port_baudraute);
    this->serialPort->setFlowControl(this->port_flowControl);
    this->serialPort->setDataBits(this->port_databits);
    this->serialPort->setParity(this->port_parity);
    this->serialPort->setStopBits(this->port_stopbits);

    emit deviceOpen();

    while (this->backgroundWorkerThreadRun) {
        this->readWriteData(this->serQueue.pop());
    }

    LogInstance::get_instance().eal_debug("Stopping SCPI worker thread");

    QMutexLocker qlock(&this->qserialPortGuard);
    if (this->serialPort && this->serialPort->isOpen()) {
        this->serialPort->close();
        delete this->serialPort;
    }

    emit backgroundThreadStopped();
}

void PowerSupplySCPI::readWriteData(std::shared_ptr<SerialCommand> com)
{
    // FIXME: There is a race condition if the destructor of a derived class is
    // called because we use several pure virtual methods here.
    // Solutions:
    // 1. Provide basic implementations in base class :(
    // 2. Wait in derived destructor until serialPortGuard mutex is unlocked. This
    // sounds good but every derived class needs to do this. Not convenient and
    // most of all not error prone.

    QMutexLocker qlock(&this->qserialPortGuard);
    std::vector<std::shared_ptr<SerialCommand>> commands = {com};

    if (com->getCommand() == powcon::COMMANDS::SETDUMMY) {
        return;
    }

    std::chrono::high_resolution_clock::time_point tStart =
        std::chrono::high_resolution_clock::now();

    if (com->getCommand() == powcon::GETSTATUS) {
        commands = this->prepareStatusCommands();
    }

    ealogger::Logger &log = LogInstance::get_instance();

    bool serial_error = false;

    for (auto &c : commands) {
        QByteArray commandByte = this->prepareCommandByteArray(c);
        bool waitForBytes = false;
        // Could this be a problem here because there are pending commands?
        if (!this->serialPort->clear(QSerialPort::Direction::AllDirections)) {
            ;
            log.eal_error("Could not clear serial port buffers");
            log.eal_error(
                "Error: " +
                static_cast<QString>(this->serialPort->error()).toStdString());
            this->serialPort->clearError();
        }
        qint64 bytesWritten =
            this->serialPort->write(commandByte, commandByte.length());
        if (bytesWritten != -1) {
            log.eal_debug("Bytes written: " +
                          QString::number(bytesWritten).toStdString() + "\n" +
                          "command length: " +
                          QString::number(commandByte.length()).toStdString());
            // wait for for bytes to be written
            if (commandByte != "") {
                waitForBytes =
                    this->serialPort->waitForBytesWritten(this->portTimeOut);
            }
        } else {
            log.eal_error("Could not write command " +
                          commandByte.toStdString());
            log.eal_error(
                "Error: " +
                static_cast<QString>(this->serialPort->error()).toStdString());
            this->serialPort->clearError();
            serial_error = true;
        }

        if (waitForBytes) {
            // is this a command with feedback?
            if (c->getCommandWithReply()) {
                QByteArray reply = "0";
                if (commandByte != "") {
                    // wait until port is ready to read
                    if (this->serialPort->waitForReadyRead(1000)) {
                        if (serialPort->bytesAvailable())
                            reply.clear();
                        while (serialPort->bytesAvailable()) {
                            reply.append(this->serialPort->readAll());
                            this->serialPort->waitForReadyRead(
                                this->portTimeOut);
                        }
                    } else {
                        log.eal_error("Wait for ready read for command " +
                                      commandByte.toStdString() + " timed out");
                        log.eal_error(
                            "Error: " +
                            static_cast<QString>(this->serialPort->error())
                                .toStdString());
                        this->serialPort->clearError();
                        serial_error = true;
                    }
                    //                    while
                    //                    (this->serialPort->waitForReadyRead(1))
                    //                    {
                    //                        reply +=
                    //                        this->serialPort->readAll();
                    //                    }
                }
                c->setValue(reply);
                this->processCommands(this->powStatus, c);
            }
        } else {
            emit this->errorReadWrite(QString(this->serialPort->error()));
            log.eal_error("Could not read from or write to device: " +
                          QString(this->serialPort->error()).toStdString());
            this->serialPort->clearError();
            serial_error = true;
        }
    }

    if (serial_error) {
        return;
    }

    std::chrono::high_resolution_clock::time_point tEnd =
        std::chrono::high_resolution_clock::now();
    long duration =
        std::chrono::duration_cast<std::chrono::milliseconds>(tEnd - tStart)
            .count();

    if (com->getCommand() == powcon::GETSTATUS) {
        this->powStatus->setDuration(duration);
        this->powStatus->setTime(std::chrono::system_clock::now());
        // calculate wattage
        if (this->canCalculateWattage)
            this->calculateWattage(this->powStatus);

        // seems like we have to emit first, but why?
        emit this->statusReady(this->powStatus);

        std::shared_ptr<PowerSupplyStatus> newStatus =
            std::make_shared<PowerSupplyStatus>();
        this->updateNewPStatus(newStatus);
        this->powStatus = newStatus;
    } else {
        emit this->requestFinished(com);
    }
}
