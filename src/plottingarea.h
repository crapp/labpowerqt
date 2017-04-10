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

#ifndef PLOTTINGAREA_H
#define PLOTTINGAREA_H

#include <QColor>
#include <QColorDialog>
#include <QDateTime>
#include <QPushButton>
#include <QScrollArea>
#include <QWidget>

#include <QFileDialog>

#include <chrono>
#include <iostream>
#include <map>
#include <memory>
#include <vector>

#include "global.h"
#include "log_instance.h"
#include "qcustomplot.h"
#include "settingsdefault.h"
#include "settingsdefinitions.h"
#include "yaxishelper.h"

/**
 * @brief The PlottingArea with the QCustomPlot and other widgets to control it
 */
class PlottingArea : public QWidget
{
    Q_OBJECT
public:
    explicit PlottingArea(QWidget *parent = 0);

signals:

public slots:

    /**
     * @brief Add data to the QCustomPlot
     *
     * @param channel Which channel
     * @param data
     * @param t Measurement Time Point
     * @param type Type of data
     */
    void addData(const int &channel, const double &data,
                 const std::chrono::system_clock::time_point &t,
                 const global_constants::LPQ_DATATYPE &type);
    /**
     * @brief This slot is invoked whenever the settings or the device changes
     */
    void setupGraph();

private:
    std::vector<QColor> voltageGraphColors = {
        QColor(Qt::GlobalColor::red), QColor(Qt::GlobalColor::red).lighter()};
    std::vector<QColor> currentGraphColors = {
        QColor(Qt::GlobalColor::blue), QColor(Qt::GlobalColor::blue).lighter()};
    std::vector<QColor> wattageGraphColors = {
        QColor(Qt::GlobalColor::green),
        QColor(Qt::GlobalColor::green).lighter()};

    const std::map<global_constants::LPQ_DATATYPE, QString> datatypeStrings = {
        {global_constants::LPQ_DATATYPE::SETVOLTAGE, "Set Voltage"},
        {global_constants::LPQ_DATATYPE::VOLTAGE, "Voltage"},
        {global_constants::LPQ_DATATYPE::SETCURRENT, "Set Current"},
        {global_constants::LPQ_DATATYPE::CURRENT, "Current"},
        {global_constants::LPQ_DATATYPE::WATTAGE, "Wattage"}};

    const std::map<global_constants::LPQ_DATATYPE, QString> graphNames = {
        {global_constants::LPQ_DATATYPE::SETVOLTAGE, "Set Voltage CH%1"},
        {global_constants::LPQ_DATATYPE::VOLTAGE, "Voltage CH%1"},
        {global_constants::LPQ_DATATYPE::SETCURRENT, "Set Current CH%1"},
        {global_constants::LPQ_DATATYPE::CURRENT, "Current CH%1"},
        {global_constants::LPQ_DATATYPE::WATTAGE, "Wattage CH%1"}};

    /**
     * @brief The plot widget
     */
    QCustomPlot *plot;
    QCPAxis *voltageAxis;
    QCPAxis *currentAxis;
    QCPAxis *wattageAxis;
    QPen xAxisGridPen;
    QPen yAxisGridPen;
    // Items to show zoom level
    QCPItemPixmap *zoomMagPic;
    QCPItemText *zoomLevel;

    // Toolbar containing actions to show different sections in controlStack
    QToolBar *controlBar;
    QAction *actionGeneral;
    QAction *actionData;
    QAction *actionAppearance;
    QAction *lastAction; /**< last action that was triggered */
    std::unique_ptr<QParallelAnimationGroup> animationGroupControl;

    // Stack widget containing control sections for plotting area
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
    std::map<global_constants::LPQ_CHANNEL,
             std::map<global_constants::LPQ_DATATYPE, QLabel *>>
        dataDisplayLabels;

    QCheckBox *cbGeneralPlot; /**< Activate data plotting */
    QCheckBox *
        cbGeneralShowData; /**< Show data at mouse cursor position in own area under plot */
    QCheckBox *cbGeneralShowTimescale; /**< Show x-axis timescale */
    QCheckBox *cbGeneralAutoscrl;      /**< need to keep a pointer to this one*/

    // TODO: Why do we have a separate boolean for auto scroll if there is
    // pointer to the checkbutton that controls this behaviour?
    bool autoScroll;
    std::chrono::system_clock::time_point startPoint;
    std::chrono::system_clock::time_point currentDataPointKey;

    /**
     * @brief Setup the basic UI, this is only called once.
     */
    void setupUI();
    /**
     * @brief Reset all dynamically generated UI elements
     *
     * @details
     *
     * Is called whenever major things change.
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

    /**
     * @brief Controls the visibility of y-axes
     *
     * @details
     * If there is no graph visibile for a corresponding y-axis it will be
     * automatically hidden
     */
    void yAxisVisibility();

private slots:

    // plot slots
    // TODO: Currently beforeReplotHandle is unused
    void beforeReplotHandle();
    void xAxisRangeChanged(const QCPRange &newRange, const QCPRange &oldRange);
    /**
     * @brief Registers all mouse moves inside the plot
     *
     * @param event
     *
     * @details
     * Currently used to show the data at mouse cursor position beneath the plot
     * object.
     */
    void mouseMoveHandler(QMouseEvent *event);

    // ui slots
    void toolbarActionTriggered(QAction *action);
    void generalCBCheckState(int state);
};

#endif  // PLOTTINGAREA_H
