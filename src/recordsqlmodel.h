#ifndef RECORDSQLMODEL_H
#define RECORDSQLMODEL_H

#include <QObject>
#include <QSqlTableModel>

#include <QDateTime>

class RecordSqlModel : public QSqlTableModel
{

    Q_OBJECT
public:
    explicit RecordSqlModel(QObject *parent = 0);

    QVariant data(const QModelIndex &index,
                  int role = Qt::DisplayRole) const Q_DECL_OVERRIDE;

private:
    Qt::ItemFlags flags(const QModelIndex &index) const Q_DECL_OVERRIDE;
};

#endif // RECORDSQLMODEL_H
