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

#include "plottingarea.h"

namespace setcon = settings_constants;
namespace globcon = global_constants;

PlottingArea::PlottingArea(QWidget *parent) : QWidget(parent)
{
    this->autoScroll = true;
    this->firstStart = true;
    this->startPoint = std::chrono::system_clock::now();
    this->currentDataPointKey = std::chrono::system_clock::now();

    this->setupUI();
    this->setupGraph();
}

void PlottingArea::addData(const int &channel, const double &data,
                           const std::chrono::system_clock::time_point &t,
                           const global_constants::DATATYPE &type)
{
    // FIXME: I think this method crashes when specifiying more channels than
    // actually exist
    /*
     * We actually can calculate the index of our graph using a simple formula
     * (a - 1) * 5 + b
     */
    int index = (channel - 1) * 5 + static_cast<int>(type);
    auto msecs = std::chrono::duration_cast<std::chrono::milliseconds>(
                     t.time_since_epoch())
                     .count();
    double key = msecs / 1000.0;

    this->currentDataPointKey = t;

    this->plot->graph(index)->addData(key, data);

    // TODO: What is firstStart good for? Sounds like a nasty hack :(
    if (this->firstStart) {
        //        this->startPoint = t;
        this->firstStart = false;
    }

    if (this->autoScroll) {
        this->plot->xAxis->setRange(key, 300, Qt::AlignRight);
    }

    this->plot->replot();
}

void PlottingArea::setupUI()
{
    QGridLayout *mainLayout = new QGridLayout();
    this->setLayout(mainLayout);
    this->plot = new QCustomPlot();
    this->plot->setSizePolicy(QSizePolicy::Policy::Expanding,
                              QSizePolicy::Policy::Expanding);
    mainLayout->addWidget(plot, 0, 0);
    // take as much space as possible
    mainLayout->setRowStretch(0, 100);

    this->graphAccordion = new QAccordion();
    this->graphControlScroll = new QScrollArea();
    mainLayout->addWidget(graphControlScroll, 1, 0);
    this->graphControlScroll->setWidget(this->graphAccordion);
    this->graphControlScroll->setWidgetResizable(true);

    int generalPaneIdx = this->graphAccordion->addContentPane("General");

    QFrame *generalFrame =
        this->graphAccordion->getContentPane(generalPaneIdx)->getContentFrame();
    generalFrame->setLayout(new QGridLayout());

    this->cbGeneralAutoscrl = new QCheckBox();
    this->cbGeneralAutoscrl->setText("Auto Scroll");
    this->cbGeneralAutoscrl->setChecked(true);
    generalFrame->layout()->addWidget(this->cbGeneralAutoscrl);

    QObject::connect(this->cbGeneralAutoscrl, SIGNAL(stateChanged(int)), this,
                     SLOT(generalCBCheckState(int)));
}

void PlottingArea::setupGraph()
{
    QSettings settings;
    settings.beginGroup(setcon::DEVICE_GROUP);
    if (settings.contains(setcon::DEVICE_PORT)) {

        this->plot->setInteractions(QCP::Interaction::iRangeDrag |
                                    QCP::Interaction::iRangeZoom |
                                    QCP::Interaction::iSelectLegend);

        this->plot->axisRect()->setRangeZoom(Qt::Orientation::Horizontal);
        this->plot->axisRect()->setRangeDrag(Qt::Orientation::Horizontal);

        this->yAxisContainer.push_back(this->plot->yAxis);
        this->currentAxis =
            this->plot->axisRect()->addAxis(QCPAxis::AxisType::atLeft);
        this->yAxisContainer.push_back(this->currentAxis);
        this->wattageAxis =
            this->plot->axisRect()->addAxis(QCPAxis::AxisType::atLeft);
        this->yAxisContainer.push_back(this->wattageAxis);
        yAxisContainer.at(0)->setLabel("Voltage V");
        yAxisContainer.at(0)->setRangeLower(
            settings.value(setcon::DEVICE_VOLTAGE_MIN).toDouble() - 1);
        yAxisContainer.at(0)->setRangeUpper(
            settings.value(setcon::DEVICE_VOLTAGE_MAX).toDouble() + 1);
        yAxisContainer.at(1)->setLabel("Current A");
        yAxisContainer.at(1)->setRangeLower(
            settings.value(setcon::DEVICE_CURRENT_MIN).toDouble() - 1);
        yAxisContainer.at(1)->setRangeUpper(
            settings.value(setcon::DEVICE_CURRENT_MAX).toDouble() + 1);
        yAxisContainer.at(2)->setLabel("Wattage W");
        yAxisContainer.at(2)->setRangeLower(
            settings.value(setcon::DEVICE_VOLTAGE_MIN).toDouble() *
                settings.value(setcon::DEVICE_CURRENT_MIN).toDouble() -
            1);
        yAxisContainer.at(2)->setRangeUpper(
            settings.value(setcon::DEVICE_VOLTAGE_MAX).toDouble() *
                settings.value(setcon::DEVICE_CURRENT_MAX).toDouble() +
            1);

        this->plot->xAxis->setLabel("Time");
        this->plot->xAxis->setTickLabelType(QCPAxis::LabelType::ltDateTime);
        this->plot->xAxis->setDateTimeFormat("HH:mm:ss");
        // this->plot->xAxis->setAutoTickStep(false);
        // this->plot->xAxis->setTickStep(30);
        this->plot->xAxis->setTickLabelRotation(45);
        this->plot->axisRect()->setupFullAxesBox(true);
        auto msecs = std::chrono::duration_cast<std::chrono::milliseconds>(
                         this->startPoint.time_since_epoch())
                         .count();
        this->plot->xAxis->setRange(msecs / 1000.0, 300, Qt::AlignRight);

        QObject::connect(
            this->plot->xAxis,
            SIGNAL(rangeChanged(const QCPRange &, const QCPRange &)), this,
            SLOT(xAxisRangeChanged(const QCPRange &, const QCPRange &)));

        int graphIndex = 0;

        for (int i = 1; i <= settings.value(setcon::DEVICE_CHANNELS).toInt();
             i++) {
            ContentPane *pane = new ContentPane("Channel " + QString::number(i));
            QFrame *cf = pane->getContentFrame();
            cf->setLayout(new QHBoxLayout());
            this->graphAccordion->addContentPane(pane);

            for (int j = 0; j < 5; j++) {
                globcon::DATATYPE dt = static_cast<globcon::DATATYPE>(j);

                QPen graphPen;

                if (dt == globcon::DATATYPE::CURRENT ||
                    dt == globcon::DATATYPE::ACTUALCURRENT) {
                    this->plot->addGraph(this->plot->xAxis,
                                         this->yAxisContainer.at(1));
                    graphPen.setColor(this->currentGraphColors.at(i - 1));
                } else if (dt == globcon::DATATYPE::WATTAGE) {
                    this->plot->addGraph(this->plot->xAxis,
                                         this->yAxisContainer.at(2));
                    graphPen.setColor(this->wattageGraphColors.at(i - 1));
                } else {
                    this->plot->addGraph();
                    graphPen.setColor(this->voltageGraphColors.at(i - 1));
                }

                // init a QPen for our graph with a color.
                graphPen.setWidthF(2);

                QCheckBox *cb = new QCheckBox();
                cb->setText(this->datatypeStrings.at(dt));

                // only some graphs are visible
                if (dt == globcon::DATATYPE::ACTUALVOLTAGE ||
                    dt == globcon::DATATYPE::ACTUALCURRENT ||
                    dt == globcon::DATATYPE::WATTAGE) {
                    cb->setCheckState(Qt::CheckState::Checked);
                    this->plot->graph(graphIndex)
                        ->setLineStyle(QCPGraph::LineStyle::lsLine);
                } else {
                    // these ones are invisible and only dotted.
                    cb->setCheckState(Qt::CheckState::Unchecked);
                    this->plot->graph(graphIndex)->setVisible(false);
                    graphPen.setStyle(Qt::PenStyle::DotLine);
                }

                this->plot->graph(graphIndex)->setPen(graphPen);
                // set graph name
                this->plot->graph(graphIndex)
                    ->setName(this->graphNames.at(dt).arg(QString::number(i)));

                cf->layout()->addWidget(cb);

                // connect checkbox with a lambda
                QCPGraph *currentGraph = this->plot->graph(graphIndex);
                QObject::connect(cb, &QCheckBox::toggled,
                                 [currentGraph](bool checked) {
                                     if (checked) {
                                         currentGraph->setVisible(true);
                                     } else {
                                         currentGraph->setVisible(false);
                                     }
                                 });

                graphIndex++;
            }
            dynamic_cast<QHBoxLayout *>(cf->layout())->addStretch();
        }
    }
}

void PlottingArea::xAxisRangeChanged(const QCPRange &newRange,
                                     const QCPRange &oldRange)
{
    long newRangeUpperMS = static_cast<long>(newRange.upper * 1000);
    std::chrono::milliseconds newRangeUpperMSDuration(newRangeUpperMS);
    //    std::time_t newLower = newRange.lower * 1000;

    std::chrono::system_clock::time_point tpNewUpper;
    tpNewUpper =
        tpNewUpper + std::chrono::duration_cast<std::chrono::milliseconds>(
                         newRangeUpperMSDuration);
    std::chrono::system_clock::time_point tpNewUpperSecs =
        std::chrono::time_point_cast<std::chrono::seconds>(tpNewUpper);
    //    std::chrono::system_clock::time_point tpNewLower =
    //        std::chrono::system_clock::from_time_t(newLower);

    std::chrono::system_clock::time_point currentDataPointKeySecs =
        std::chrono::time_point_cast<std::chrono::seconds>(
            this->currentDataPointKey);
    std::chrono::system_clock::time_point startPointSecs =
        std::chrono::time_point_cast<std::chrono::seconds>(this->startPoint);

    // QDateTime blah = QDateTime::fromTime_t(newUpper);
    if (tpNewUpperSecs > currentDataPointKeySecs) {
        this->plot->xAxis->setRange(oldRange);
        return;
    }

    if (tpNewUpperSecs < startPointSecs) {
        this->plot->xAxis->setRange(oldRange);
        return;
    }

    if (this->autoScroll && tpNewUpperSecs < currentDataPointKeySecs) {
        // this->autoScroll = false;
        this->cbGeneralAutoscrl->setCheckState(Qt::CheckState::Unchecked);
    }

    // if (newRange.upper > this->currentDataPointKey)
    // this->plot->xAxis->setRange();
    qDebug() << Q_FUNC_INFO
             << "Old Lower: " << QString::number(oldRange.lower, 'f', 6)
             << " Old Upper: " << QString::number(oldRange.upper, 'f', 6);
    qDebug() << Q_FUNC_INFO
             << "Lower: " << QString::number(newRange.lower, 'f', 6)
             << " Upper: " << QString::number(newRange.upper, 'f', 6);
}

void PlottingArea::generalCBCheckState(int state)
{
    if (static_cast<Qt::CheckState>(state) == Qt::CheckState::Checked) {
        this->autoScroll = true;
    } else {
        this->autoScroll = false;
    }
}
