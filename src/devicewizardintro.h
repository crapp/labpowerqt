// labpowerqt is a Gui application to control programmable lab power supplies
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

#ifndef DEVICEWIZARDINTRO_H
#define DEVICEWIZARDINTRO_H

#include <QWizardPage>
#include <QWidget>
#include <QVBoxLayout>
#include <QLabel>

/**
 * @brief  Device wizard intro page
 */
class DeviceWizardIntro : public QWizardPage
{
    Q_OBJECT

public:
    DeviceWizardIntro(QWidget *parent = 0);
};

#endif  // DEVICEWIZARDINTRO_H
