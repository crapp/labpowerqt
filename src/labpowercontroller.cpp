#include "labpowercontroller.h"

namespace setcon = settings_constants;
namespace powstatus = PowerSupplyStatus_constants;
namespace powcon = PowerSupplySCPI_constants;
namespace globcon = global_constants;

LabPowerController::LabPowerController(std::shared_ptr<LabPowerModel> appModel)
    : applicationModel(appModel)
{
    this->powerSupplyConnector = nullptr;
    this->powerSupplyStatusUpdater = nullptr;
    this->dbConnector = std::unique_ptr<DBConnector>(new DBConnector());
    // this->connectDevice();
}

LabPowerController::~LabPowerController() { this->disconnectDevice(); }

void LabPowerController::connectDevice()
{
    QSettings settings;
    settings.beginGroup(setcon::DEVICE_GROUP);
    settings.beginGroup(settings.value(setcon::DEVICE_ACTIVE).toString());
    if (settings.contains(setcon::DEVICE_PORT)) {
        QString portName = settings.value(setcon::DEVICE_PORT).toString();
        QSerialPort::BaudRate brate = static_cast<QSerialPort::BaudRate>(
            settings.value(setcon::DEVICE_PORT_BRATE).toInt());
        QSerialPort::FlowControl flowctl = static_cast<QSerialPort::FlowControl>(
            settings.value(setcon::DEVICE_PORT_FLOW).toInt());
        QSerialPort::DataBits dbits = static_cast<QSerialPort::DataBits>(
            settings.value(setcon::DEVICE_PORT_DBITS).toInt());
        QSerialPort::Parity parity = static_cast<QSerialPort::Parity>(
            settings.value(setcon::DEVICE_PORT_PARITY).toInt());
        QSerialPort::StopBits sbits = static_cast<QSerialPort::StopBits>(
            settings.value(setcon::DEVICE_PORT_SBITS).toInt());
        QByteArray deviceHash =
            settings.value(setcon::DEVICE_HASH).toByteArray();
        if (!this->powerSupplyConnector ||
            this->powerSupplyConnector->getDeviceHash() != deviceHash) {
            if (settings.value(setcon::DEVICE_PROTOCOL).toInt() ==
                static_cast<int>(globcon::PROTOCOL::KORADV2)) {
                this->powerSupplyConnector =
                    std::unique_ptr<KoradSCPI>(new KoradSCPI(
                        std::move(portName), std::move(deviceHash),
                        settings.value(setcon::DEVICE_CHANNELS).toInt(),
                        settings.value(setcon::DEVICE_VOLTAGE_ACCURACY).toInt(),
                        settings.value(setcon::DEVICE_CURRENT_ACCURACY).toInt(),
                        brate, flowctl, dbits, parity, sbits));
            }
            QObject::connect(this->powerSupplyConnector.get(),
                             &PowerSupplySCPI::errorOpen, this,
                             &LabPowerController::deviceError);
            QObject::connect(this->powerSupplyConnector.get(),
                             &PowerSupplySCPI::errorReadWrite, this,
                             &LabPowerController::deviceReadWriteError);
            QObject::connect(this->powerSupplyConnector.get(),
                             &PowerSupplySCPI::deviceOpen, this,
                             &LabPowerController::deviceConnected);

            QObject::connect(this->powerSupplyConnector.get(),
                             &PowerSupplySCPI::requestFinished, this,
                             &LabPowerController::receiveData);
            QObject::connect(this->powerSupplyConnector.get(),
                             &PowerSupplySCPI::statusReady, this,
                             &LabPowerController::receiveStatus);

            this->powerSupplyWorkerThread =
                std::unique_ptr<QThread>(new QThread());

            this->powerSupplyConnector->moveToThread(
                this->powerSupplyWorkerThread.get());
            QObject::connect(this->powerSupplyWorkerThread.get(),
                             &QThread::finished, []() {
                                 qDebug() << Q_FUNC_INFO
                                          << "Background Thread Finished Signal";
                             });
            QObject::connect(
                this->powerSupplyConnector.get(),
                &PowerSupplySCPI::backgroundThreadStopped,
                [this]() { this->powerSupplyWorkerThread->quit(); });
            QObject::connect(this->powerSupplyWorkerThread.get(),
                             &QThread::started, this->powerSupplyConnector.get(),
                             &PowerSupplySCPI::startPowerSupplyBackgroundThread);
            this->powerSupplyWorkerThread->start();
        }
    } else {
        // TODO: Notify model that no valid power supply is connected.
    }
}

void LabPowerController::disconnectDevice()
{
    if (this->powerSupplyStatusUpdater)
        this->powerSupplyStatusUpdater->stop();
    if (this->powerSupplyConnector) {
        this->powerSupplyConnector->stopPowerSupplyBackgroundThread();
        this->powerSupplyWorkerThread->wait();
        this->powerSupplyConnector.reset(nullptr);
        this->applicationModel->setDeviceConnected(false);
    }
}

void LabPowerController::deviceError(const QString &errorString)
{
    qDebug() << Q_FUNC_INFO << "Could not open device: " << errorString;
    this->disconnectDevice();
    QMessageBox box;
    box.setIcon(QMessageBox::Icon::Critical);
    box.setText("Could not open Device");
    box.setInformativeText("Error: " + errorString);
    box.exec();
}

void LabPowerController::deviceConnected()
{
    // Tell the model we are connected
    this->applicationModel->setDeviceConnected(true);

    // Check identification
    this->getIdentification();

    // Start our background updater
    this->powerSupplyStatusUpdater = std::unique_ptr<QTimer>(new QTimer());
    QObject::connect(this->powerSupplyStatusUpdater.get(), SIGNAL(timeout()),
                     this, SLOT(getStatus()));
    // TODO: Make the interval configurable. Determine a sane minimum.
    this->powerSupplyStatusUpdater->setInterval(1000);
    this->powerSupplyStatusUpdater->start();
}

void LabPowerController::deviceReadWriteError(const QString &errorString)
{
    qDebug() << Q_FUNC_INFO
             << "Could not open Read/write to device: " << errorString;
}

void LabPowerController::setVoltage(int channel, double value)
{
    if (this->powerSupplyConnector)
        this->powerSupplyConnector->setVoltage(channel, value);
}

void LabPowerController::setCurrent(int channel, double value)
{
    if (this->powerSupplyConnector)
        this->powerSupplyConnector->setCurrent(channel, value);
}

void LabPowerController::setOutput(int channel, bool status)
{
    this->powerSupplyConnector->setOutput(channel, status);
}

void LabPowerController::setOVP(bool status)
{
    this->powerSupplyConnector->setOVP(status);
}

void LabPowerController::setOCP(bool status)
{
    this->powerSupplyConnector->setOCP(status);
}

void LabPowerController::setOTP(bool status)
{
    this->powerSupplyConnector->setOTP(status);
}

void LabPowerController::setAudio(bool status)
{
    this->powerSupplyConnector->setBeep(status);
}

void LabPowerController::setLock(bool status)
{
    this->powerSupplyConnector->setLocked(status);
}

void LabPowerController::setTrackingMode(int mode)
{
    if (this->powerSupplyConnector)
        this->powerSupplyConnector->setTracking(
            static_cast<globcon::TRACKING>(mode));
}

void LabPowerController::getIdentification()
{
    if (this->powerSupplyConnector)
        this->powerSupplyConnector->getIdentification();
}

void LabPowerController::getStatus()
{
    if (this->powerSupplyConnector)
        this->powerSupplyConnector->getStatus();
}

void LabPowerController::receiveData(std::shared_ptr<SerialCommand> com)
{
    qDebug() << Q_FUNC_INFO << "com: " << com->getValue();
    switch (static_cast<powcon::COMMANDS>(com->getCommand())) {
    case powcon::COMMANDS::GETIDN:
        this->applicationModel->setDeviceIdentification(
            com->getValue().toString());
        break;
    default:
        break;
    }
}

void LabPowerController::receiveStatus(std::shared_ptr<PowerSupplyStatus> status)
{
    this->applicationModel->updatePowerSupplyStatus(status);

    if (this->applicationModel->getRecord()) {
        QSettings settings;
        settings.beginGroup(setcon::RECORD_GROUP);
        if (this->applicationModel->getBufferSize() >=
            settings.value(setcon::RECORD_BUFFER, 60).toInt()) {
            this->dbConnector->insertMeasurement(
                this->applicationModel->getBuffer());
            this->applicationModel->clearBuffer();
        }
    }
    /*qDebug() << Q_FUNC_INFO << "PowerSupply Status: ";
    qDebug() << Q_FUNC_INFO << "Beeper: " << status->getBeeper();
    qDebug() << Q_FUNC_INFO << "Locked: " << status->getLocked();
    qDebug() << Q_FUNC_INFO
             << "Channel Output1: " << status->getChannelOutput(1);
    qDebug() << Q_FUNC_INFO << "Channel Mode1: " << status->getChannelMode(1);
    qDebug() << Q_FUNC_INFO
             << "Actual Current1: " << status->getActualCurrent(1);
    qDebug() << Q_FUNC_INFO
             << "Actual Voltage1: " << status->getActualVoltage(1);
    qDebug() << Q_FUNC_INFO
             << "Adjusted Current1: " << status->getAdjustedCurrent(1);
    qDebug() << Q_FUNC_INFO
             << "Adjusted Voltage1: " << status->getAdjustedVoltage(1);*/
}

void LabPowerController::toggleRecording(bool status, QString rname)
{
    this->applicationModel->setRecord(status);
    if (status) {
        this->dbConnector->startRecording(std::move(rname));
    } else {
        this->dbConnector->insertMeasurement(
            this->applicationModel->getBuffer());
        this->applicationModel->clearBuffer();
        this->dbConnector->stopRecording();
    }
}
