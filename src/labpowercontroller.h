#ifndef LABPOWERCONTROLLER_H
#define LABPOWERCONTROLLER_H

#include <memory>

#include <QObject>
#include <QDebug>
#include <QString>
#include <QByteArray>
#include <QSettings>

#include "settingsdefinitions.h"
#include "koradscpi.h"
#include "serialcommand.h"
#include "powersupplystatus.h"

class LabPowerController : public QObject
{
    Q_OBJECT
public:
    LabPowerController();

signals:

public slots:
    void setupPowerSupplyConnector();
    void freeSerialPort();

    // Implement the Power Supply Interface
    void deviceError(const QString &errorString);
    void deviceReadWriteError(const QString &errorString);
    void setVoltage(const double &value);
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
    std::unique_ptr<KoradSCPI> powerSupplyConnector;
};

#endif // LABPOWERCONTROLLER_H
