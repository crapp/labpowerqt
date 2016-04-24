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

#include "recordsqlmodel.h"

RecordSqlModel::RecordSqlModel(QObject *parent) : QSqlTableModel(parent) {}
QVariant RecordSqlModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid())
        return QModelIndex();

    // get the actual value from base class
    QVariant value = QSqlTableModel::data(index, role);

    // do adjustments if necessary
    if (role == Qt::DisplayRole &&
        (index.column() == 6 || index.column() == 7)) {
        QDateTime dt = value.toDateTime();
        value = dt.toString("yyyy-MM-dd HH:mm:ss");
    }

    return value;
}

Qt::ItemFlags RecordSqlModel::flags(const QModelIndex &index) const
{
    Qt::ItemFlags f = QSqlTableModel::flags(index);
    // Only the name column is editable
    if (index.column() == 1) {
        f |= Qt::ItemIsEditable;
    } else {
        f &= ~Qt::ItemIsEditable;
    }
    return f;
}
