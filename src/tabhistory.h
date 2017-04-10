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

#ifndef TABHISTORY_H
#define TABHISTORY_H

#include <QAction>
#include <QGridLayout>
#include <QHBoxLayout>
#include <QHeaderView>
#include <QLabel>
#include <QTableView>
#include <QToolBar>
#include <QVBoxLayout>
#include <QWidget>

#include <QSqlDatabase>
#include <QSqlRecord>
#include <QSqlTableModel>

#include <QSettings>

#include <QFileDialog>
#include <QMessageBox>

#include <QDir>
#include <QFile>
#include <QStandardPaths>
#include <QTextStream>

#include <memory>

#include "databasedef.h"
#include "log_instance.h"
#include "settingsdefinitions.h"

#include "recordsqlmodel.h"

/**
 * @brief Base widget for the Recordings History tab
 */
class TabHistory : public QWidget
{
    Q_OBJECT
public:
    explicit TabHistory(QWidget *parent = 0);

signals:

public slots:

    void updateModel();

private slots:

    void indexChanged(const QModelIndex &current, const QModelIndex &previous);
    void toolBarAction(QAction *action);

private:
    QGridLayout *lay;
    QToolBar *tbar;
    QAction *actionDelete;
    QAction *actionExport;
    std::unique_ptr<QSqlTableModel> tblModel;
    QTableView *tblView;

    void setupUI();
    void setupConnections();

    void deleteRecordings();
    void exportToCsv();
};

#endif  // TABHISTORY_H
