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
#include <QStandardPaths>

#include "mainwindow.h"

int main(int argc, char *argv[])
{
    QApplication labpowerqt(argc, argv);

    // set some informations for this application, especially usefull for
    // QSettings
    QCoreApplication::setOrganizationName("crappbytes");
    QCoreApplication::setOrganizationDomain("crappbytes.org");
    QCoreApplication::setApplicationName("labpowerqt");

    MainWindow mw;
    mw.show();

    return labpowerqt.exec();
}
