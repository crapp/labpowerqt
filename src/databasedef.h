// This file is part of labpowerqt, a Gui application to control programmable
// lab power supplies.
// Copyright © 2015, 2016 Christian Rapp <0x2a at posteo dot org>
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

#ifndef DATABASEDEF_H
#define DATABASEDEF_H

#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlError>

#include <QDir>
#include <QFileInfo>

#include <QtDebug>

#include <vector>

namespace database_constants
{

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
}

namespace database_utils
{

namespace dbcon = database_constants;

inline void setDBOptimizations()
{
    QSqlDatabase db = QSqlDatabase::database();
    QSqlQuery("PRAGMA foreign_keys = ON", db);
    QSqlQuery("PRAGMA journal_mode = MEMORY", db);
    QSqlQuery("PRAGMA temp_store = MEMORY", db);
    // TODO: Think about sqlite page and cache size
    QSqlQuery("PRAGMA page_size = 16384", db);
    QSqlQuery("PRAGMA cache_size = 163840", db);
    // TODO: Which locking mechanism should I choose?
    QSqlQuery("PRAGMA locking_mode = UNLOCKED", db);
    QSqlQuery("PRAGMA synchronous = OFF", db);
}

inline void initTables()
{
    std::vector<QSqlQuery> queryVec;
    QSqlDatabase db = QSqlDatabase::database();
    QSqlQuery queryRec(db);
    QSqlQuery queryMes(db);
    QSqlQuery queryCha(db);
    // clang-format off
    queryRec.prepare(QString("CREATE TABLE IF NOT EXISTS ") + dbcon::TBL_RECORDING + " ("
                     + dbcon::TBL_RECORDING_ID + " INTEGER PRIMARY KEY, "
                     + dbcon::TBL_RECORDING_NAME + " TEXT NOT NULL, "
                     + dbcon::TBL_RECORDING_DEVICE + " TEXT NOT NULL, "
                     + dbcon::TBL_RECORDING_PROTO + " INTEGER NOT NULL, "
                     + dbcon::TBL_RECORDING_PORT + " TEXT NOT NULL, "
                     + dbcon::TBL_RECORDING_CHAN + " INTEGER NOT NULL, "
                     + dbcon::TBL_RECORDING_START + " DATETIME NOT NULL, "
                     + dbcon::TBL_RECORDING_STOP + " DATETIME, "
                     + dbcon::TBL_RECORDING_TS + " DATEIME NOT NULL DEFAULT (STRFTIME('%Y-%m-%d %H:%M:%f', 'NOW')))");
    queryMes.prepare(QString("CREATE TABLE IF NOT EXISTS ") + dbcon::TBL_MEASUREMENT + " ("
                     + dbcon::TBL_MEASUREMENT_ID + " INTEGER PRIMARY KEY, "
                     + dbcon::TBL_MEASUREMENT_REC + " INTEGER NOT NULL, "
                     + dbcon::TBL_MEASUREMENT_TRMODE + " INTEGER, "
                     + dbcon::TBL_MEASUREMENT_OCP + " INTEGER, "
                     + dbcon::TBL_MEASUREMENT_OVP + " INTEGER, "
                     + dbcon::TBL_MEASUREMENT_OTP + " INTEGER, "
                     + dbcon::TBL_MEASUREMENT_TIME + " DATETIME NOT NULL, "
                     + dbcon::TBL_MEASUREMENT_TS + " DATEIME NOT NULL DEFAULT (STRFTIME('%Y-%m-%d %H:%M:%f', 'NOW')), "
                     + "FOREIGN KEY (" + dbcon::TBL_MEASUREMENT_REC + ") "
                     + "REFERENCES " + dbcon::TBL_RECORDING + "(" + dbcon::TBL_RECORDING_ID + ") ON DELETE CASCADE)");
    queryCha.prepare(QString("CREATE TABLE IF NOT EXISTS ") + dbcon::TBL_CHANNEL + " ("
                     + dbcon::TBL_CHANNEL_ID + " INTEGER PRIMARY KEY, "
                     + dbcon::TBL_CHANNEL_MES + " INTEGER NOT NULL, "
                     + dbcon::TBL_CHANNEL_CHAN + " INTEGER NOT NULL, "
                     + dbcon::TBL_CHANNEL_OUTPUT + " INTEGER, "
                     + dbcon::TBL_CHANNEL_MODE + " INTEGER, "
                     + dbcon::TBL_CHANNEL_V + " DOUBLE, "
                     + dbcon::TBL_CHANNEL_VS + " DOUBLE, "
                     + dbcon::TBL_CHANNEL_A + " DOUBLE, "
                     + dbcon::TBL_CHANNEL_AS + " DOUBLE, "
                     + dbcon::TBL_CHANNEL_W + " DOUBLE, "
                     + dbcon::TBL_CHANNEL_TS + " DATETIME NOT NULL DEFAULT(STRFTIME('%Y-%m-%d %H:%M:%f', 'NOW')), "
                     + "FOREIGN KEY (" + dbcon::TBL_CHANNEL_MES + ") "
                     + "REFERENCES " + dbcon::TBL_MEASUREMENT + "(" + dbcon::TBL_MEASUREMENT_ID + ") ON DELETE CASCADE)");
    // TODO: Add some indexes? Especially on the foreign key columns.
    // clang-format on
    queryVec.push_back(std::move(queryRec));
    queryVec.push_back(std::move(queryMes));
    queryVec.push_back(std::move(queryCha));
    // TODO: Are transactions supported for DDL?
    db.transaction();
    for (auto &query : queryVec) {
        if (!query.exec()) {
            db.rollback();
            qDebug() << Q_FUNC_INFO << query.lastError().text();
            qDebug() << Q_FUNC_INFO << db.lastError().text();
            return;
        }
    }
    db.commit();
}

inline void initDatabase(QString driver, QString dbFile)
{
    if (!QSqlDatabase::database().isValid()) {
        QSqlDatabase db = QSqlDatabase::addDatabase(driver);
        // make sure the path for the db file exists
        QFileInfo fi(dbFile);
        if (!QDir().mkpath(fi.absolutePath())) {
            qDebug() << Q_FUNC_INFO << "Could not create path for db file: "
                     << fi.absolutePath();
        }
        db.setDatabaseName(dbFile);
        // open creates the sqlite database file
        if (!db.open()) {
            qDebug() << Q_FUNC_INFO << "Can not open Database. Error: "
                     << db.lastError().text();
        } else {
            setDBOptimizations();
            initTables();
        }
    }
}
}

#endif // DATABASEDEF_H
