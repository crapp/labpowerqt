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

#include "devicewizardoptions.h"

namespace globcon = global_constants;

DeviceWizardOptions::DeviceWizardOptions(QWidget *parent) : QWizardPage(parent)
{
    this->setTitle(tr("Add Power Supply"));
    this->setSubTitle(
        tr("Choose Device Protocol, Specifications and Communication Port"));
    this->setPixmap(QWizard::LogoPixmap, QPixmap(":/icons/device32.png"));

    this->setLayout(new QVBoxLayout());

    this->protocolBox();
    this->specBox();
    this->comBox();
}

void DeviceWizardOptions::initializePage()
{
    this->comPort->clear();
    // get the serial ports and add them as items to the QComboBox.
    for (const QSerialPortInfo &port : QSerialPortInfo::availablePorts()) {
        this->comPort->addItem(port.portName());
        qDebug() << "Info: " << port.portName();
        qDebug() << "Desc " << port.description();
        qDebug() << "Manu: " << port.manufacturer();
    }
}

QString DeviceWizardOptions::getProtoName() const
{
    return this->protoCombo->currentText();
}

void DeviceWizardOptions::protocolBox()
{
    QGroupBox *protoBox = new QGroupBox();
    protoBox->setTitle("Protocol");
    protoBox->setLayout(new QVBoxLayout());
    this->layout()->addWidget(protoBox);

    this->protoCombo = new QComboBox;
    this->protoCombo->addItem("Korad SCPI V2", globcon::PROTOCOL::KORADV2);
    this->protoCombo->setCurrentIndex(0);

    protoBox->layout()->addWidget(this->protoCombo);

    registerField("protocol", this->protoCombo);
    registerField("protocolText", this, "protocolName");
}

void DeviceWizardOptions::specBox()
{
    QGroupBox *gbSpecs = new QGroupBox();
    gbSpecs->setTitle("Specifications");
    QGridLayout *layout = new QGridLayout();
    gbSpecs->setLayout(layout);
    this->layout()->addWidget(gbSpecs);

    QLabel *channelLabel = new QLabel("Number of channels");
    layout->addWidget(channelLabel, 0, 0);
    QSpinBox *channels = new QSpinBox();
    channels->setMinimum(1);
    channels->setMaximum(2);
    channels->setValue(1);
    layout->addWidget(channels, 1, 0);

    QLabel *voltageLabel = new QLabel("Voltage Range");
    layout->addWidget(voltageLabel, 2, 0, 1, 2);
    QLabel *accLabel = new QLabel("Read back Accuracy");
    layout->addWidget(accLabel, 2, 2);

    QDoubleSpinBox *voltLow = new QDoubleSpinBox();
    voltLow->setMinimum(-50);
    voltLow->setMaximum(100);
    voltLow->setDecimals(1);
    voltLow->setSingleStep(0.1);
    voltLow->setValue(0.0);
    voltLow->setSuffix("V");
    layout->addWidget(voltLow, 1, 0);
    QDoubleSpinBox *voltHigh = new QDoubleSpinBox();
    voltHigh->setMinimum(0);
    voltHigh->setMaximum(100);
    voltHigh->setDecimals(1);
    voltHigh->setSingleStep(0.1);
    voltHigh->setValue(31.0);
    voltHigh->setSuffix("V");
    layout->addWidget(voltHigh, 1, 1);
    QComboBox *voltAccCombo = new QComboBox();
    voltAccCombo->addItems({"1V", "100mV", "10mV", "1mV"});
    voltAccCombo->setCurrentIndex(2); // 10mV default
    layout->addWidget(voltAccCombo, 1, 2);

    QLabel *currentLabel = new QLabel("Current Range");
    layout->addWidget(currentLabel, 3, 0, 1, 2);
    QDoubleSpinBox *currentLow = new QDoubleSpinBox();
    currentLow->setMinimum(0);
    currentLow->setMaximum(10);
    currentLow->setDecimals(1);
    currentLow->setSingleStep(0.1);
    currentLow->setValue(0.0);
    currentLow->setSuffix("A");
    layout->addWidget(currentLow, 3, 0);
    QDoubleSpinBox *currentHigh = new QDoubleSpinBox();
    currentHigh->setMinimum(0);
    currentHigh->setMaximum(10);
    currentHigh->setDecimals(1);
    currentHigh->setSingleStep(0.1);
    currentHigh->setValue(5.1);
    currentHigh->setSuffix("A");
    layout->addWidget(currentHigh, 3, 1);
    QComboBox *currentAccCombo = new QComboBox();
    currentAccCombo->addItems({"1A", "100mA", "10mA", "1mA"});
    currentAccCombo->setCurrentIndex(3); // 1mA default
    layout->addWidget(currentAccCombo, 3, 2);

    registerField("channel", channels);
    registerField("voltLow", voltLow, "value", "valueChanged");
    registerField("voltHigh", voltHigh, "value", "valueChanged");
    registerField("voltAcc", voltAccCombo);
    registerField("currentLow", currentLow, "value", "valueChanged");
    registerField("currentHigh", currentHigh, "value", "valueChanged");
    registerField("currentAcc", currentAccCombo);
}

void DeviceWizardOptions::comBox()
{
    QGroupBox *gbCom = new QGroupBox();
    gbCom->setTitle("Communication");
    gbCom->setLayout(new QVBoxLayout());
    this->layout()->addWidget(gbCom);

    this->comPort = new QComboBox();

    this->comPort->setToolTip(
        "Choose the port to which your device is connected");
    gbCom->layout()->addWidget(this->comPort);

    // we need the comport string not index
    registerField("comPort", this->comPort, "currentText");
}
