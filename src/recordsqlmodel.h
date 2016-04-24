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

#ifndef RECORDSQLMODEL_H
#define RECORDSQLMODEL_H

#include <QObject>
#include <QSqlTableModel>

#include <QDateTime>

/**
 * @brief Model for the Record TableView
 *
 * @details
 * Labpowerqt needs its own model class so only some columns in the TableView
 * can be edited and to format the recording date in a special date/time format
 */
class RecordSqlModel : public QSqlTableModel
{
    Q_OBJECT
public:
    explicit RecordSqlModel(QObject *parent = 0);

    /**
     * @brief Return the data for all columns
     *
     * @param index
     * @param role
     *
     * @return
     *
     * @details
     * Will format record date with a special date/time format
     */
    QVariant data(const QModelIndex &index,
                  int role = Qt::DisplayRole) const Q_DECL_OVERRIDE;

private:
    /**
     * @brief Used to determine which columns are editable
     *
     * @param index
     *
     * @return
     */
    Qt::ItemFlags flags(const QModelIndex &index) const Q_DECL_OVERRIDE;
};

#endif  // RECORDSQLMODEL_H
