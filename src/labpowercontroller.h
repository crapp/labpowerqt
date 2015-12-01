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
#include "dbconnector.h"

class LabPowerController : public QObject
{
    Q_OBJECT
public:
    // TODO: Why don't we use in the MainWindow class. Would be much better to
    // have the model only here and notify the MainWindow when the model has
    // changed. At least it would be more consistent
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
    void setVoltage(int channel, double value);
    void setCurrent(int channel, double value);
    void setOutput(int channel, bool status);
    void setAudio(bool status);
    void setLock(bool status);
    void setTrackingMode(int mode);
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

    void toggleRecording(bool status, QString rname);

private:
    std::unique_ptr<PowerSupplySCPI> powerSupplyConnector;
    std::shared_ptr<LabPowerModel> applicationModel;
    std::unique_ptr<DBConnector> dbConnector;

    std::unique_ptr<QTimer> powerSupplyStatusUpdater;
};

#endif // LABPOWERCONTROLLER_H
