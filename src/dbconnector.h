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

#ifndef DBCONNECTOR_H
#define DBCONNECTOR_H

#include <QObject>

#include <QSqlDatabase>
#include <QSqlError>
#include <QSqlQuery>
#include <QSqlRecord>

#include <QDateTime>

#include <QDir>
#include <QSettings>
#include <QStandardPaths>

#include <chrono>
#include <memory>
#include <vector>

#include "databasedef.h"
#include "global.h"
#include "log_instance.h"
#include "powersupplystatus.h"
#include "settingsdefinitions.h"

/**
 * @brief Connector class between the application controller and the database
 *
 * @details
 *
 * Defines some essential helper functions
 */
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
    long long recID;

    long long maxID(const QString &table, const QString &id);
};

#endif  // DBCONNECTOR_H
