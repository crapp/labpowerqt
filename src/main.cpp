// labpowerqt is a Gui application to control programmable lab power supplies
// Copyright © 2015, 2016 Christian Rapp <0x2a@posteo.org>

// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.

// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.

// You should have received a copy of the GNU General Public License
// along with this program.  If not, see <http://www.gnu.org/licenses/>.

#include <QApplication>
#include <QDir>
#include <QFileInfoList>
#include <QStandardPaths>
#include <QString>
#include <QTextStream>
#include <QMessageBox>

#include "log_instance.h"
#include "settingsdefinitions.h"

#include "mainwindow.h"

int main(int argc, char *argv[])
{
    namespace setcon = settings_constants;
    namespace setdef = settings_default;
    QApplication labpowerqt(argc, argv);

    // set some informations for this application, especially usefull for
    // QSettings
    QCoreApplication::setOrganizationName("crappbytes");
    QCoreApplication::setOrganizationDomain("crappbytes.org");
    QCoreApplication::setApplicationName("labpowerqt");

    ealogger::Logger &log = LogInstance::get_instance();
#ifdef WIN32
	QSettings::setDefaultFormat(QSettings::Format::IniFormat);
#endif
    QSettings settings;
    settings.beginGroup(setcon::LOG_GROUP);
    if (settings.value(setcon::LOG_ENABLED,
                       setdef::general_defaults.at(setcon::LOG_ENABLED))
            .toBool()) {
        ealogger::constants::LOG_LEVEL lvl =
            static_cast<ealogger::constants::LOG_LEVEL>(
                settings.value(setcon::LOG_MIN_SEVERITY,
                               setdef::general_defaults.at(
                                   setcon::LOG_MIN_SEVERITY))
                    .toInt());
        // create file name
        QDateTime now = QDateTime::currentDateTime();
        QDir cachedir(
            QStandardPaths::writableLocation(QStandardPaths::CacheLocation));
        if (!cachedir.exists()) {
            if (!QDir().mkpath(cachedir.absolutePath())) {
                QMessageBox box;
                box.setIcon(QMessageBox::Icon::Critical);
                box.setWindowTitle("Logfiles directory");
                box.setText("Could not create log file directory \n " +
                            cachedir.absolutePath());
                box.exec();
            }
        }
        QString logfile =
            settings.value(setcon::LOG_DIRECTORY, cachedir.absolutePath())
                .toString();
        logfile += QString(QDir::separator()) + "labpowerqt_" +
                   now.toString("yyyyMMdd") + ".log";
        log.init_file_sink(
            true, lvl, "%d %s [%f:%l] %m", "%F %T", logfile.toStdString(),
            settings.value(setcon::LOG_FLUSH,
                           setdef::general_defaults.at(setcon::LOG_FLUSH))
                .toBool());

        QString titleString;
        QTextStream titleStream(&titleString, QIODevice::WriteOnly);
        titleStream << "LabPowerQt " << LABPOWERQT_VERSION_MAJOR << "."
                    << LABPOWERQT_VERSION_MINOR << "."
                    << LABPOWERQT_VERSION_PATCH;
        log.eal_info(titleString.toStdString() + " is starting");

        QDir logdir(
            settings.value(setcon::LOG_DIRECTORY, cachedir.absolutePath())
                .toString());
        logdir.setFilter(QDir::Files | QDir::Writable);
        logdir.setSorting(QDir::SortFlag::Name);
        logdir.setNameFilters({"labpowerqt_*.log"});
        QFileInfoList logfilesinfo = logdir.entryInfoList();
        // check how many logfiles we have
        // TODO: Make number of logfiles configurable
        if (logfilesinfo.size() > 5) {
            for (int i = 0; i < logfilesinfo.size() - 5; i++) {
                log.eal_info("Deleting log file " +
                             logfilesinfo.at(i).fileName().toStdString());
                logdir.remove(logfilesinfo.at(i).fileName());
            }
        }
    }

    MainWindow mw;
    mw.show();

    return labpowerqt.exec();
}
