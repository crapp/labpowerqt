#ifndef LABPOWERCONTROLLER_H
#define LABPOWERCONTROLLER_H

#include <memory>

#include <QObject>
#include <QDebug>
#include <QString>
#include <QByteArray>
#include <QSettings>
#include <QTimer>
#include <QMessageBox>

#include "global.h"
#include "settingsdefinitions.h"
#include "koradscpi.h"
#include "serialcommand.h"
#include "powersupplystatus.h"
#include "labpowermodel.h"

class LabPowerController : public QObject
{
    Q_OBJECT
public:
    LabPowerController(std::shared_ptr<LabPowerModel> appModel);
    ~LabPowerController();

signals:

public slots:
    void connectDevice();
    void disconnectDevice();

    // Implement the Power Supply Interface
    void deviceError(const QString &errorString);
    void deviceConnected();
    void deviceReadWriteError(const QString &errorString);
    void setVoltage(const int &channel, const double &value);
    void setCurrent(const int &channel, const double &value);
    void setOutput(const int &channel, const bool &status);
    void setAudio(const bool &status);
    void setLock(const bool &status);
    void setTrackingMode(const int &mode);
    void getIdentification();
    void getStatus();
    /**
     * @brief receiveData Receive a single Serial Command Object
     * @param com
     */
    void receiveData(std::shared_ptr<SerialCommand> com);
    /**
     * @brief receiveStatus Receive a status object from the power supply connector
     * @param status
     */
    void receiveStatus(std::shared_ptr<PowerSupplyStatus> status);

private:
    std::unique_ptr<PowerSupplySCPI> powerSupplyConnector;
    std::shared_ptr<LabPowerModel> applicationModel;

    std::unique_ptr<QTimer> powerSupplyStatusUpdater;
};

#endif // LABPOWERCONTROLLER_H
