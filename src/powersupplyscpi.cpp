#include "powersupplyscpi.h"

namespace powcon = PowerSupplySCPI_constants;

PowerSupplySCPI::PowerSupplySCPI(QString serialPortName, QObject *parent)
    : serialPortName(serialPortName), QObject(parent)
{
    this->serialPort = nullptr;

    this->backgroundWorkerThreadRun = true;
    backgroundWorkerThread = std::thread(&PowerSupplySCPI::threadFunc, this);
}

PowerSupplySCPI::~PowerSupplySCPI()
{
    this->backgroundWorkerThreadRun = false;
    this->serQueue.push(static_cast<int>(powcon::COMMANDS::SETDUMMY));
    this->backgroundWorkerThread.join();

    delete serialPort;
}

QString PowerSupplySCPI::getserialPortName() { return this->serialPortName; }

void PowerSupplySCPI::threadFunc()
{
    if (!this->serialPort) {
        this->serialPort = new QSerialPort(this->serialPortName);
        if (!this->serialPort->open(QIODevice::ReadWrite)) {
            throw std::runtime_error(
                this->serialPort->errorString().toStdString());
        }

        this->serialPort->setBaudRate(this->BAUDRATE);
        this->serialPort->setFlowControl(this->FLOWCONTROL);
        this->serialPort->setDataBits(this->DATABITS);
        this->serialPort->setParity(this->PARITY);
        this->serialPort->setStopBits(this->STOPBITS);
    }

    while (this->backgroundWorkerThreadRun) {
        this->readWriteData(this->serQueue.pop());
    }

    if (this->serialPort->isOpen()) {
        this->serialPort->close();
    }
}

void PowerSupplySCPI::readWriteData(std::shared_ptr<SerialCommand> com)
{
    std::lock_guard<std::mutex> lock(this->serialPortGuard);
    std::shared_ptr<PowerSupplyStatus> status = nullptr;
    std::vector<std::shared_ptr<SerialCommand>> commands = {com};

    if (com->getCommand() == powcon::COMMANDS::SETDUMMY) {
        return;
    }
    if (com->getCommand() == powcon::GETSTATUS) {
        commands = this->prepareStatusCommands();
        status = std::make_shared<PowerSupplyStatus>();
    }
    for (auto &c : commands) {
        QByteArray commandByte = this->prepareCommand(c);
        this->serialPort->write(commandByte);
        // wait for ouur bytes to be written
        if (this->serialPort->waitForBytesWritten(1000)) {
            // is this command with a feedback?
            if (c->getCommandWithReply()) {
                // wait until port is ready to read
                this->serialPort->waitForReadyRead(1000);
                QByteArray reply = this->serialPort->readAll();
                while (this->serialPort->waitForReadyRead(10)) {
                    reply += this->serialPort->readAll();
                }
                c->setValue(QVariant(reply));

                if (status != nullptr) {
                    this->processStatusCommands(status, c);
                } else {
                    // send our com object with the signal
                    emit this->requestFinished(c);
                }
            }
        } else {
            qDebug() << Q_FUNC_INFO
                     << "Could not write to serial port. Error number: "
                     << this->serialPort->error();
            ;
            // TODO emit an error
        }
    }
    if (status != nullptr) {
        emit this->statusReady(status);
    }
}

std::vector<std::shared_ptr<SerialCommand>>
PowerSupplySCPI::prepareStatusCommands()
{
    std::vector<std::shared_ptr<SerialCommand>> comVec;
    for (const auto &c : this->statusCommands) {
        std::shared_ptr<SerialCommand> com = std::make_shared<SerialCommand>(
            static_cast<int>(c), 1, QVariant(), true);
        comVec.push_back(com);
    }
    return comVec;
}
