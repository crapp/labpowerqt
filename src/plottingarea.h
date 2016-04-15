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
#include <QColorDialog>
#include <QPushButton>
#include <QDateTime>

#include <QFileDialog>

#include <QDebug>

#include <vector>
#include <chrono>
#include <map>
#include <memory>
#include <iostream>

#include "qcustomplot.h"
#include "global.h"
#include "settingsdefinitions.h"
#include "yaxishelper.h"

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
    // TODO: trigger this slot when settings have been changed.
    void setupGraph();

private:
    std::vector<QColor> voltageGraphColors = {
        QColor(Qt::GlobalColor::red), QColor(Qt::GlobalColor::red).lighter()};
    std::vector<QColor> currentGraphColors = {
        QColor(Qt::GlobalColor::blue), QColor(Qt::GlobalColor::blue).lighter()};
    std::vector<QColor> wattageGraphColors = {
        QColor(Qt::GlobalColor::green),
        QColor(Qt::GlobalColor::green).lighter()};

    const std::map<global_constants::DATATYPE, QString> datatypeStrings = {
        {global_constants::DATATYPE::SETVOLTAGE, "Set Voltage"},
        {global_constants::DATATYPE::VOLTAGE, "Voltage"},
        {global_constants::DATATYPE::SETCURRENT, "Set Current"},
        {global_constants::DATATYPE::CURRENT, "Current"},
        {global_constants::DATATYPE::WATTAGE, "Wattage"}};

    const std::map<global_constants::DATATYPE, QString> graphNames = {
        {global_constants::DATATYPE::SETVOLTAGE, "Set Voltage CH%1"},
        {global_constants::DATATYPE::VOLTAGE, "Voltage CH%1"},
        {global_constants::DATATYPE::SETCURRENT, "Set Current CH%1"},
        {global_constants::DATATYPE::CURRENT, "Current CH%1"},
        {global_constants::DATATYPE::WATTAGE, "Wattage CH%1"}};

    /**
     * @brief The plot widget
     */
    QCustomPlot *plot;
    QCPAxis *voltageAxis;
    QCPAxis *currentAxis;
    QCPAxis *wattageAxis;
    QPen xAxisGridPen;
    QPen yAxisGridPen;

    QToolBar *controlBar;
    QAction *actionGeneral;
    QAction *actionData;
    QAction *actionAppearance;
    QAction *lastAction; /**< last action that was triggered */
    std::unique_ptr<QParallelAnimationGroup> animationGroupControl;

    QStackedWidget *controlStack;
    QWidget *controlGeneral;
    QScrollArea *controlGeneralScroll;
    QWidget *controlData;
    QScrollArea *controlDataScroll;
    QWidget *controlAppearance;
    QScrollArea *controlAppearanceScroll;

    std::unique_ptr<QParallelAnimationGroup> animationGroupDataDisplay;
    int dataDisplayFrameHeight;
    QFrame *dataDisplayFrame;
    // channels
    QFrame *dataDisplayChannels;
    // datetime label
    QLabel *dataDisplayDT;
    // 2d map for the data display labels for each channel
    std::map<global_constants::CHANNEL,
             std::map<global_constants::DATATYPE, QLabel *>> dataDisplayLabels;

    QCheckBox *cbGeneralAutoscrl; /**< need to keep a pointer to this one*/

    bool autoScroll;
    std::chrono::system_clock::time_point startPoint;
    std::chrono::system_clock::time_point currentDataPointKey;

    QCPRange lastRange;

    int lowZoom;
    int highZoom;

    /**
     * @brief Setup the basic UI
     */
    void setupUI();
    /**
     * @brief Reset all dynamically generated UI elements
     */
    void resetGraph();

    /**
     * @brief Set the default values and connections for the plot
     */
    void setupGraphPlot(const QSettings &settings);

    /**
     * @brief Update the y-axis range
     * @param currentXRange
     */
    void yAxisRange(const QCPRange &currentXRange, const QSettings &settings);
    void yAxisVisibility();

private slots:

    // plot slots
    void beforeReplotHandle();
    void xAxisRangeChanged(const QCPRange &newRange, const QCPRange &oldRange);
    void mouseMoveHandler(QMouseEvent *event);

    // ui slots
    void toolbarActionTriggered(QAction *action);
    void generalCBCheckState(int state);
};

#endif // PLOTTINGAREA_H
