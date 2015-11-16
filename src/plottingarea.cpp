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
    this->lastAction = nullptr;

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
        this->plot->xAxis->setRange(key, this->plot->xAxis->range().size(),
                                    Qt::AlignRight);
    }

    this->plot->replot();
}

void PlottingArea::setupGraph()
{
    if (this->plot->graphCount() > 0) {
        // TODO: Does this work? really have to check this!
        delete this->plot;
        this->plot = new QCustomPlot();
        this->plot->setSizePolicy(QSizePolicy::Policy::Expanding,
                                  QSizePolicy::Policy::Expanding);
    }

    QSettings settings;
    settings.beginGroup(setcon::DEVICE_GROUP);
    if (settings.contains(setcon::DEVICE_PORT)) {

        this->plot->setInteractions(QCP::Interaction::iRangeDrag |
                                    QCP::Interaction::iRangeZoom |
                                    QCP::Interaction::iSelectLegend);

        this->plot->axisRect()->setRangeZoom(Qt::Orientation::Horizontal);
        this->plot->axisRect()->setRangeDrag(Qt::Orientation::Horizontal);

        this->voltageAxis = this->plot->yAxis;
        this->currentAxis =
            this->plot->axisRect()->addAxis(QCPAxis::AxisType::atLeft);
        this->wattageAxis =
            this->plot->axisRect()->addAxis(QCPAxis::AxisType::atLeft);
        this->voltageAxis->setLabel("Voltage V");
        this->voltageAxis->setRangeLower(
            settings.value(setcon::DEVICE_VOLTAGE_MIN).toDouble() - 1);
        this->voltageAxis->setRangeUpper(
            settings.value(setcon::DEVICE_VOLTAGE_MAX).toDouble() + 1);
        this->currentAxis->setLabel("Current A");
        this->currentAxis->setRangeLower(
            settings.value(setcon::DEVICE_CURRENT_MIN).toDouble() - 1);
        this->currentAxis->setRangeUpper(
            settings.value(setcon::DEVICE_CURRENT_MAX).toDouble() + 1);
        this->wattageAxis->setLabel("Wattage W");
        this->wattageAxis->setRangeLower(
            settings.value(setcon::DEVICE_VOLTAGE_MIN).toDouble() *
                settings.value(setcon::DEVICE_CURRENT_MIN).toDouble() -
            1);
        this->wattageAxis->setRangeUpper(
            settings.value(setcon::DEVICE_VOLTAGE_MAX).toDouble() *
                settings.value(setcon::DEVICE_CURRENT_MAX).toDouble() +
            1);
        // xaxis label and ticks
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
        // initial range
        this->plot->xAxis->setRange(msecs / 1000.0, 300, Qt::AlignRight);

        QObject::connect(
            this->plot->xAxis,
            SIGNAL(rangeChanged(const QCPRange &, const QCPRange &)), this,
            SLOT(xAxisRangeChanged(const QCPRange &, const QCPRange &)));
        QObject::connect(this->plot, &QCustomPlot::beforeReplot, this,
                         &PlottingArea::beforeReplotHandle);

        QObject::connect(this->plot, &QCustomPlot::mouseMove, this,
                         &PlottingArea::mouseMoveHandler);

        int graphIndex = 0;

        for (int i = 1; i <= settings.value(setcon::DEVICE_CHANNELS).toInt();
             i++) {
            ContentPane *pane = new ContentPane("Channel " + QString::number(i));
            QFrame *cf = pane->getContentFrame();
            cf->setLayout(new QHBoxLayout());
            this->graphAccordion->addContentPane(pane);

            QGroupBox *dataBox = new QGroupBox("Channel " + QString::number(i));
            dataBox->setLayout(new QHBoxLayout());
            this->controlData->layout()->addWidget(dataBox);
            QGroupBox *appearanceBox =
                new QGroupBox("Channel " + QString::number(i));
            appearanceBox->setLayout(new QVBoxLayout());
            this->controlAppearance->layout()->addWidget(appearanceBox);

            for (int j = 0; j < 5; j++) {
                globcon::DATATYPE dt = static_cast<globcon::DATATYPE>(j);

                // init a QPen for our graph
                QPen graphPen;
                // set the width of the pen
                graphPen.setWidth(2);

                QCheckBox *cbDataSwitch = new QCheckBox();
                cbDataSwitch->setText(this->datatypeStrings.at(dt));
                QLabel *labelGraphProps =
                    new QLabel(this->datatypeStrings.at(dt));
                labelGraphProps->setMinimumWidth(120);
                QComboBox *graphLineStyle = new QComboBox();
                graphLineStyle->addItem("Solid", QVariant(0));
                graphLineStyle->addItem("Dotted", QVariant(0));

                QSpinBox *graphLineThickness = new QSpinBox();
                graphLineThickness->setMinimum(1);
                graphLineThickness->setMaximum(5);
                graphLineThickness->setValue(2);

                QPushButton *graphColor = new QPushButton();
                graphColor->setToolTip("Choose graph color");
                graphColor->setIconSize(QSize(64, 16));
                QPixmap pic(64, 16);

                if (dt == globcon::DATATYPE::CURRENT ||
                    dt == globcon::DATATYPE::ACTUALCURRENT) {
                    this->plot->addGraph(this->plot->xAxis, this->currentAxis);
                    pic.fill(this->currentGraphColors.at(i - 1));
                    graphPen.setColor(this->currentGraphColors.at(i - 1));
                } else if (dt == globcon::DATATYPE::WATTAGE) {
                    this->plot->addGraph(this->plot->xAxis, this->wattageAxis);
                    pic.fill(this->wattageGraphColors.at(i - 1));
                    graphPen.setColor(this->wattageGraphColors.at(i - 1));
                } else {
                    this->plot->addGraph();
                    pic.fill(this->voltageGraphColors.at(i - 1));
                    graphPen.setColor(this->voltageGraphColors.at(i - 1));
                }

                graphColor->setIcon(pic);

                // only some graphs are visible
                if (dt == globcon::DATATYPE::ACTUALVOLTAGE ||
                    dt == globcon::DATATYPE::ACTUALCURRENT ||
                    dt == globcon::DATATYPE::WATTAGE) {
                    cbDataSwitch->setCheckState(Qt::CheckState::Checked);
                    graphLineStyle->setCurrentIndex(0);
                    graphPen.setStyle(Qt::PenStyle::SolidLine);
                } else {
                    // these ones are invisible and only dotted.
                    cbDataSwitch->setCheckState(Qt::CheckState::Unchecked);
                    graphLineStyle->setCurrentIndex(1);
                    this->plot->graph(graphIndex)->setVisible(false);
                    graphPen.setStyle(Qt::PenStyle::DotLine);
                }

                this->plot->graph(graphIndex)->setPen(graphPen);
                // set graph name
                this->plot->graph(graphIndex)
                    ->setName(this->graphNames.at(dt).arg(QString::number(i)));

                dataBox->layout()->addWidget(cbDataSwitch);
                QHBoxLayout *appearanceElemLayout = new QHBoxLayout();
                appearanceElemLayout->addWidget(labelGraphProps);
                appearanceElemLayout->addWidget(graphLineStyle);
                appearanceElemLayout->addWidget(graphLineThickness);
                appearanceElemLayout->addWidget(graphColor);
                appearanceElemLayout->addStretch();
                dynamic_cast<QVBoxLayout *>(appearanceBox->layout())
                    ->addLayout(appearanceElemLayout);

                // connect checkbox with a lambda
                QCPGraph *currentGraph = this->plot->graph(graphIndex);
                QObject::connect(cbDataSwitch, &QCheckBox::toggled,
                                 [currentGraph](bool checked) {
                                     if (checked) {
                                         currentGraph->setVisible(true);
                                     } else {
                                         currentGraph->setVisible(false);
                                     }
                                 });
                // connect linestyle
                QObject::connect(
                    graphLineStyle, static_cast<void (QComboBox::*)(int)>(
                                        &QComboBox::currentIndexChanged),
                    [currentGraph, graphLineStyle, graphIndex](int idx) {
                        QPen graphPen = currentGraph->pen();
                        if (idx == 0) {
                            graphPen.setStyle(Qt::PenStyle::SolidLine);
                        } else {
                            graphPen.setStyle(Qt::PenStyle::DotLine);
                        }
                        currentGraph->setPen(graphPen);
                    });
                // connect line thickness
                QObject::connect(
                    graphLineThickness, static_cast<void (QSpinBox::*)(int)>(
                                            &QSpinBox::valueChanged),
                    [currentGraph, graphIndex, graphLineThickness](int value) {
                        QPen graphPen = currentGraph->pen();
                        graphPen.setWidth(value);
                        currentGraph->setPen(graphPen);
                    });
                // connect color button
                QObject::connect(
                    graphColor, &QPushButton::clicked,
                    [this, currentGraph, graphColor, graphIndex]() {
                        QPen graphPen = currentGraph->pen();
                        QColor col = QColorDialog::getColor(
                            graphPen.color(), this, "Choose a color");
                        if (col.isValid()) {
                            graphPen.setColor(col);
                            currentGraph->setPen(graphPen);
                            QPixmap pic = graphColor->icon().pixmap(64, 16);
                            pic.fill(col);
                            graphColor->setIcon(pic);
                        }
                    });

                graphIndex++;
            }
            dynamic_cast<QHBoxLayout *>(cf->layout())->addStretch();
            dynamic_cast<QHBoxLayout *>(dataBox->layout())->addStretch();
        }
    }
}

void PlottingArea::setupUI()
{
    QGridLayout *mainLayout = new QGridLayout();
    this->setLayout(mainLayout);

    this->controlBar = new QToolBar();
    this->controlBar->setFloatable(false);
    this->controlBar->setMovable(false);
    mainLayout->addWidget(this->controlBar, 0, 0);
    this->actionGeneral = this->controlBar->addAction("General");
    this->actionData = this->controlBar->addAction("Data");
    this->actionAppearance = this->controlBar->addAction("Appearance");
    QObject::connect(this->controlBar, &QToolBar::actionTriggered, this,
                     &PlottingArea::toolbarActionTriggered);

    this->controlStack = new QStackedWidget();
    mainLayout->addWidget(this->controlStack, 1, 0);
    this->controlGeneral = new QWidget();
    QGridLayout *controlGeneralLay = new QGridLayout();
    this->controlGeneralScroll = new QScrollArea();
    this->controlGeneral->setLayout(controlGeneralLay);
    this->cbGeneralAutoscrl = new QCheckBox();
    this->cbGeneralAutoscrl->setText("Auto Scroll");
    this->cbGeneralAutoscrl->setChecked(true);
    controlGeneralLay->addWidget(this->cbGeneralAutoscrl, 0, 0, Qt::AlignTop);
    QObject::connect(this->cbGeneralAutoscrl, SIGNAL(stateChanged(int)), this,
                     SLOT(generalCBCheckState(int)));
    this->controlGeneralScroll->setWidget(this->controlGeneral);
    this->controlGeneralScroll->setWidgetResizable(true);

    this->controlStack->addWidget(this->controlGeneralScroll);

    // control data widget. Allows to toggle graphs to display. Will be filled
    // later.
    this->controlData = new QWidget();
    this->controlData->setLayout(new QVBoxLayout());
    this->controlDataScroll = new QScrollArea();
    this->controlDataScroll->setWidget(this->controlData);
    this->controlDataScroll->setWidgetResizable(true);
    this->controlStack->addWidget(this->controlDataScroll);

    this->controlAppearance = new QWidget();
    this->controlAppearance->setLayout(new QVBoxLayout());
    this->controlAppearanceScroll = new QScrollArea();
    this->controlAppearanceScroll->setWidget(this->controlAppearance);
    this->controlAppearanceScroll->setWidgetResizable(true);
    this->controlStack->addWidget(this->controlAppearanceScroll);

    // Show general widget
    this->controlStack->setCurrentIndex(0);
    this->controlStack->setMinimumHeight(0);
    this->controlStack->setMaximumHeight(0);

    this->animationGroupControl =
        std::unique_ptr<QParallelAnimationGroup>(new QParallelAnimationGroup());
    QPropertyAnimation *minHeightAni = new QPropertyAnimation();
    QPropertyAnimation *maxHeightAni = new QPropertyAnimation();
    minHeightAni->setTargetObject(this->controlStack);
    maxHeightAni->setTargetObject(this->controlStack);
    minHeightAni->setPropertyName("minimumHeight");
    maxHeightAni->setPropertyName("maximumHeight");
    this->animationGroupControl->addAnimation(minHeightAni);
    this->animationGroupControl->addAnimation(maxHeightAni);

    // generate plot widget
    this->plot = new QCustomPlot();
    this->plot->setSizePolicy(QSizePolicy::Policy::Expanding,
                              QSizePolicy::Policy::Expanding);
    mainLayout->addWidget(this->plot, 2, 0);
    // take as much space as possible
    mainLayout->setRowStretch(2, 100);

    this->graphAccordion = new QAccordion();
    this->graphControlScroll = new QScrollArea();
    mainLayout->addWidget(graphControlScroll, 3, 0);
    this->graphControlScroll->setWidget(this->graphAccordion);
    this->graphControlScroll->setWidgetResizable(true);
    this->graphControlScroll->setMinimumHeight(80);
    this->graphControlScroll->setFrameStyle(QFrame::Shape::NoFrame);
    this->graphAccordion->setCollapsible(true);
    this->graphAccordion->setMultiActive(true);
}

void PlottingArea::yAxisRange(const QCPRange &currentXRange)
{
    if (this->plot->graphCount() == 0)
        return;

    QCPDataMap *dataMap = this->plot->graph(0)->data();
    std::vector<double> graphValues;
    if (dataMap->size() > 1) {
        // get the nearest key to lower
        QCPDataMap::Iterator itbegin = dataMap->upperBound(currentXRange.lower);
        if (itbegin == dataMap->end())
            itbegin = dataMap->begin();
        // get the nearest key to upper
        QCPDataMap::Iterator itend = dataMap->lowerBound(currentXRange.upper);
        if (itend == dataMap->end())
            itend = dataMap->end() - 1;
        for (; itbegin != itend; itbegin++) {
            graphValues.push_back((*itbegin).value);
        }
        std::sort(graphValues.begin(), graphValues.end());
    } else if (!dataMap->empty()) {
        graphValues.push_back(dataMap->first().value);
    }
    if (graphValues.size() > 0)
        this->voltageAxis->setRange(
            QCPRange(graphValues.front() - 0.5, graphValues.back() + 0.5));
}

void PlottingArea::beforeReplotHandle()
{
    // qDebug() << Q_FUNC_INFO << "before replot";
}

void PlottingArea::xAxisRangeChanged(const QCPRange &newRange,
                                     const QCPRange &oldRange)
{

    // first set y axis range
    this->yAxisRange(newRange);

    long newRangeUpperMS = static_cast<long>(newRange.upper * 1000);
    std::chrono::milliseconds newRangeUpperMSDuration(newRangeUpperMS);
    long newRangeLowerMS = static_cast<long>(newRange.lower * 1000);
    std::chrono::milliseconds newRangeLowerMSDuration(newRangeLowerMS);

    // generate a timepoint from newRange upper
    std::chrono::system_clock::time_point tpNewUpper;
    tpNewUpper =
        tpNewUpper + std::chrono::duration_cast<std::chrono::milliseconds>(
                         newRangeUpperMSDuration);
    std::chrono::system_clock::time_point tpNewLower;
    tpNewLower =
        tpNewLower + std::chrono::duration_cast<std::chrono::milliseconds>(
                         newRangeLowerMSDuration);
    // get the upper timepoint based on seconds. to many bit flips with
    // milliseconds precision
    std::chrono::system_clock::time_point tpNewUpperSecs =
        std::chrono::time_point_cast<std::chrono::seconds>(tpNewUpper);
    std::chrono::system_clock::time_point tpNewLowerSecs =
        std::chrono::time_point_cast<std::chrono::seconds>(tpNewLower);

    // calculate delta between upper and lower
    std::chrono::duration<double> deltaSecsUpperLower =
        tpNewUpperSecs - tpNewLowerSecs;

    // get a seconds based timepoint from the last key that was set when data was
    // added
    std::chrono::system_clock::time_point currentDataPointKeySecs =
        std::chrono::time_point_cast<std::chrono::seconds>(
            this->currentDataPointKey);
    std::chrono::system_clock::time_point startPointSecs =
        std::chrono::time_point_cast<std::chrono::seconds>(this->startPoint);

    if (tpNewUpperSecs > currentDataPointKeySecs) {
        this->plot->xAxis->setRange(oldRange);
        return;
    }

    // don't pan to much to the left. startPoint is the lowest possible time key
    if (tpNewUpperSecs < startPointSecs) {
        this->lastRange = oldRange;
        this->plot->xAxis->setRange(oldRange);
        return;
    }

    // limit zoom to 60s --> 3600s
    if (deltaSecsUpperLower < std::chrono::duration<double>(60) ||
        deltaSecsUpperLower > std::chrono::duration<double>(3600)) {
        this->plot->xAxis->setRange(oldRange);
        return;
    }

    // check if we need to toggle autoScroll
    if (this->autoScroll && tpNewUpperSecs < currentDataPointKeySecs) {
        // Stop autoscroll when panning to the left
        this->cbGeneralAutoscrl->setCheckState(Qt::CheckState::Unchecked);
    } else if (!this->autoScroll && tpNewUpperSecs == currentDataPointKeySecs) {
        // Pan to the right edge starts autoscrolling
        this->cbGeneralAutoscrl->setCheckState(Qt::CheckState::Checked);
    }
}

void PlottingArea::mouseMoveHandler(QMouseEvent *event)
{
    // don't do anything if there are no graphs.
    if (this->plot->graphCount() == 0)
        return;
}

void PlottingArea::toolbarActionTriggered(QAction *action)
{
    // set stackedControl to a widget according to action
    if (action == this->actionGeneral) {
        this->controlStack->setCurrentWidget(this->controlGeneralScroll);
    } else if (action == this->actionData) {
        this->controlStack->setCurrentWidget(this->controlDataScroll);
    } else if (action == this->actionAppearance) {
        this->controlStack->setCurrentWidget(this->controlAppearanceScroll);
    }
    if (lastAction == action) {
        if (this->controlStack->maximumHeight() > 0) {
            dynamic_cast<QPropertyAnimation *>(
                this->animationGroupControl->animationAt(0))
                ->setStartValue(120);
            dynamic_cast<QPropertyAnimation *>(
                this->animationGroupControl->animationAt(0))
                ->setEndValue(0);
            dynamic_cast<QPropertyAnimation *>(
                this->animationGroupControl->animationAt(1))
                ->setStartValue(120);
            dynamic_cast<QPropertyAnimation *>(
                this->animationGroupControl->animationAt(1))
                ->setEndValue(0);
            this->animationGroupControl->start();
        } else {
            dynamic_cast<QPropertyAnimation *>(
                this->animationGroupControl->animationAt(0))
                ->setStartValue(0);
            dynamic_cast<QPropertyAnimation *>(
                this->animationGroupControl->animationAt(0))
                ->setEndValue(120);
            dynamic_cast<QPropertyAnimation *>(
                this->animationGroupControl->animationAt(1))
                ->setStartValue(0);
            dynamic_cast<QPropertyAnimation *>(
                this->animationGroupControl->animationAt(1))
                ->setEndValue(120);
            this->animationGroupControl->start();
        }
    } else if (this->controlStack->maximumHeight() == 0) {
        dynamic_cast<QPropertyAnimation *>(
            this->animationGroupControl->animationAt(0))
            ->setStartValue(0);
        dynamic_cast<QPropertyAnimation *>(
            this->animationGroupControl->animationAt(0))
            ->setEndValue(120);
        dynamic_cast<QPropertyAnimation *>(
            this->animationGroupControl->animationAt(1))
            ->setStartValue(0);
        dynamic_cast<QPropertyAnimation *>(
            this->animationGroupControl->animationAt(1))
            ->setEndValue(120);
        this->animationGroupControl->start();
    }
    this->lastAction = action;
}

void PlottingArea::generalCBCheckState(int state)
{
    if (static_cast<Qt::CheckState>(state) == Qt::CheckState::Checked) {
        this->autoScroll = true;
    } else {
        this->autoScroll = false;
    }
}
