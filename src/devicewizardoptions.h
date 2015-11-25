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

#ifndef DEVICEWIZARDOPTIONS_H
#define DEVICEWIZARDOPTIONS_H

#include <QWidget>
#include <QWizardPage>
#include <QVBoxLayout>
#include <QGridLayout>
#include <QGroupBox>
#include <QLabel>
#include <QDoubleSpinBox>
#include <QComboBox>

#include <QtSerialPort/QSerialPort>
#include <QtSerialPort/QSerialPortInfo>

#include <QDebug>

#include "global.h"

class DeviceWizardOptions : public QWizardPage
{

    Q_OBJECT
    Q_PROPERTY(QString protocolName READ getProtoName)

public:
    DeviceWizardOptions(QWidget *parent = 0);

    void initializePage() Q_DECL_OVERRIDE;
    QString getProtoName() const;

private:
    QComboBox *protoCombo;
    QComboBox *comPort;

    void protocolBox();
    void specBox();
    void comBox();
};

#endif // DEVICEWIZARDOPTIONS_H
