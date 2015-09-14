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

#ifndef FLOATINGVALUESDIALOG_H
#define FLOATINGVALUESDIALOG_H

#include <QDialog>

#include <QGridLayout>
#include <QStackedWidget>
#include <QLabel>
#include <QDoubleSpinBox>
#include <QToolButton>

#include <QDebug>

class FloatingValuesDialog : public QDialog
{

    Q_OBJECT

public:
    FloatingValuesDialog(QWidget *parent = 0, Qt::WindowFlags f = 0);

signals:

    void doubleValueAccepted(const double &val, const int &sourceWidget,
                             const int &sourceChannel);

public slots:

    void setSourceWidget(const int &sourcew);
    void setSourceChannel(const int &channel);
    void setInputWidget(const int &w);
    void setInputWidgetValue(const double &value);
    void setInputWidgetValue(const int &trackingMode);
    void updateDeviceSpecs(const double &voltageMin, const double &voltageMax,
                           const uint &voltagePrecision,
                           const double &currentMin, const double &currentMax,
                           const uint &currentPrecision,
                           const uint &noOfChannels);

private:
    QGridLayout *mainLayout;
    QStackedWidget *stackedContainer;
    QToolButton *acceptButton;

    QDoubleSpinBox *voltageSpinBox;
    QDoubleSpinBox *currentSpinBox;

    int sourceWidget;
    int sourceChannel;

    void createUI();

private slots:
    void accept();
    void reject();
};

#endif // FLOATINGVALUESDIALOG_H
