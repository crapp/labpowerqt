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
