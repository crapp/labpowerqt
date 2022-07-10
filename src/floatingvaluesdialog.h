// This file is part of labpowerqt, a Gui application to control programmable
// lab power supplies.
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

#ifndef FLOATINGVALUESDIALOG_H
#define FLOATINGVALUESDIALOG_H

#include <QDialog>

#include <QGridLayout>
#include <QLabel>
#include <QStackedWidget>
#include <QToolButton>

#include <cmath>
#include <memory>

#include "global.h"
#include "valuedoublespinbox.h"

/**
 * @brief Struct to save the values user set with the FloatingValuesDialog
 */
struct FloatingValuesDialogData {
    FloatingValuesDialogData()
    {
        this->voltage = 0;
        this->current = 0;
        this->tracking = 0;
    }

    double voltage;
    double current;
    int tracking;
};

/**
 * @brief A Dialog that is floating over the Display area to set V, A and tracking mode.
 */
class FloatingValuesDialog : public QDialog
{
    Q_OBJECT

public:
    explicit FloatingValuesDialog(QWidget *parent = nullptr, Qt::WindowFlags f = {});

    void setValuesDialogData(std::shared_ptr<FloatingValuesDialogData> data);

signals:

    void doubleValueAccepted(double val, int sourceWidget, int sourceChannel);

public slots:

    void setDatatype(global_constants::LPQ_DATATYPE dt);
    void setCurrentValue(double value);
    void setCurrentValue(int trackingMode);
    void updateDeviceSpecs(double voltageMin, double voltageMax,
                           uint voltagePrecision, double currentMin,
                           double currentMax, uint currentPrecision,
                           uint noOfChannels);

private:
    QGridLayout *mainLayout;
    QStackedWidget *stackedContainer;
    QToolButton *acceptButton;

    ValueDoubleSpinBox *voltageSpinBox;
    ValueDoubleSpinBox *currentSpinBox;

    global_constants::LPQ_DATATYPE dt;
    int sourceChannel;

    std::shared_ptr<FloatingValuesDialogData> data;

    void createUI();

private slots:
    void accept() override;
    void reject() override;
};

#endif  // FLOATINGVALUESDIALOG_H
