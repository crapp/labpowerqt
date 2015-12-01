// This file is part of labpowerqt, a Gui application to control programmable
// lab power supplies.
// Copyright © 2015 Christian Rapp <0x2a at posteo dot org>
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program.  If not, see <http://www.gnu.org/licenses/>.

#ifndef DBCONNECTOR_H
#define DBCONNECTOR_H

#include <QObject>

#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlRecord>
#include <QSqlError>

#include <QDateTime>

#include <QStandardPaths>
#include <QDir>
#include <QSettings>
#include <QDebug>

#include <vector>
#include <memory>
#include <chrono>

#include "global.h"
#include "settingsdefinitions.h"
#include "powersupplystatus.h"

class DBConnector : public QObject
{

    Q_OBJECT

public:
    DBConnector();
    ~DBConnector();

public slots:

    void startRecording(QString recName);
    void stopRecording();
    void insertMeasurement(
        std::vector<std::shared_ptr<PowerSupplyStatus>> statusBuffer);
    void insertMeasurement(std::shared_ptr<PowerSupplyStatus> powStatus);

private:
    /**
     * @brief Recording infos like Device, start and stop time.
     */
    const char *const TBL_RECORDING = "Recording";
    const char *const TBL_RECORDING_ID = "id";
    const char *const TBL_RECORDING_NAME = "name";
    const char *const TBL_RECORDING_DEVICE = "device";
    const char *const TBL_RECORDING_PROTO = "protocol";
    const char *const TBL_RECORDING_PORT = "port";
    const char *const TBL_RECORDING_CHAN = "channels";
    const char *const TBL_RECORDING_START = "time_start";
    const char *const TBL_RECORDING_STOP = "time_stop";
    const char *const TBL_RECORDING_TS = "timestamp";

    /**
     * @brief Information on Measurement are stored in this table
     *
     * Foreign Key constraint on Recording(id)
     */
    const char *const TBL_MEASUREMENT = "Measurement";
    const char *const TBL_MEASUREMENT_ID = "id";
    const char *const TBL_MEASUREMENT_REC =
        "recording"; /**< Foreign Key Constraint*/
    const char *const TBL_MEASUREMENT_TRMODE = "trmode";
    const char *const TBL_MEASUREMENT_OCP = "ocp";
    const char *const TBL_MEASUREMENT_OVP = "ovp";
    const char *const TBL_MEASUREMENT_OTP = "otp";
    const char *const TBL_MEASUREMENT_TIME = "measure_time";
    const char *const TBL_MEASUREMENT_TS = "timestamp";

    /**
     * @brief Holds the data for all channels.
     *
     * Foreign Key contraint on Measurement(id)
     */
    const char *const TBL_CHANNEL = "Channel";
    const char *const TBL_CHANNEL_ID = "id";
    const char *const TBL_CHANNEL_MES = "measurement";
    const char *const TBL_CHANNEL_CHAN = "channelno";
    const char *const TBL_CHANNEL_OUTPUT = "output";
    const char *const TBL_CHANNEL_MODE = "mode";
    const char *const TBL_CHANNEL_V = "voltage";
    const char *const TBL_CHANNEL_VS = "voltage_set";
    const char *const TBL_CHANNEL_A = "current";
    const char *const TBL_CHANNEL_AS = "current_set";
    const char *const TBL_CHANNEL_W = "wattage";
    const char *const TBL_CHANNEL_TS = "timestamp";

    long long recID;

    void dbOptimize();
    void initDBTables();

    long long maxID(const QString &table, const QString &id);
};

#endif // DBCONNECTOR_H
