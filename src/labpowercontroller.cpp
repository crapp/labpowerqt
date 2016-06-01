#include "labpowercontroller.h"

namespace setcon = settings_constants;
namespace setdef = settings_default;
namespace powstatus = PowerSupplyStatus_constants;
namespace powcon = PowerSupplySCPI_constants;
namespace globcon = global_constants;

LabPowerController::LabPowerController(std::shared_ptr<LabPowerModel> appModel)
    : QObject(), applicationModel(appModel)
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
        int portTimeOut = settings.value(setcon::DEVICE_PORT_TIMEOUT).toInt();
        if (!this->powerSupplyConnector ||
            this->powerSupplyConnector->getDeviceHash() != deviceHash) {
            if (settings.value(setcon::DEVICE_PROTOCOL).toInt() ==
                static_cast<int>(globcon::LPQ_PROTOCOL::KORADV2)) {
                this->powerSupplyConnector =
                    std::unique_ptr<KoradSCPI>(new KoradSCPI(
                        std::move(portName), std::move(deviceHash),
                        settings.value(setcon::DEVICE_CHANNELS).toInt(),
                        settings.value(setcon::DEVICE_VOLTAGE_ACCURACY).toInt(),
                        settings.value(setcon::DEVICE_CURRENT_ACCURACY).toInt(),
                        brate, flowctl, dbits, parity, sbits, portTimeOut));
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
                                 LogInstance::get_instance().eal_debug(
                                     "Background Thread Finished Signal");
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
        if (!this->powerSupplyWorkerThread->wait(3000)) {
            LogInstance::get_instance().eal_warn(
                "Thread Timeout. Will terminate.");
            // TODO: Maybe we should connect a signal to this to notify the user
            // TODO: Why is terminating the thread commented out?
            // this->powerSupplyWorkerThread->terminate();
        }
        this->powerSupplyConnector.reset(nullptr);
        this->applicationModel->setDeviceConnected(false);
    }
}

void LabPowerController::deviceError(const QString &errorString)
{
    LogInstance::get_instance().eal_error("Could not open device: " +
                                          errorString.toStdString());
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

    QSettings settings;
    settings.beginGroup(setcon::DEVICE_GROUP);
    settings.beginGroup(settings.value(setcon::DEVICE_ACTIVE).toString());
    // Get set voltage and set current
    for (int i = 1; i <= settings.value(setcon::DEVICE_CHANNELS).toInt(); i++) {
        this->powerSupplyConnector->getVoltage(i);
        this->powerSupplyConnector->getCurrent(i);
    }
    // TODO: Add other getValue calls here like getOVP, getOCP...

    // Start our background updater
    this->powerSupplyStatusUpdater = std::unique_ptr<QTimer>(new QTimer());
    QObject::connect(this->powerSupplyStatusUpdater.get(), SIGNAL(timeout()),
                     this, SLOT(getStatus()));

    this->powerSupplyStatusUpdater->setInterval(
        settings.value(setcon::DEVICE_POLL_FREQ, 1000).toInt());
    this->powerSupplyStatusUpdater->start();
}

void LabPowerController::deviceReadWriteError(
    ATTR_UNUSED const QString &errorString)
{
    // TODO: This must be propagated to the GUI and the Model
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
            static_cast<globcon::LPQ_TRACKING>(mode));
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
    LogInstance::get_instance().eal_debug(
        "Sending command: " + com->getValue().toString().toStdString());
    switch (static_cast<powcon::COMMANDS>(com->getCommand())) {
    case powcon::COMMANDS::GETIDN:
        this->applicationModel->setDeviceIdentification(
            com->getValue().toString());
        break;
    case powcon::COMMANDS::GETVOLTAGESET:
        this->powerSupplyConnector->setVoltage(com->getPowerSupplyChannel(),
                                               com->getValue().toDouble());
        break;
    case powcon::COMMANDS::GETCURRENTSET:
        this->powerSupplyConnector->setCurrent(com->getPowerSupplyChannel(),
                                               com->getValue().toDouble());
        break;
    default:
        break;
    }
}

void LabPowerController::receiveStatus(std::shared_ptr<PowerSupplyStatus> status)
{
    this->applicationModel->updatePowerSupplyStatus(status);

    // TODO: Wouldn't it be better to let the model signal the DBConnector to
    // fetch the buffer and write them to the database? Why has the controlller
    // to do this?
    if (this->applicationModel->getRecord()) {
        QSettings settings;
        settings.beginGroup(setcon::RECORD_GROUP);
        if (this->applicationModel->getBufferSize() >=
            settings
                .value(setcon::RECORD_BUFFER,
                       setdef::general_defaults.at(setcon::RECORD_BUFFER))
                .toInt()) {
            this->dbConnector->insertMeasurement(
                this->applicationModel->getBuffer());
            this->applicationModel->clearBuffer();
        }
    }
    std::stringstream ss;
    ss << status;
    LogInstance::get_instance().eal_debug(ss.str());
}

void LabPowerController::toggleRecording(bool status, QString rname)
{
    this->applicationModel->setRecord(status);
    if (status) {
        this->dbConnector->startRecording(std::move(rname));
    } else {
        // make sure to write all remaining measurements to the database
        this->dbConnector->insertMeasurement(
            this->applicationModel->getBuffer());
        this->applicationModel->clearBuffer();
        this->dbConnector->stopRecording();
    }
}
