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

#include "devicewizard.h"

namespace setcon = settings_constants;

DeviceWizard::DeviceWizard(QWidget *parent) : QWizard(parent)
{
    this->addPage(new DeviceWizardIntro());
    this->addPage(new DeviceWizardOptions());
    this->addPage(new DeviceWizardConnection());
    this->addPage(new DeviceWizardFinal());

    this->setWindowTitle("Device Wizard");
    this->setWizardStyle(QWizard::WizardStyle::ModernStyle);
}

void DeviceWizard::accept()
{
    // Save the device
    QSettings settings;
    settings.beginGroup(setcon::DEVICE_GROUP);
    settings.beginGroup(field("deviceName").toString());
    settings.setValue(setcon::DEVICE_NAME, field("deviceName").toString());
    settings.setValue(setcon::DEVICE_PROTOCOL, field("protocol").toInt());
    settings.setValue(setcon::DEVICE_PORT, field("comPort").toString());
    settings.setValue(setcon::DEVICE_CHANNELS, field("channel").toInt());
    settings.setValue(setcon::DEVICE_VOLTAGE_MIN, field("voltLow").toDouble());
    settings.setValue(setcon::DEVICE_VOLTAGE_MAX, field("voltHigh").toDouble());
    settings.setValue(setcon::DEVICE_VOLTAGE_ACCURACY, field("voltAcc").toInt());
    settings.setValue(setcon::DEVICE_CURRENT_MIN,
                      field("currentLow").toDouble());
    settings.setValue(setcon::DEVICE_CURRENT_MAX,
                      field("currentHigh").toDouble());
    settings.setValue(setcon::DEVICE_CURRENT_ACCURACY,
                      field("currentAcc").toInt());
    settings.endGroup();
    settings.setValue(setcon::DEVICE_ACTIVE, field("deviceName").toString());
    settings.endGroup();

    QDialog::accept();
}
