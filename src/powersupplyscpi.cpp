#include "powersupplyscpi.h"

namespace powcon = PowerSupplySCPI_constants;

PowerSupplySCPI::PowerSupplySCPI(
    QString serialPortName, QByteArray deviceHash, int noOfChannels,
    int voltageAccuracy, int currentAccuracy, QSerialPort::BaudRate brate,
    QSerialPort::FlowControl flowctl, QSerialPort::DataBits dbits,
    QSerialPort::Parity parity, QSerialPort::StopBits sbits, int portTimeOut,
    QObject *parent)
    : serialPortName(std::move(serialPortName)),
      deviceHash(std::move(deviceHash)), noOfChannels(noOfChannels),
      voltageAccuracy(voltageAccuracy), currentAccuracy(currentAccuracy),
      port_baudraute(brate), port_flowControl(flowctl), port_databits(dbits),
      port_parity(parity), port_stopbits(sbits), portTimeOut(portTimeOut),
      QObject(parent)
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

    qDebug() << Q_FUNC_INFO << "Background thread stopped";

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
    // Solutions: 1. Provide basic implementations in base class :(
    // 2. Wait in derived destructor until serialPortGuard is available. This
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

    // TODO: The Timeout values for serial port operations are critical. Maybe
    // they need to be configurable
    for (auto &c : commands) {
        QByteArray commandByte = this->prepareCommandByteArray(c);
        bool waitForBytes = true;
        this->serialPort->clear(QSerialPort::Direction::AllDirections);
        if (this->serialPort->write(commandByte, commandByte.length()) != -1) {
            // wait for for bytes to be written
            if (commandByte != "") {
                waitForBytes =
                    this->serialPort->waitForBytesWritten(this->portTimeOut);
                // this->thread()->msleep(10);
            }
        }

        if (waitForBytes) {
            // is this command with a feedback?
            if (c->getCommandWithReply()) {
                QByteArray reply = "0";
                if (commandByte != "") {
                    // wait until port is ready to read
                    this->serialPort->waitForReadyRead(2000);
                    if (serialPort->bytesAvailable())
                        reply.clear();
                    while (serialPort->bytesAvailable()) {
                        reply.append(this->serialPort->readAll());
                        this->serialPort->waitForReadyRead(this->portTimeOut);
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
            qDebug() << Q_FUNC_INFO
                     << "Could not write to serial port. Error number: "
                     << this->serialPort->error();
            ;
            // TODO emit an error
        }
    }

    std::chrono::high_resolution_clock::time_point tEnd =
        std::chrono::high_resolution_clock::now();
    long duration =
        std::chrono::duration_cast<std::chrono::milliseconds>(tEnd - tStart)
            .count();
    qDebug() << Q_FUNC_INFO << "Elapsed time for serial command(s): " << duration
             << "ms";

    if (com->getCommand() == powcon::GETSTATUS) {
        this->powStatus->setDuration(duration);
        this->powStatus->setTime(std::move(std::chrono::system_clock::now()));
        // calculate wattage
        if (this->canCalculateWattage)
            this->calculateWattage(this->powStatus);
        emit this->statusReady(this->powStatus);
        std::shared_ptr<PowerSupplyStatus> newStatus =
            std::make_shared<PowerSupplyStatus>();
        this->updateNewPStatus(newStatus);
        this->powStatus = newStatus;
    } else {
        emit this->requestFinished(com);
    }
}
