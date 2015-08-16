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

class LabPowerController : public QObject
{
    Q_OBJECT
public:
    LabPowerController();

signals:

public slots:
    void setupPowerSupplyConnector();
    void freeSerialPort();

    void setVoltage(double value);
    void readSetVoltage();
    void readCurrentVoltage();

    void getIdentification();

    void dataReceived(std::shared_ptr<SerialCommand> com);

private:
    std::unique_ptr<KoradSCPI> powerSupplyConnector;
};

#endif // LABPOWERCONTROLLER_H
