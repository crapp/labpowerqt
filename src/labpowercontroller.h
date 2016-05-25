#ifndef LABPOWERCONTROLLER_H
#define LABPOWERCONTROLLER_H

#include <memory>
#include <sstream>

#include <QByteArray>
#include <QDebug>
#include <QMessageBox>
#include <QObject>
#include <QSettings>
#include <QString>
#include <QThread>
#include <QTimer>

#include "global.h"
#include "log_instance.h"
#include "settingsdefault.h"
#include "settingsdefinitions.h"

#include "koradscpi.h"
#include "powersupplystatus.h"
#include "serialcommand.h"

#include "dbconnector.h"
#include "labpowermodel.h"

/**
 * @brief The controller class of labpowerqt
 *
 * @details
 *
 * Receives commands from the MainWindow that will be relaiyed to the device
 * Object of type PowerSupplySCPI. The status is send to the model which will
 * in turn notify the GUI so the GUI can be updated. If the record option is on
 * the controller will also use a Database connector object to write the
 * Measurement Buffer to a SQLite Database
 */
class LabPowerController : public QObject
{
    Q_OBJECT
public:
    // TODO: Why don't we use in the MainWindow class. Would be much better to
    // have the model only here and notify the MainWindow when the model has
    // changed. At least it would be more consistent
    // EDIT 2016-04-24: What is this todo about?
    LabPowerController(std::shared_ptr<LabPowerModel> appModel);
    ~LabPowerController();

signals:

public slots:
    // Device connection
    void connectDevice();
    void disconnectDevice();

    // Implement the Power Supply Interface
    void deviceError(const QString &errorString);
    void deviceConnected();
    void deviceReadWriteError(const QString &errorString);
    void setVoltage(int channel, double value);
    void setCurrent(int channel, double value);
    void setOutput(int channel, bool status);
    void setOVP(bool status);
    void setOCP(bool status);
    void setOTP(bool status);
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

    /**
     * @brief Star stop recording of Measurements
     *
     * @param status On/Off
     * @param rname Recording name
     */
    void toggleRecording(bool status, QString rname);

private:
    std::unique_ptr<PowerSupplySCPI> powerSupplyConnector;
    std::shared_ptr<LabPowerModel> applicationModel;
    std::unique_ptr<DBConnector> dbConnector;

    std::unique_ptr<QTimer> powerSupplyStatusUpdater;
    std::unique_ptr<QThread> powerSupplyWorkerThread;
};

#endif  // LABPOWERCONTROLLER_H
