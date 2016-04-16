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
    layout->addWidget(channels, 1, 0, 1, 3);

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
    layout->addWidget(voltLow, 3, 0);
    QDoubleSpinBox *voltHigh = new QDoubleSpinBox();
    voltHigh->setMinimum(0);
    voltHigh->setMaximum(100);
    voltHigh->setDecimals(1);
    voltHigh->setSingleStep(0.1);
    voltHigh->setValue(31.0);
    voltHigh->setSuffix("V");
    layout->addWidget(voltHigh, 3, 1);
    QComboBox *voltAccCombo = new QComboBox();
    voltAccCombo->addItems({"1V", "100mV", "10mV", "1mV"});
    voltAccCombo->setCurrentIndex(2); // 10mV default
    layout->addWidget(voltAccCombo, 3, 2);

    QLabel *currentLabel = new QLabel("Current Range");
    layout->addWidget(currentLabel, 4, 0, 1, 2);
    QDoubleSpinBox *currentLow = new QDoubleSpinBox();
    currentLow->setMinimum(0);
    currentLow->setMaximum(10);
    currentLow->setDecimals(1);
    currentLow->setSingleStep(0.1);
    currentLow->setValue(0.0);
    currentLow->setSuffix("A");
    layout->addWidget(currentLow, 5, 0);
    QDoubleSpinBox *currentHigh = new QDoubleSpinBox();
    currentHigh->setMinimum(0);
    currentHigh->setMaximum(10);
    currentHigh->setDecimals(1);
    currentHigh->setSingleStep(0.1);
    currentHigh->setValue(5.1);
    currentHigh->setSuffix("A");
    layout->addWidget(currentHigh, 5, 1);
    QComboBox *currentAccCombo = new QComboBox();
    currentAccCombo->addItems({"1A", "100mA", "10mA", "1mA"});
    currentAccCombo->setCurrentIndex(3); // 1mA default
    layout->addWidget(currentAccCombo, 5, 2);

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

    QGridLayout *baudFlowDBits = new QGridLayout();
    dynamic_cast<QVBoxLayout *>(gbCom->layout())->addLayout(baudFlowDBits);

    QLabel *baudLabel = new QLabel("Baud Rate");
    QComboBox *baudBox = new QComboBox();
    baudBox->addItems(
        {"1200", "2400", "4800", "9600", "19200", "38400", "57600", "115200"});
    baudBox->setCurrentText("9600");
    baudFlowDBits->addWidget(baudLabel, 0, 0);
    baudFlowDBits->addWidget(baudBox, 1, 0);

    QLabel *flowctlLabel = new QLabel("Flow Control");
    QComboBox *flowctlBox = new QComboBox();
    flowctlBox->addItems(
        {"No flow control", "Hardware flow control", "Software flow control"});
    flowctlBox->setCurrentIndex(0);
    baudFlowDBits->addWidget(flowctlLabel, 0, 1);
    baudFlowDBits->addWidget(flowctlBox, 1, 1);

    QLabel *dbitsLabel = new QLabel("Data Bits");
    QComboBox *dbitsBox = new QComboBox();
    dbitsBox->addItems({"5", "6", "7", "8"});
    dbitsBox->setCurrentText("8");
    baudFlowDBits->addWidget(dbitsLabel, 0, 2);
    baudFlowDBits->addWidget(dbitsBox, 1, 2);

    QLabel *parityLabel = new QLabel("Parity");
    QComboBox *parityBox = new QComboBox();
    parityBox->addItem("No Parity", 0);
    parityBox->addItem("Even Parity", 2);
    parityBox->addItem("Odd Parity", 3);
    parityBox->addItem("Space Parity", 4);
    parityBox->addItem("Mark Parity", 5);
    parityBox->setCurrentIndex(0);
    baudFlowDBits->addWidget(parityLabel, 2, 0);
    baudFlowDBits->addWidget(parityBox, 3, 0);

    QLabel *stopLabel = new QLabel("Stop Bits");
    QComboBox *stopBox = new QComboBox();
    stopBox->addItem("1", 1);
    stopBox->addItem("1.5", 3);
    stopBox->addItem("2", 2);
    stopBox->setCurrentIndex(0);
    baudFlowDBits->addWidget(stopLabel, 2, 1);
    baudFlowDBits->addWidget(stopBox, 3, 1);

    QLabel *pollingFreqLabel = new QLabel("Polling frequency");
    QComboBox *pollFreqBox = new QComboBox();
    pollFreqBox->setToolTip("Polling Frequency in Hertz");
    pollFreqBox->addItem("10 Hz", 100);
    pollFreqBox->addItem("5 Hz", 200);
    pollFreqBox->addItem("2 Hz", 500);
    pollFreqBox->addItem("1 Hz", 1000);
    pollFreqBox->addItem("0.5 Hz", 2000);
    pollFreqBox->addItem("0.2 Hz", 5000);
    pollFreqBox->addItem("0.1 Hz", 10000);
    pollFreqBox->setCurrentIndex(1);
    baudFlowDBits->addWidget(pollingFreqLabel, 4, 0);
    baudFlowDBits->addWidget(pollFreqBox, 5, 0);

    QLabel *sportTimeoutLabel = new QLabel("Serial port timeout");
    QSpinBox *sportTimeout = new QSpinBox();
    sportTimeout->setMinimum(1);
    sportTimeout->setMaximum(10000);
    sportTimeout->setSuffix("ms");
    sportTimeout->setValue(10);
    sportTimeout->setToolTip("Time in Milliseconds the Serialport will wait \n"
                             "for an answer from the device. Tuning this option \n"
                             "might improve communication resulting in a higher \n"
                             "polling frequency. If this value is to low you \n"
                             "will encounter partial or complete data loss on \n"
                             "the serial port connection.");
    baudFlowDBits->addWidget(sportTimeoutLabel, 4, 1);
    baudFlowDBits->addWidget(sportTimeout, 5, 1);

    // we need the comport string not index
    registerField("comPort", this->comPort, "currentText");
    registerField("baudBox", baudBox, "currentText");
    registerField("flowctlBox", flowctlBox);
    registerField("dbitsBox", dbitsBox, "currentText");
    registerField("parityBox", parityBox, "currentData");
    registerField("stopBox", stopBox, "currentData");
    registerField("pollFreqBox", pollFreqBox, "currentData");
    registerField("sportTimeout", sportTimeout);
}
