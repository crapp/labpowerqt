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
    // this->connectDevice();
}

LabPowerController::~LabPowerController()
{
    if (this->powerSupplyStatusUpdater)
        this->powerSupplyStatusUpdater->stop();
}

void LabPowerController::connectDevice()
{
    QSettings settings;
    settings.beginGroup(setcon::DEVICE_GROUP);
    if (settings.contains(setcon::DEVICE_PORT)) {
        QString portName = settings.value(setcon::DEVICE_PORT).toString();
        if (!this->powerSupplyConnector ||
            this->powerSupplyConnector->getserialPortName() != portName) {

            this->powerSupplyConnector =
                std::unique_ptr<KoradSCPI>(new KoradSCPI(
                    portName, settings.value(setcon::DEVICE_CHANNELS).toInt(),
                    settings.value(setcon::DEVICE_VOLTAGE_ACCURACY).toInt(),
                    settings.value(setcon::DEVICE_CURRENT_ACCURACY).toInt()));

            QObject::connect(this->powerSupplyConnector.get(),
                             SIGNAL(errorOpen(const QString &)),
                             SLOT(deviceError(const QString &)));
            QObject::connect(this->powerSupplyConnector.get(),
                             SIGNAL(errorReadWrite(const QString &)), this,
                             SLOT(deviceReadWriteError(const QString &)));
            QObject::connect(this->powerSupplyConnector.get(),
                             SIGNAL(deviceOpen()), this,
                             SLOT(deviceConnected()));

            QObject::connect(
                this->powerSupplyConnector.get(),
                SIGNAL(requestFinished(std::shared_ptr<SerialCommand>)), this,
                SLOT(receiveData(std::shared_ptr<SerialCommand>)));
            QObject::connect(
                this->powerSupplyConnector.get(),
                SIGNAL(statusReady(std::shared_ptr<PowerSupplyStatus>)), this,
                SLOT(receiveStatus(std::shared_ptr<PowerSupplyStatus>)));

            QObject::connect(
                this->powerSupplyConnector.get(),
                SIGNAL(statusReady(std::shared_ptr<PowerSupplyStatus>)),
                this->applicationModel.get(),
                SLOT(updatePowerSupplyStatus(
                    std::shared_ptr<PowerSupplyStatus>)));

            this->powerSupplyConnector->startPowerSupplyBackgroundThread();
        }
    } else {
        // TODO: Notify model that no valid power supply is connected.
    }
    settings.endGroup();
}

void LabPowerController::disconnectDevice()
{
    if (this->powerSupplyStatusUpdater)
        this->powerSupplyStatusUpdater->stop();
    if (this->powerSupplyConnector) {
        if (this->powerSupplyStatusUpdater) {
            // this->powerSupplyStatusUpdater->
        }
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

void LabPowerController::setVoltage(const int &channel, const double &value)
{
    if (this->powerSupplyConnector)
        this->powerSupplyConnector->setVoltage(channel, value);
}

void LabPowerController::setCurrent(const int &channel, const double &value)
{
    if (this->powerSupplyConnector)
        this->powerSupplyConnector->setCurrent(channel, value);
}

void LabPowerController::setOutput(const int &channel, const bool &status)
{
    this->powerSupplyConnector->setOutput(channel, status);
}

void LabPowerController::setTrackingMode(const int &mode)
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
    default:
        break;
    }
}

void LabPowerController::receiveStatus(std::shared_ptr<PowerSupplyStatus> status)
{
    qDebug() << Q_FUNC_INFO << "PowerSupply Status: ";
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
             << "Adjusted Voltage1: " << status->getAdjustedVoltage(1);
}
