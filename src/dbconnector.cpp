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

#include "dbconnector.h"

namespace globcon = global_constants;
namespace setcon = settings_constants;

DBConnector::DBConnector()
{
    this->recID = -1;
    QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE");
    QSettings settings;
    settings.beginGroup(setcon::RECORD_GROUP);
    db.setDatabaseName(settings.value(setcon::RECORD_SQLPATH,
                                      QStandardPaths::writableLocation(
                                          QStandardPaths::DataLocation) +
                                          QDir::separator() +
                                          QString("labpowerqt.sqlite"))
                           .toString());
    qDebug() << Q_FUNC_INFO << db.databaseName();
    if (!db.open()) {
        qDebug() << Q_FUNC_INFO
                 << "Can not open Database. Error: " << db.lastError().text();
        // TODO: Emit Database Error
    } else {
        this->dbOptimize();
        this->initDBTables();
    }
}

DBConnector::~DBConnector()
{
    // close the open database
    QSqlDatabase::database().close();
}

void DBConnector::startRecording(QString recName)
{
    QSettings settings;
    settings.beginGroup(setcon::DEVICE_GROUP);
    settings.beginGroup(settings.value(setcon::DEVICE_ACTIVE).toString());
    QSqlDatabase db = QSqlDatabase::database();
    db.transaction();
    QSqlQuery recInsert(db);
    // clang-format off
    recInsert.prepare(QString("INSERT INTO ") + TBL_RECORDING
                      + "(" + TBL_RECORDING_NAME + ", "
                      + TBL_RECORDING_DEVICE + ", "
                      + TBL_RECORDING_PROTO + ", "
                      + TBL_RECORDING_PORT + ", "
                      + TBL_RECORDING_CHAN + ", "
                      + TBL_RECORDING_START + ") VALUES(?, ?, ?, ?, ?, ?)");
    // clang-format on
    recInsert.bindValue(0, recName);
    recInsert.bindValue(1, settings.value(setcon::DEVICE_NAME));
    recInsert.bindValue(2, settings.value(setcon::DEVICE_PROTOCOL));
    recInsert.bindValue(3, settings.value(setcon::DEVICE_PORT));
    recInsert.bindValue(4, settings.value(setcon::DEVICE_CHANNELS));
    recInsert.bindValue(5, QDateTime::currentDateTime());
    if (recInsert.exec()) {
        db.commit();
    } else {
        db.rollback();
        // TODO: Error handling and not only here
        this->recID = -1;
        qDebug() << Q_FUNC_INFO << recInsert.lastError().text();
        qDebug() << Q_FUNC_INFO << db.lastError().text();
        return;
    }
    this->recID = this->maxID(TBL_RECORDING, TBL_RECORDING_ID);
}

void DBConnector::stopRecording()
{
    QSqlDatabase db = QSqlDatabase::database();
    db.transaction();
    QSqlQuery recUpdate(db);
    // clang-format off
    recUpdate.prepare(QString("UPDATE ") + TBL_RECORDING + " SET "
                      + TBL_RECORDING_STOP + " = ? WHERE "
                      + TBL_RECORDING_ID + " = ? AND "
                      + TBL_RECORDING_STOP + " IS NULL");
    // clang-format on
    recUpdate.bindValue(0, QDateTime::currentDateTime());
    recUpdate.bindValue(1, QVariant(this->recID));
    if (recUpdate.exec()) {
        db.commit();
    } else {
        db.rollback();
        qDebug() << Q_FUNC_INFO << recUpdate.lastError().text();
        qDebug() << Q_FUNC_INFO << db.lastError().text();
    }
}

void DBConnector::insertMeasurement(
    std::vector<std::shared_ptr<PowerSupplyStatus>> statusBuffer)
{
    // should we run this in a separate thread? If the user sets the buffer size
    // very high we could get into trouble here with a non responsive ui.
    QSqlDatabase db = QSqlDatabase::database();
    db.transaction();
    for (const auto &status : statusBuffer) {
        this->insertMeasurement(status);
    }
    if (!db.commit()) {
        qDebug() << Q_FUNC_INFO << db.lastError().text();
        db.rollback();
    }
}

void DBConnector::insertMeasurement(std::shared_ptr<PowerSupplyStatus> powStatus)
{
    if (this->recID == -1)
        return;
    QSettings settings;
    settings.beginGroup(setcon::DEVICE_GROUP);
    settings.beginGroup(settings.value(setcon::DEVICE_ACTIVE).toString());
    QSqlDatabase db = QSqlDatabase::database();
    QSqlQuery insertQueryMeasurement(db);
    QSqlQuery insertQueryChannel(db);
    // clang-format off
    insertQueryMeasurement.prepare(QString("INSERT INTO ") + TBL_MEASUREMENT
                                   + " (" + TBL_MEASUREMENT_REC + ", "
                                   + TBL_MEASUREMENT_TRMODE + ", "
                                   + TBL_MEASUREMENT_OCP + ", "
                                   + TBL_MEASUREMENT_OVP + ", "
                                   + TBL_MEASUREMENT_OTP + ", "
                                   + TBL_MEASUREMENT_TIME + ") VALUES(?, ?, ?, ?, ?, ?)");
    insertQueryChannel.prepare(QString("INSERT INTO ") + TBL_CHANNEL
                               + " (" + TBL_CHANNEL_MES + ", "
                               + TBL_CHANNEL_CHAN + ", "
                               + TBL_CHANNEL_OUTPUT + ", "
                               + TBL_CHANNEL_MODE + ", "
                               + TBL_CHANNEL_V + ", "
                               + TBL_CHANNEL_VS + ", "
                               + TBL_CHANNEL_A + ", "
                               + TBL_CHANNEL_AS + ", "
                               + TBL_CHANNEL_W + ") VALUES(?, ?, ?, ?, ?, ?, ?, ?, ?)");
    // clang-format on
    insertQueryMeasurement.bindValue(0, this->recID);
    insertQueryMeasurement.bindValue(1, QVariant()); // TODO: Not implemented yet
    insertQueryMeasurement.bindValue(2, powStatus->getOcp());
    insertQueryMeasurement.bindValue(3, powStatus->getOvp());
    insertQueryMeasurement.bindValue(4, powStatus->getOtp());
    auto duration = powStatus->getTime().time_since_epoch();
    insertQueryMeasurement.bindValue(
        5, QDateTime::fromMSecsSinceEpoch(
               std::chrono::duration_cast<std::chrono::milliseconds>(duration)
                   .count()));
    if (!insertQueryMeasurement.exec()) {
        qDebug() << Q_FUNC_INFO << insertQueryMeasurement.lastError().text();
        qDebug() << Q_FUNC_INFO << db.lastError().text();
    }

    long long maxIDMeasurement =
        this->maxID(TBL_MEASUREMENT, TBL_MEASUREMENT_ID);
    if (maxIDMeasurement == -1)
        return;
    insertQueryChannel.bindValue(0, maxIDMeasurement);
    for (int channel = 1;
         channel <= settings.value(setcon::DEVICE_CHANNELS).toInt(); channel++) {
        insertQueryChannel.bindValue(1, channel);
        insertQueryChannel.bindValue(2, powStatus->getChannelOutput(channel));
        insertQueryChannel.bindValue(3, powStatus->getChannelMode(channel));
        insertQueryChannel.bindValue(4, powStatus->getVoltage(channel));
        insertQueryChannel.bindValue(5, powStatus->getVoltageSet(channel));
        insertQueryChannel.bindValue(6, powStatus->getCurrent(channel));
        insertQueryChannel.bindValue(7, powStatus->getCurrentSet(channel));
        insertQueryChannel.bindValue(8, powStatus->getWattage(channel));
        if (!insertQueryChannel.exec()) {
            qDebug() << Q_FUNC_INFO << insertQueryChannel.lastError().text();
            qDebug() << Q_FUNC_INFO << db.lastError().text();
        }
    }
}

void DBConnector::dbOptimize()
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

void DBConnector::initDBTables()
{
    std::vector<QSqlQuery> queryVec;
    QSqlDatabase db = QSqlDatabase::database();
    QSqlQuery queryRec(db);
    QSqlQuery queryMes(db);
    QSqlQuery queryCha(db);
    // clang-format off
    queryRec.prepare(QString("CREATE TABLE IF NOT EXISTS ") + TBL_RECORDING + " ("
                     + TBL_RECORDING_ID + " INTEGER PRIMARY KEY, "
                     + TBL_RECORDING_NAME + " TEXT NOT NULL, "
                     + TBL_RECORDING_DEVICE + " TEXT NOT NULL, "
                     + TBL_RECORDING_PROTO + " INTEGER NOT NULL, "
                     + TBL_RECORDING_PORT + " TEXT NOT NULL, "
                     + TBL_RECORDING_CHAN + " INTEGER NOT NULL, "
                     + TBL_RECORDING_START + " DATETIME NOT NULL, "
                     + TBL_RECORDING_STOP + " DATETIME, "
                     + TBL_RECORDING_TS + " DATEIME NOT NULL DEFAULT (STRFTIME('%Y-%m-%d %H:%M:%f', 'NOW')))");
    queryMes.prepare(QString("CREATE TABLE IF NOT EXISTS ") + TBL_MEASUREMENT + " ("
                     + TBL_MEASUREMENT_ID + " INTEGER PRIMARY KEY, "
                     + TBL_MEASUREMENT_REC + " INTEGER NOT NULL, "
                     + TBL_MEASUREMENT_TRMODE + " INTEGER, "
                     + TBL_MEASUREMENT_OCP + " INTEGER, "
                     + TBL_MEASUREMENT_OVP + " INTEGER, "
                     + TBL_MEASUREMENT_OTP + " INTEGER, "
                     + TBL_MEASUREMENT_TIME + " DATETIME NOT NULL, "
                     + TBL_MEASUREMENT_TS + " DATEIME NOT NULL DEFAULT (STRFTIME('%Y-%m-%d %H:%M:%f', 'NOW')), "
                     + "FOREIGN KEY (" + TBL_MEASUREMENT_REC + ") "
                     + "REFERENCES " + TBL_RECORDING + "(" + TBL_RECORDING_ID + ") ON DELETE CASCADE)");
    queryCha.prepare(QString("CREATE TABLE IF NOT EXISTS ") + TBL_CHANNEL + " ("
                     + TBL_CHANNEL_ID + " INTEGER PRIMARY KEY, "
                     + TBL_CHANNEL_MES + " INTEGER NOT NULL, "
                     + TBL_CHANNEL_CHAN + " INTEGER NOT NULL, "
                     + TBL_CHANNEL_OUTPUT + " INTEGER, "
                     + TBL_CHANNEL_MODE + " INTEGER, "
                     + TBL_CHANNEL_V + " DOUBLE, "
                     + TBL_CHANNEL_VS + " DOUBLE, "
                     + TBL_CHANNEL_A + " DOUBLE, "
                     + TBL_CHANNEL_AS + " DOUBLE, "
                     + TBL_CHANNEL_W + " DOUBLE, "
                     + TBL_CHANNEL_TS + " DATETIME NOT NULL DEFAULT(STRFTIME('%Y-%m-%d %H:%M:%f', 'NOW')), "
                     + "FOREIGN KEY (" + TBL_CHANNEL_MES + ") "
                     + "REFERENCES " + TBL_MEASUREMENT + "(" + TBL_MEASUREMENT_ID + ") ON DELETE CASCADE)");
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

long long DBConnector::maxID(const QString &table, const QString &id)
{
    QSqlDatabase db = QSqlDatabase::database();
    QSqlQuery maxQuery(db);
    maxQuery.prepare("SELECT MAX(" + id + ") as " + id + " FROM " + table);
    if (maxQuery.exec() && maxQuery.first()) {
        // TODO: This is not safe. No bounds checking. Implicit cast
        return maxQuery.value(maxQuery.record().indexOf(id)).toLongLong();
    } else {
        qDebug() << Q_FUNC_INFO << maxQuery.lastError().text();
        qDebug() << Q_FUNC_INFO << db.lastError().text();
    }
    return -1;
}
