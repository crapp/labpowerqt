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

#ifndef PLOTTINGAREA_H
#define PLOTTINGAREA_H

#include <QWidget>
#include <QColor>
#include <QScrollArea>

#include <vector>
#include <chrono>
#include <map>

#include "qcustomplot.h"
#include "qAccordion/qaccordion.h"
#include "global.h"
#include "settingsdefinitions.h"

class PlottingArea : public QWidget
{
    Q_OBJECT
public:
    explicit PlottingArea(QWidget *parent = 0);

signals:

public slots:

    void addData(const int &channel, const double &data,
                 const std::chrono::system_clock::time_point &t,
                 const global_constants::DATATYPE &type);

private:
    const std::vector<QColor> voltageGraphColors = {
        QColor(Qt::GlobalColor::red), QColor(Qt::GlobalColor::red)};
    const std::vector<QColor> currentGraphColors = {
        QColor(Qt::GlobalColor::blue), QColor(Qt::GlobalColor::blue)};
    const std::vector<QColor> wattageGraphColors = {
        QColor(Qt::GlobalColor::green), QColor(Qt::GlobalColor::green)};

    const std::map<global_constants::DATATYPE, QString> datatypeStrings = {
        {global_constants::DATATYPE::VOLTAGE, "Set Voltage"},
        {global_constants::DATATYPE::ACTUALVOLTAGE, "Actual Voltage"},
        {global_constants::DATATYPE::CURRENT, "Set Current"},
        {global_constants::DATATYPE::ACTUALCURRENT, "Actual Current"},
        {global_constants::DATATYPE::WATTAGE, "Wattage"}
    };

    const std::map<global_constants::DATATYPE, QString> graphNames = {
        {global_constants::DATATYPE::VOLTAGE, "Voltage CH%1"},
        {global_constants::DATATYPE::ACTUALVOLTAGE, "Actual Voltage CH%1"},
        {global_constants::DATATYPE::CURRENT, "Current CH%1"},
        {global_constants::DATATYPE::ACTUALCURRENT, "Actual Current CH%1"},
        {global_constants::DATATYPE::WATTAGE, "Wattage CH%1"}
    };

    std::vector<QCPAxis *> yAxisContainer;

    /**
     * @brief plot The plot widget
     */
    QCustomPlot *plot;
    QCPAxis *currentAxis;
    QCPAxis *wattageAxis;

    QScrollArea *graphControlScroll;
    QAccordion *graphAccordion;

    QCheckBox *cbGeneralAutoscrl;

    bool firstStart;
    bool autoScroll;
    std::chrono::system_clock::time_point startPoint;
    std::chrono::system_clock::time_point currentDataPointKey;

    void setupUI();
    void setupGraph();

private slots:

    void xAxisRangeChanged(const QCPRange &newRange, const QCPRange &oldRange);

    void generalCBCheckState(int state);
};

#endif // PLOTTINGAREA_H
