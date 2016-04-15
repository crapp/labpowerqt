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
    QCryptographicHash hash(QCryptographicHash::Algorithm::Md5);
    settings.setValue(setcon::DEVICE_NAME, field("deviceName").toString());
    hash.addData(settings.value(setcon::DEVICE_NAME).toByteArray());

    settings.setValue(setcon::DEVICE_PROTOCOL, field("protocol").toInt());
    hash.addData(settings.value(setcon::DEVICE_PROTOCOL).toByteArray());

    settings.setValue(setcon::DEVICE_PORT, field("comPort").toString());
    hash.addData(settings.value(setcon::DEVICE_PORT).toByteArray());

    settings.setValue(setcon::DEVICE_PORT_BRATE, field("baudBox").toInt());
    hash.addData(settings.value(setcon::DEVICE_PORT_BRATE).toByteArray());

    settings.setValue(setcon::DEVICE_PORT_FLOW, field("flowctlBox").toInt());
    hash.addData(settings.value(setcon::DEVICE_PORT_FLOW).toByteArray());

    settings.setValue(setcon::DEVICE_PORT_DBITS, field("dbitsBox").toInt());
    hash.addData(settings.value(setcon::DEVICE_PORT_DBITS).toByteArray());

    settings.setValue(setcon::DEVICE_PORT_PARITY, field("parityBox").toInt());
    hash.addData(settings.value(setcon::DEVICE_PORT_PARITY).toByteArray());

    settings.setValue(setcon::DEVICE_PORT_SBITS, field("stopBox").toInt());
    hash.addData(settings.value(setcon::DEVICE_PORT_SBITS).toByteArray());

    settings.setValue(setcon::DEVICE_PORT_TIMEOUT, field("sportTimeout").toInt());
    hash.addData(settings.value(setcon::DEVICE_PORT_TIMEOUT).toByteArray());

    settings.setValue(setcon::DEVICE_CHANNELS, field("channel").toInt());
    hash.addData(settings.value(setcon::DEVICE_CHANNELS).toByteArray());

    settings.setValue(setcon::DEVICE_VOLTAGE_MIN, field("voltLow").toDouble());
    hash.addData(settings.value(setcon::DEVICE_VOLTAGE_MIN).toByteArray());

    settings.setValue(setcon::DEVICE_VOLTAGE_MAX, field("voltHigh").toDouble());
    hash.addData(settings.value(setcon::DEVICE_VOLTAGE_MAX).toByteArray());

    settings.setValue(setcon::DEVICE_VOLTAGE_ACCURACY, field("voltAcc").toInt());
    hash.addData(settings.value(setcon::DEVICE_VOLTAGE_ACCURACY).toByteArray());

    settings.setValue(setcon::DEVICE_POLL_FREQ, field("pollFreqBox").toInt());
    hash.addData(settings.value(setcon::DEVICE_POLL_FREQ).toByteArray());

    settings.setValue(setcon::DEVICE_CURRENT_MIN,
                      field("currentLow").toDouble());
    hash.addData(settings.value(setcon::DEVICE_CURRENT_MIN).toByteArray());

    settings.setValue(setcon::DEVICE_CURRENT_MAX,
                      field("currentHigh").toDouble());
    hash.addData(settings.value(setcon::DEVICE_CURRENT_MAX).toByteArray());

    settings.setValue(setcon::DEVICE_CURRENT_ACCURACY,
                      field("currentAcc").toInt());
    hash.addData(settings.value(setcon::DEVICE_CURRENT_ACCURACY).toByteArray());
    // use a md5 hash to easily determine if the device has changed.
    // TODO: Why don't we use this hash for the active field? Would allow the
    // user to add devices with the same name (if this makes any sense)
    settings.setValue(setcon::DEVICE_HASH, hash.result());
    settings.endGroup();
    settings.setValue(setcon::DEVICE_ACTIVE, field("deviceName").toString());
    settings.endGroup();

    QDialog::accept();
}
