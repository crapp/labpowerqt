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

#include "tabhistory.h"

namespace dbcon = database_constants;
namespace dbutil = database_utils;
namespace setcon = settings_constants;

TabHistory::TabHistory(QWidget *parent) : QWidget(parent)
{
    this->lay = new QGridLayout();
    this->setLayout(this->lay);
    QSettings settings;
    settings.beginGroup(setcon::RECORD_GROUP);
    dbutil::initDatabase("QSQLITE",
                         settings.value(setcon::RECORD_SQLPATH,
                                        QStandardPaths::writableLocation(
                                            QStandardPaths::DataLocation) +
                                            QDir::separator() +
                                            QString("labpowerqt.sqlite"))
                             .toString());

    this->setupUI();
}

void TabHistory::updateModel(bool status) {}

void TabHistory::setupUI()
{
    this->tblModel = std::unique_ptr<QSqlTableModel>(new QSqlTableModel());
    this->tblModel->setTable(dbcon::TBL_RECORDING);
    this->tblModel->setEditStrategy(QSqlTableModel::OnManualSubmit);
    this->tblModel->select();
    QSqlDatabase db = this->tblModel->database();
    qDebug() << db.databaseName();
    qDebug() << db.isOpen();
    qDebug() << this->tblModel->rowCount();
//    this->tblModel->setHeaderData(0, Qt::Horizontal, tr("Recording"));
//    this->tblModel->setHeaderData(1, Qt::Horizontal, tr("Start"));
//    this->tblModel->setHeaderData(2, Qt::Horizontal, tr("Stop"));

    this->tblView = new QTableView();
    this->tblView->setModel(this->tblModel.get());
    this->tblView->hideColumn(0);
    this->tblView->horizontalHeader()->setStretchLastSection(true);
    this->tblView->resizeColumnsToContents();
    this->lay->addWidget(this->tblView, 0, 0);
    this->lay->addWidget(new QLabel("blasdasdklnasdnm"), 1, 0);
}
