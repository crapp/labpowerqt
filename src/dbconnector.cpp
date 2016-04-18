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

#include "dbconnector.h"

namespace globcon = global_constants;
namespace setcon = settings_constants;
namespace dbcon = database_constants;
namespace dbutil = database_utils;

DBConnector::DBConnector()
{
    this->recID = -1;
    QSettings settings;
    settings.beginGroup(setcon::RECORD_GROUP);
    dbutil::initDatabase("QSQLITE",
                         settings.value(setcon::RECORD_SQLPATH,
                                        QStandardPaths::writableLocation(
                                            QStandardPaths::DataLocation) +
                                            QDir::separator() +
                                            QString("labpowerqt.sqlite"))
                             .toString());

}

DBConnector::~DBConnector()
{
    // stop the recording.
    this->stopRecording();
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
    recInsert.prepare(QString("INSERT INTO ") + dbcon::TBL_RECORDING
                      + "(" + dbcon::TBL_RECORDING_NAME + ", "
                      + dbcon::TBL_RECORDING_DEVICE + ", "
                      + dbcon::TBL_RECORDING_PROTO + ", "
                      + dbcon::TBL_RECORDING_PORT + ", "
                      + dbcon::TBL_RECORDING_CHAN + ", "
                      + dbcon::TBL_RECORDING_START + ") VALUES(?, ?, ?, ?, ?, ?)");
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
    this->recID = this->maxID(dbcon::TBL_RECORDING, dbcon::TBL_RECORDING_ID);
}

void DBConnector::stopRecording()
{
    QSqlDatabase db = QSqlDatabase::database();
    db.transaction();
    QSqlQuery recUpdate(db);
    // clang-format off
    recUpdate.prepare(QString("UPDATE ") + dbcon::TBL_RECORDING + " SET "
                      + dbcon::TBL_RECORDING_STOP + " = ? WHERE "
                      + dbcon::TBL_RECORDING_ID + " = ? AND "
                      + dbcon::TBL_RECORDING_STOP + " IS NULL");
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
    insertQueryMeasurement.prepare(QString("INSERT INTO ") + dbcon::TBL_MEASUREMENT
                                   + " (" + dbcon::TBL_MEASUREMENT_REC + ", "
                                   + dbcon::TBL_MEASUREMENT_TRMODE + ", "
                                   + dbcon::TBL_MEASUREMENT_OCP + ", "
                                   + dbcon::TBL_MEASUREMENT_OVP + ", "
                                   + dbcon::TBL_MEASUREMENT_OTP + ", "
                                   + dbcon::TBL_MEASUREMENT_TIME + ") VALUES(?, ?, ?, ?, ?, ?)");
    insertQueryChannel.prepare(QString("INSERT INTO ") + dbcon::TBL_CHANNEL
                               + " (" + dbcon::TBL_CHANNEL_MES + ", "
                               + dbcon::TBL_CHANNEL_CHAN + ", "
                               + dbcon::TBL_CHANNEL_OUTPUT + ", "
                               + dbcon::TBL_CHANNEL_MODE + ", "
                               + dbcon::TBL_CHANNEL_V + ", "
                               + dbcon::TBL_CHANNEL_VS + ", "
                               + dbcon::TBL_CHANNEL_A + ", "
                               + dbcon::TBL_CHANNEL_AS + ", "
                               + dbcon::TBL_CHANNEL_W + ") VALUES(?, ?, ?, ?, ?, ?, ?, ?, ?)");
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
        this->maxID(dbcon::TBL_MEASUREMENT, dbcon::TBL_MEASUREMENT_ID);
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
