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

#include "devicewizardfinal.h"

namespace setcon = settings_constants;

DeviceWizardFinal::DeviceWizardFinal(QWidget *parent) : QWizardPage(parent)
{
    this->setTitle(tr("Add Power Supply"));
    this->setSubTitle(
        tr("Set a device name and press Finish to add the new Power Supply"));
    this->setPixmap(QWizard::LogoPixmap, QPixmap(":/icons/device32.png"));

    this->setLayout(new QVBoxLayout());

    this->deviceName = new QLineEdit();
    this->deviceName->setPlaceholderText("Enter a Device Name");
    QObject::connect(this->deviceName, &QLineEdit::textChanged,
                     [this]() { this->checkDeviceName(); });
    this->layout()->addWidget(this->deviceName);

    this->summary = new QLabel();
    this->summary->setWordWrap(true);
    this->layout()->addWidget(this->summary);

    registerField("deviceName", this->deviceName);
}

void DeviceWizardFinal::initializePage()
{
    this->nameOkay = false;
    this->deviceName->setText(field("deviceIdentification").toString());
    QString summaryText;
    summaryText += "Protocol: " + field("protocolText").toString() + "\n";
    summaryText += "COM Port: " + field("comPort").toString() + "\n";
    this->summary->setText(summaryText);
    this->checkDeviceName();
}

bool DeviceWizardFinal::isComplete() const { return this->nameOkay; }

void DeviceWizardFinal::checkDeviceName()
{
    if (this->deviceName->text() == "") {
        this->deviceName->setToolTip(
            "Invalid Device Name. Device Name is an empty string");
        this->nameOkay = false;
        emit this->completeChanged();
        return;
    }

    this->settings.beginGroup(setcon::DEVICE_GROUP);
    QStringList devices = this->settings.childGroups();
    for (const auto &d : devices) {
        if (d == this->deviceName->text()) {
            this->nameOkay = false;
            emit this->completeChanged();
            this->deviceName->setStyleSheet("QLineEdit {color: red;}");
            this->deviceName->setToolTip(
                "Invalid Device Name. The Device Name \"" + d +
                "\" is already in use");
            this->settings.endGroup();
            return;
        }
    }
    this->settings.endGroup();
    this->deviceName->setStyleSheet(styleSheet());
    this->deviceName->setToolTip("");
    this->nameOkay = true;
    emit this->completeChanged();
}
