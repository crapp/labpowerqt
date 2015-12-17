// labpowerqt is a Gui application to control programmable lab power supplies
// Copyright © 2015 Christian Rapp <0x2a at posteo dot org>

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

#include "devicewizardconnection.h"

DeviceWizardConnection::DeviceWizardConnection(QWidget *parent)
    : QWizardPage(parent)
{
    this->setTitle(tr("Add Power Supply"));
    this->setSubTitle(tr("Test the connection to your power supply"));
    this->setPixmap(QWizard::LogoPixmap, QPixmap(":/icons/device32.png"));
    QGridLayout *gridl = new QGridLayout();
    this->setLayout(gridl);

    this->devID = "";

    this->startTest = new QPushButton("Test connection");
    gridl->addWidget(this->startTest, 0, 0);
    gridl->addItem(new QSpacerItem(1, 1, QSizePolicy::Policy::Expanding,
                                   QSizePolicy::Policy::Minimum),
                   0, 1);
    this->txt = new QPlainTextEdit();
    this->txt->setReadOnly(true);
    this->txt->setStyleSheet(
        "QPlainTextEdit {background-color: #000000; color: #F3F3F3}");
    gridl->addWidget(this->txt, 1, 0, 1, 2);
    this->txt->setPlainText("Idle");

    this->t = std::unique_ptr<QThread>(new QThread());

    QObject::connect(this->startTest, &QPushButton::clicked, this,
                     &DeviceWizardConnection::testConnection);

    registerField("deviceIdentification", this, "deviceID");
}

QString DeviceWizardConnection::getDeviceID() const { return this->devID; }

void DeviceWizardConnection::initializePage()
{
    // disable next button until test is successfull.
    this->connectionSuccessfull = false;
}

bool DeviceWizardConnection::isComplete() const
{
    return this->connectionSuccessfull;
}

void DeviceWizardConnection::testConnection()
{
    this->startTest->setDisabled(true);
    if (this->t->isRunning()) {
        qDebug() << Q_FUNC_INFO << "Thread has been terminated";
        this->t->terminate();
    }

    this->txt->clear();
    QString statustext =
        "Connecting to device on port " + field("comPort").toString();
    this->txt->appendPlainText(statustext);

    QSerialPort::BaudRate brate =
        static_cast<QSerialPort::BaudRate>(field("baudBox").toInt());
    QSerialPort::FlowControl flowctl =
        static_cast<QSerialPort::FlowControl>(field("flowctlBox").toInt());
    QSerialPort::DataBits dbits =
        static_cast<QSerialPort::DataBits>(field("dbitsBox").toInt());
    QSerialPort::Parity parity =
        static_cast<QSerialPort::Parity>(field("parityBox").toInt());
    QSerialPort::StopBits sbits =
        static_cast<QSerialPort::StopBits>(field("stopBox").toInt());

    if (static_cast<global_constants::PROTOCOL>(field("protocol").toInt()) ==
        global_constants::PROTOCOL::KORADV2) {
        this->powerSupplyConnector = std::unique_ptr<KoradSCPI>(new KoradSCPI(
            std::move(field("comPort").toString()),
            std::move(QByteArray("WizardConnectionTest")),
            field("channel").toInt(), field("voltAcc").toInt(),
            field("currentAcc").toInt(), brate, flowctl, dbits, parity, sbits));
    }

    this->txt->appendPlainText("Using " + field("protocolText").toString() +
                               " protocol");

    this->powerSupplyConnector->moveToThread(this->t.get());

    QObject::connect(this->powerSupplyConnector.get(),
                     &PowerSupplySCPI::requestFinished, this,
                     &DeviceWizardConnection::dataAvailable);
    QObject::connect(this->powerSupplyConnector.get(),
                     &PowerSupplySCPI::errorOpen, this,
                     &DeviceWizardConnection::deviceError);
    QObject::connect(this->t.get(), &QThread::started,
                     this->powerSupplyConnector.get(),
                     &PowerSupplySCPI::startPowerSupplyBackgroundThread);
    QObject::connect(this->powerSupplyConnector.get(),
                     &PowerSupplySCPI::backgroundThreadStopped,
                     [this]() { this->t->quit(); });

    this->t->start();
    this->powerSupplyConnector->getIdentification();
}

void DeviceWizardConnection::dataAvailable(
    std::shared_ptr<SerialCommand> command)
{
    if (static_cast<PowerSupplySCPI_constants::COMMANDS>(
            command->getCommand()) ==
        PowerSupplySCPI_constants::COMMANDS::GETIDN) {
        QString idString = command->getValue().toString();
        this->devID = idString;
        if (idString == "") {
            QString statustext = "Connection successfull but Device send back "
                                 "an empty identification String. Check the "
                                 "chosen protocol and port.";
            this->txt->appendPlainText(statustext);
            this->connectionSuccessfull = false;
            emit this->completeChanged();
        } else {
            QString statustext = "Connection successfull";
            this->txt->appendPlainText(statustext);
            statustext = "Device identified as " + idString;
            this->txt->appendPlainText(statustext);
            this->connectionSuccessfull = true;
            emit this->completeChanged();
        }
        this->startTest->setDisabled(false);
        this->powerSupplyConnector->stopPowerSupplyBackgroundThread();
        this->t->wait();
    }
}

void DeviceWizardConnection::deviceError(QString errorString)
{
    QString statustext = "Failed to open device.\nError message: " + errorString;
    this->txt->appendPlainText(statustext);
    this->txt->appendPlainText(
        "Ensure your device is powered on and you have chosen the appropriate "
        "communications "
        "protocol as well as the correct device port.");
    this->connectionSuccessfull = false;
    this->powerSupplyConnector->stopPowerSupplyBackgroundThread();
    this->t->wait();
    emit this->completeChanged();
    this->startTest->setDisabled(false);
}
