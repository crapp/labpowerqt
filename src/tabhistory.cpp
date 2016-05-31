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
    dbutil::initDatabase(
        "QSQLITE",
        settings
            .value(
                setcon::RECORD_SQLPATH,
                QStandardPaths::writableLocation(QStandardPaths::DataLocation) +
                    QDir::separator() + QString("labpowerqt.sqlite"))
            .toString());

    this->setupUI();
    this->setupConnections();
}

void TabHistory::updateModel()
{
    this->tblModel->select();
    this->tblView->resizeColumnsToContents();
}

void TabHistory::indexChanged(ATTR_UNUSED const QModelIndex &current,
                              ATTR_UNUSED const QModelIndex &previous)
{
}

void TabHistory::toolBarAction(QAction *action)
{
    if (this->tblView->selectionModel()->selectedRows().size() > 0) {
        if (action == this->actionDelete) {
            this->deleteRecordings();
        }
        if (action == this->actionExport) {
            this->exportToCsv();
        }
    }
}

void TabHistory::setupUI()
{
    this->tbar = new QToolBar();
    this->tbar->setFloatable(false);
    this->tbar->setMovable(false);
    this->lay->addWidget(this->tbar, 0, 0);
    this->actionDelete = this->tbar->addAction("Delete");
    this->actionDelete->setIcon(QPixmap(":/icons/trash32.png"));
    this->actionDelete->setToolTip("Delete selected recordings");
    this->actionExport = this->tbar->addAction("Export");
    this->actionExport->setIcon(QPixmap(":/icons/csv32.png"));
    this->actionExport->setToolTip("Export selected recordings to CSV");

    this->tblModel = std::unique_ptr<RecordSqlModel>(new RecordSqlModel());
    this->tblModel->setTable(dbcon::TBL_RECORDING);
    this->tblModel->setEditStrategy(QSqlTableModel::OnFieldChange);
    this->tblModel->select();
    this->tblModel->setHeaderData(1, Qt::Horizontal, tr("Recording"));
    this->tblModel->setHeaderData(2, Qt::Horizontal, tr("Device"));
    this->tblModel->setHeaderData(3, Qt::Horizontal, tr("Protocol"));
    this->tblModel->setHeaderData(4, Qt::Horizontal, tr("Port"));
    this->tblModel->setHeaderData(5, Qt::Horizontal, tr("Channels"));
    this->tblModel->setHeaderData(6, Qt::Horizontal, tr("Beginn"));
    this->tblModel->setHeaderData(7, Qt::Horizontal, tr("End"));

    this->tblView = new QTableView();
    this->tblView->setModel(this->tblModel.get());
    // hide id and timestamp column
    this->tblView->hideColumn(0);
    this->tblView->hideColumn(8);
    this->tblView->horizontalHeader()->setStretchLastSection(true);
    this->tblView->resizeColumnsToContents();
    this->lay->addWidget(this->tblView, 1, 0);
}

void TabHistory::setupConnections()
{
    QObject::connect(this->tbar, &QToolBar::actionTriggered, this,
                     &TabHistory::toolBarAction);

    QObject::connect(this->tblView->selectionModel(),
                     &QItemSelectionModel::currentRowChanged, this,
                     &TabHistory::indexChanged);
}

void TabHistory::deleteRecordings()
{
    int ret = QMessageBox::question(
        this, "Delete Recordings",
        "Are you sure you want to delete the selected Recordings?");
    if (static_cast<QMessageBox::StandardButton>(ret) == QMessageBox::Yes) {
        QModelIndexList selectedRows =
            this->tblView->selectionModel()->selectedRows();
        for (const auto &index : selectedRows) {
            this->tblModel->removeRows(index.row(), 1);
        }
        this->tblModel->select();
    }
}

void TabHistory::exportToCsv()
{
    ealogger::Logger &log = LogInstance::get_instance();
    QModelIndexList selectedRows =
        this->tblView->selectionModel()->selectedRows();
    QString csvFile = QFileDialog::getSaveFileName(
        this, "Export Recordings",
        QStandardPaths::writableLocation(QStandardPaths::HomeLocation),
        "CSV (*.csv)");
    log.eal_info("Exporting data to csv file " + csvFile.toStdString());
    if (csvFile != "") {
        QFile csvf(csvFile);
        csvf.open(QFile::OpenModeFlag::WriteOnly | QFile::OpenModeFlag::Text);
        QTextStream txt(&csvf);
        txt << "Recording;Device;OCP;OVP;OTP;TrackingMode;DateTime;Channel;"
               "Output;Mode;Voltage;VoltageSet;Current;CurrentSet;Wattage"
            << endl;
        for (const auto &index : selectedRows) {
            int recId = this->tblModel->record(index.row())
                            .value(dbcon::TBL_RECORDING_ID)
                            .toInt();
            QString recName = this->tblModel->record(index.row())
                                  .value(dbcon::TBL_RECORDING_NAME)
                                  .toString();
            QString devName = this->tblModel->record(index.row())
                                  .value(dbcon::TBL_RECORDING_DEVICE)
                                  .toString();
            // in fact it is nonsense to select the default connection as this is
            // done by default
            QSqlQuery getMeasurements(QSqlDatabase::database());
            // clang-format off
            if (!getMeasurements.prepare(QString("SELECT m.") + dbcon::TBL_MEASUREMENT_OCP + ", "
                                    + "m." + dbcon::TBL_MEASUREMENT_OVP + ", "
                                    + "m." + dbcon::TBL_MEASUREMENT_OTP + ", "
                                    + "m." + dbcon::TBL_MEASUREMENT_TRMODE + ", "
                                    + "m." + dbcon::TBL_MEASUREMENT_TIME + ", "
                                    + "c." + dbcon::TBL_CHANNEL_CHAN + ", "
                                    + "c." + dbcon::TBL_CHANNEL_OUTPUT + ", "
                                    + "c." + dbcon::TBL_CHANNEL_MODE + ", "
                                    + "c." + dbcon::TBL_CHANNEL_V + ", "
                                    + "c." + dbcon::TBL_CHANNEL_VS + ", "
                                    + "c." + dbcon::TBL_CHANNEL_A + ", "
                                    + "c." + dbcon::TBL_CHANNEL_AS + ", "
                                    + "c." + dbcon::TBL_CHANNEL_W + " \n"
                                    + "FROM " + dbcon::TBL_CHANNEL + " AS c \n"
                                    + "INNER JOIN " + dbcon::TBL_MEASUREMENT + " AS m \n"
                                    + "ON c." + dbcon::TBL_CHANNEL_MES + " = m." + dbcon::TBL_MEASUREMENT_ID + "\n"
                                    + "WHERE m." + dbcon::TBL_MEASUREMENT_REC + " = ?")) {
                log.eal_error(getMeasurements.lastError().text().toStdString());
            }
            log.eal_debug(getMeasurements.executedQuery().toStdString());
            // clang-format on
            getMeasurements.bindValue(0, recId);
            if (getMeasurements.exec()) {
                while (getMeasurements.next()) {
                    txt << "\"" << recName << "\""
                        << ";"
                        << "\"" << devName << "\"";
                    for (int i = 0; i < 14; i++) {
                        txt << ";" << getMeasurements.value(i).toString();
                    }
                    txt << endl;
                }
            } else {
                QMessageBox::critical(this,
                                      "Could not export Recording " + recName,
                                      getMeasurements.lastError().text());
            }
        }
    }
}
