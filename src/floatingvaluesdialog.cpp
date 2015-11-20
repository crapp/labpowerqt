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

#include "floatingvaluesdialog.h"

namespace globcon = global_constants;

FloatingValuesDialog::FloatingValuesDialog(QWidget *parent, Qt::WindowFlags f)
    : QDialog(parent, f)
{
    // set to sane default values;
    this->data = nullptr;
    this->createUI();
}

void FloatingValuesDialog::setValuesDialogData(
    std::shared_ptr<FloatingValuesDialogData> data)
{
    this->data = data;
}

void FloatingValuesDialog::setDatatype(global_constants::DATATYPE dt)
{
    this->dt = dt;
    // set the index of the stacked container.
    switch (dt) {
    case globcon::DATATYPE::SETVOLTAGE:
        this->stackedContainer->setCurrentIndex(0);
        break;
    case globcon::DATATYPE::SETCURRENT:
        this->stackedContainer->setCurrentIndex(1);
        break;
    default:
        break;
    }

    // make sure the dialog is as small as possible
    this->resize(1, 1);
}

void FloatingValuesDialog::setCurrentValue(double value)
{
    // WARNING: This is highly dependend of our gui structure
    QFrame *cont =
        dynamic_cast<QFrame *>(this->stackedContainer->currentWidget());
    dynamic_cast<QDoubleSpinBox *>(cont->children()[1])->setValue(value);
}

void FloatingValuesDialog::setCurrentValue(ATTR_UNUSED int trackingMode) {}

void FloatingValuesDialog::updateDeviceSpecs(
    double voltageMin, double voltageMax, uint voltagePrecision,
    double currentMin, double currentMax, uint currentPrecision,
    uint noOfChannels)
{
    voltageSpinBox->setMinimum(voltageMin);
    voltageSpinBox->setMaximum(voltageMax);
    voltageSpinBox->setDecimals(voltagePrecision);
    voltageSpinBox->setSingleStep(1.0 / pow(10.0, voltagePrecision));

    currentSpinBox->setMinimum(currentMin);
    currentSpinBox->setMaximum(currentMax);
    currentSpinBox->setDecimals(currentPrecision);
    currentSpinBox->setSingleStep(1.0 / pow(10.0, currentPrecision));
}

void FloatingValuesDialog::createUI()
{
    this->mainLayout = new QGridLayout(this);
    this->setLayout(mainLayout);
    this->stackedContainer = new QStackedWidget();
    mainLayout->addWidget(this->stackedContainer, 0, 0);

    QFrame *frameVoltage = new QFrame();
    stackedContainer->addWidget(frameVoltage);
    QGridLayout *voltageLayout = new QGridLayout();
    frameVoltage->setLayout(voltageLayout);
    voltageLayout->setHorizontalSpacing(0);
    voltageLayout->setVerticalSpacing(0);
    voltageLayout->setMargin(0);
    this->voltageSpinBox = new QDoubleSpinBox();
    voltageSpinBox->setSuffix(" V");
    voltageSpinBox->setAlignment(Qt::AlignRight);
    voltageLayout->addWidget(voltageSpinBox);

    QFrame *frameCurrent = new QFrame();
    stackedContainer->addWidget(frameCurrent);
    QGridLayout *currentLayout = new QGridLayout();
    frameCurrent->setLayout(currentLayout);
    currentLayout->setHorizontalSpacing(0);
    currentLayout->setVerticalSpacing(0);
    currentLayout->setMargin(0);
    this->currentSpinBox = new QDoubleSpinBox();
    currentSpinBox->setSuffix(" A");
    currentSpinBox->setAlignment(Qt::AlignRight);
    currentLayout->addWidget(currentSpinBox);

    // TODO emkplement the other stacked Widgets

    this->acceptButton = new QToolButton();
    acceptButton->setText("");
    QIcon acceptIcon;
    acceptIcon.addPixmap(QPixmap(":/icons/dialog_accept.png"));
    acceptButton->setIcon(acceptIcon);

    this->mainLayout->addWidget(this->acceptButton, 0, 1);

    QObject::connect(this->acceptButton, SIGNAL(clicked()), this,
                     SLOT(accept()));
}

void FloatingValuesDialog::accept()
{
    qDebug() << Q_FUNC_INFO << "Dialog was accepted";
    QFrame *cont =
        dynamic_cast<QFrame *>(this->stackedContainer->currentWidget());
    double value = dynamic_cast<QDoubleSpinBox *>(cont->children()[1])->value();
    switch (dt) {
    case globcon::DATATYPE::SETVOLTAGE:
        this->data->voltage = value;
        break;
    case globcon::DATATYPE::SETCURRENT:
        this->data->current = value;
        break;
    default:
        break;
    }

    this->done(1);
}

void FloatingValuesDialog::reject()
{
    qDebug() << Q_FUNC_INFO << "Dialog was rejected";
    this->done(0);
}
