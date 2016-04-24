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

#include "displayarea.h"

namespace globcon = global_constants;
namespace utils = global_utilities;
namespace setcon = settings_constants;

DisplayArea::DisplayArea(QWidget *parent) : QWidget(parent)
{
    this->valuesDialog = nullptr;
    this->valuesDialogData = nullptr;
    this->setupUI();
    this->setupChannels();
}

void DisplayArea::setValuesDialog(
    std::shared_ptr<FloatingValuesDialogData> valuesDialogData,
    std::shared_ptr<FloatingValuesDialog> valuesDialog)
{
    this->valuesDialog = valuesDialog;
    this->valuesDialogData = valuesDialogData;
}

void DisplayArea::dataUpdate(QVariant val, global_constants::DATATYPE dt,
                             int channel)
{
    switch (dt) {
    case globcon::DATATYPE::SETVOLTAGE:
        this->chanwVector.at(channel - 1)
            ->voltageSet->setText(std::move(val.toString()));
        break;
    case globcon::DATATYPE::VOLTAGE:
        this->chanwVector.at(channel - 1)
            ->voltageActual->setText(std::move(val.toString()));
        break;
    case globcon::DATATYPE::SETCURRENT:
        this->chanwVector.at(channel - 1)
            ->currentSet->setText(std::move(val.toString()));
        break;
    case globcon::DATATYPE::CURRENT:
        this->chanwVector.at(channel - 1)
            ->currentActual->setText(std::move(val.toString()));
        break;
    case globcon::DATATYPE::WATTAGE:
        this->chanwVector.at(channel - 1)
            ->wattageActual->setText(std::move(val.toString()));
        break;
    }
}

void DisplayArea::dataUpdate(QVariant val, global_constants::CONTROL ct,
                             int channel)
{
    switch (ct) {
    case globcon::CONTROL::DEVICEID:
        this->labelDeviceName->setText(val.toString());
        break;
    case globcon::CONTROL::CONNECT:
        if (val.toBool()) {
            this->labelConnect->setPixmap(QPixmap(":/icons/plug_in_orange"));
        } else {
            this->labelConnect->setPixmap(QPixmap(":/icons/plug_out_orange"));
        }
        this->controlStateEnabled(val.toBool());
        break;
    case globcon::CONTROL::SOUND:
        this->labelSound->setPixmap(QPixmap(val.toString()));
        break;
    case globcon::CONTROL::LOCK:
        this->labelSound->setPixmap(QPixmap(val.toString()));
        break;
    case globcon::CONTROL::OUTPUT:
        this->chanwVector.at(channel - 1)->outputSet->setText(val.toString());
        break;
    case globcon::CONTROL::OVP:
        this->labelOVPSet->setText(val.toString());
        break;
    case globcon::CONTROL::OCP:
        this->labelOCPSet->setText(val.toString());
        break;
    case globcon::CONTROL::OTP:
        this->labelOTPSet->setText(val.toString());
        break;
    }
}

void DisplayArea::dataUpdate(global_constants::MODE md, int channel)
{
    switch (md) {
    case globcon::MODE::CONSTANT_VOLTAGE:
        this->chanwVector.at(channel - 1)->modeActual->setText("CV");
        break;
    case globcon::MODE::CONSTANT_CURRENT:
        this->chanwVector.at(channel - 1)->modeActual->setText("CC");
        break;
    }
}

// The following two functions are very big. Lots of code is necessay for
// dynamically build guis.
void DisplayArea::setupChannels()
{
    this->labelConnect->setClickable(false);
    QSettings settings;
    settings.beginGroup(setcon::DEVICE_GROUP);
    settings.beginGroup(settings.value(setcon::DEVICE_ACTIVE).toString());

    if (settings.contains(setcon::DEVICE_PORT)) {
        // if there is a device we must enable the connect button.
        this->labelConnect->setClickable(true);
        // TODO: One could make this more intelligent and only create / delete
        // channels that are (no longer) needed.

        // remove all channels from the frame if there are any. Happens if the
        // user changes the device specs or chooses another one.
        for (auto f : this->channelFramesVec) {
            utils::clearLayout(f->layout());
            this->frameChannels->layout()->removeWidget(f);
            delete (f);
        }
        // clear the vector
        this->channelFramesVec.clear();
        this->chanwVector.clear();

        // create the frames that control one channel
        for (int i = 1; i <= settings.value(setcon::DEVICE_CHANNELS).toInt();
             i++) {
            std::shared_ptr<ChannelWidgets> chanw =
                std::make_shared<ChannelWidgets>();

            QFrame *channelFrameContainer = new QFrame();
            channelFrameContainer->setLayout(new QVBoxLayout());
            channelFrameContainer->setObjectName("ch" + QString::number(i) +
                                                 "Container");
            // this is the container frame
            QString frameQss = QString("QFrame#%1 {border: 1px solid ") +
                               globcon::GREENCOLOR + ";}";
            channelFrameContainer->setStyleSheet(
                frameQss.arg(channelFrameContainer->objectName()));
            channelFrameContainer->layout()->setSpacing(0);
            channelFrameContainer->layout()->setContentsMargins(QMargins());

            this->frameChannels->layout()->addWidget(channelFrameContainer);

            // now the frame for the electrical values V, A, W.

            QFrame *vawFrame = new QFrame();
            vawFrame->setObjectName("ch" + QString::number(i) + "vawFrame");
            QString vawFrameQss =
                QString("QFrame#%1 {border-bottom: 1px solid ") +
                globcon::GREENCOLOR + ";}";
            vawFrame->setStyleSheet(vawFrameQss.arg(vawFrame->objectName()));
            QGridLayout *vawLayout = new QGridLayout();
            vawFrame->setLayout(vawLayout);
            channelFrameContainer->layout()->addWidget(vawFrame);

            vawLayout->addWidget(new QLabel("CH" + QString::number(i)), 0, 0, 1,
                                 0, Qt::AlignLeft);

            chanw->voltageActual = new QLabel("0.00");
            QFont actualFont = chanw->voltageActual->font();
            actualFont.setPointSize(20);
            chanw->voltageActual->setAlignment(Qt::AlignRight |
                                               Qt::AlignVCenter);
            chanw->voltageActual->setStyleSheet(
                QString("QLabel {padding-right: 0.5em;}"));
            chanw->voltageActual->setFont(actualFont);
            chanw->currentActual = new QLabel("0.000");
            chanw->currentActual->setAlignment(Qt::AlignRight |
                                               Qt::AlignVCenter);
            chanw->currentActual->setStyleSheet(
                QString("QLabel {padding-right: 0.5em;}"));
            chanw->currentActual->setFont(actualFont);
            chanw->wattageActual = new QLabel("0.000");
            chanw->wattageActual->setAlignment(Qt::AlignRight |
                                               Qt::AlignVCenter);
            chanw->wattageActual->setStyleSheet(
                QString("QLabel {padding-right: 0.5em;}"));
            chanw->wattageActual->setFont(actualFont);
            vawLayout->addWidget(chanw->voltageActual, 1, 0);
            vawLayout->addWidget(chanw->currentActual, 2, 0);
            vawLayout->addWidget(chanw->wattageActual, 3, 0);

            QLabel *vActualUnit = new QLabel("V");
            vActualUnit->setSizePolicy(QSizePolicy::Policy::Fixed,
                                       QSizePolicy::Policy::Preferred);
            vActualUnit->setAlignment(Qt::AlignTop);
            QLabel *aActualUnit = new QLabel("A");
            aActualUnit->setSizePolicy(QSizePolicy::Policy::Fixed,
                                       QSizePolicy::Policy::Preferred);
            aActualUnit->setAlignment(Qt::AlignTop);
            QLabel *wActualUnit = new QLabel("W");
            wActualUnit->setSizePolicy(QSizePolicy::Policy::Fixed,
                                       QSizePolicy::Policy::Preferred);
            wActualUnit->setAlignment(Qt::AlignTop);
            vawLayout->addWidget(vActualUnit, 1, 1);
            vawLayout->addWidget(aActualUnit, 2, 1);
            vawLayout->addWidget(wActualUnit, 3, 1);

            QFrame *bottomContainer = new QFrame();
            bottomContainer->setLayout(new QHBoxLayout());
            channelFrameContainer->layout()->addWidget(bottomContainer);
            bottomContainer->layout()->setSpacing(0);
            bottomContainer->layout()->setContentsMargins(QMargins());

            QFrame *setContainer = new QFrame();
            setContainer->setObjectName("ch" + QString::number(i) +
                                        "setContainer");
            QGridLayout *setContLayout = new QGridLayout();
            setContainer->setLayout(setContLayout);
            QString setContainerQss =
                QString("QFrame#%1 {border-right: 1px solid ") +
                globcon::GREENCOLOR + ";}";
            setContainer->setStyleSheet(
                setContainerQss.arg(setContainer->objectName()));
            chanw->voltageSet = new ClickableLabel("0.00");
            chanw->currentSet = new ClickableLabel("0.000");
            chanw->voltageSet->setAlignment(Qt::AlignRight | Qt::AlignVCenter);
            chanw->currentSet->setAlignment(Qt::AlignRight | Qt::AlignVCenter);

            // Now the lambdas for the set widgets
            QObject::connect(
                chanw->voltageSet, &ClickableLabel::doubleClick,
                [this, chanw, i](QPoint pos, double value) {
                    this->controlValuesDialog(
                        std::move(pos), chanw->voltageSet,
                        global_constants::DATATYPE::SETVOLTAGE, value);
                    if (this->valuesDialog->exec()) {
                        emit this->doubleValueChanged(
                            this->valuesDialogData->voltage,
                            static_cast<int>(
                                global_constants::DATATYPE::SETVOLTAGE),
                            i);
                    }
                });
            QObject::connect(
                chanw->currentSet, &ClickableLabel::doubleClick,
                [this, chanw, i](QPoint pos, double value) {
                    this->controlValuesDialog(
                        std::move(pos), chanw->currentSet,
                        global_constants::DATATYPE::SETCURRENT, value);
                    if (this->valuesDialog->exec()) {
                        emit this->doubleValueChanged(
                            this->valuesDialogData->current,
                            static_cast<int>(
                                global_constants::DATATYPE::SETCURRENT),
                            i);
                    }
                });

            QLabel *setLabel = new QLabel("Set");
            setLabel->setAlignment(Qt::AlignCenter);
            QLabel *voltageSetUnit = new QLabel("V");
            QFont bottomUnitFont = voltageSetUnit->font();
            bottomUnitFont.setPointSize(8);
            voltageSetUnit->setFont(bottomUnitFont);
            voltageSetUnit->setSizePolicy(QSizePolicy::Policy::Fixed,
                                          QSizePolicy::Policy::Preferred);
            voltageSetUnit->setAlignment(Qt::AlignTop);
            QLabel *currentSetUnit = new QLabel("A");
            currentSetUnit->setFont(bottomUnitFont);
            currentSetUnit->setSizePolicy(QSizePolicy::Policy::Fixed,
                                          QSizePolicy::Policy::Preferred);
            currentSetUnit->setAlignment(Qt::AlignTop);
            setContLayout->addWidget(setLabel, 0, 0, 1, 0);
            setContLayout->addWidget(chanw->voltageSet, 1, 0);
            setContLayout->addWidget(voltageSetUnit, 1, 1);
            setContLayout->addWidget(chanw->currentSet, 2, 0);
            setContLayout->addWidget(currentSetUnit, 2, 1);

            QFrame *modeOutputContainer = new QFrame();
            modeOutputContainer->setLayout(new QVBoxLayout());
            modeOutputContainer->layout()->setSpacing(0);
            modeOutputContainer->layout()->setContentsMargins(QMargins());

            QFrame *modeContainer = new QFrame();
            modeContainer->setLayout(new QHBoxLayout());
            modeContainer->setObjectName("ch" + QString::number(i) +
                                         "modeContainer");
            QString modeContainerQss =
                QString("QFrame#%1 {border-bottom: 1px solid ") +
                globcon::GREENCOLOR + ";}";
            modeContainer->setStyleSheet(
                modeContainerQss.arg(modeContainer->objectName()));
            QLabel *modeLabel = new QLabel("Mode");
            chanw->modeActual = new QLabel("CV");
            modeContainer->layout()->addWidget(modeLabel);
            modeContainer->layout()->addWidget(chanw->modeActual);

            QFrame *outputContainer = new QFrame();
            outputContainer->setLayout(new QHBoxLayout());
            QLabel *outputLabel = new QLabel("Output");
            chanw->outputSet = new ClickableLabel("Off");
            chanw->outputSet->setNoReturnValue(true);
            QObject::connect(
                chanw->outputSet, &ClickableLabel::doubleClickNoValue,
                [this, chanw, i]() {
                    emit this->deviceControlValueChanged(
                        static_cast<int>(global_constants::CONTROL::OUTPUT), i);
                });

            outputContainer->layout()->addWidget(outputLabel);
            outputContainer->layout()->addWidget(chanw->outputSet);

            modeOutputContainer->layout()->addWidget(modeContainer);
            modeOutputContainer->layout()->addWidget(outputContainer);

            bottomContainer->layout()->addWidget(setContainer);
            bottomContainer->layout()->addWidget(modeOutputContainer);

            this->channelFramesVec.push_back(channelFrameContainer);
            // move semantics are needed here because unique_ptr does not allow
            // copying what push_back actually does.
            this->chanwVector.push_back(std::move(chanw));
        }
    }
}

void DisplayArea::setupUI()
{
    this->setLayout(new QGridLayout());
    this->setStyleSheet("background-color: rgb(47, 47, 47); color: " +
                        QString(globcon::GREENCOLOR) + "; \n" + " QToolTip {}");
    this->setAutoFillBackground(true);
    this->layout()->setSpacing(0);
    QMargins layMargins = this->layout()->contentsMargins();
    layMargins.setBottom(0);
    layMargins.setTop(0);
    this->layout()->setContentsMargins(layMargins);

    // init header
    this->frameHeader = new QFrame();
    this->frameHeader->setLayout(new QHBoxLayout());
    dynamic_cast<QGridLayout *>(this->layout())
        ->addWidget(this->frameHeader, 0, 0);

    this->labelDeviceName = new QLabel();
    this->labelDeviceName->setText("Unknown Device");
    this->frameHeader->layout()->addWidget(this->labelDeviceName);

    dynamic_cast<QHBoxLayout *>(this->frameHeader->layout())->addStretch();

    this->labelConnect = new ClickableLabel();
    this->labelConnect->setPixmap(QPixmap(":/icons/plug_out_orange.png"));
    this->labelConnect->setMinimumWidth(37);
    this->labelConnect->setToolTip("Disconnected");
    this->labelConnect->setNoReturnValue(true);
    this->labelConnect->setAlignment(Qt::AlignCenter);
    this->frameHeader->layout()->addWidget(this->labelConnect);
    QObject::connect(this->labelConnect, &ClickableLabel::doubleClickNoValue,
                     [this]() {
                         emit this->deviceControlValueChanged(
                             static_cast<int>(globcon::CONTROL::CONNECT), 0);
                     });

    this->labelSound = new ClickableLabel();
    this->labelSound->setPixmap(QPixmap(":/icons/speaker_orange.png"));
    this->labelSound->setMinimumWidth(25);
    this->labelSound->setToolTip("Sound on");
    this->labelSound->setNoReturnValue(true);
    this->labelSound->setAlignment(Qt::AlignCenter);
    this->frameHeader->layout()->addWidget(this->labelSound);

    this->labelLock = new ClickableLabel();
    this->labelLock->setPixmap(QPixmap(":/icons/lock-open_orange.png"));
    this->labelLock->setMinimumSize(QSize(25, 16));
    this->labelLock->setToolTip("Device Controls unlocked");
    this->labelLock->setNoReturnValue(true);
    this->labelLock->setAlignment(Qt::AlignCenter);
    this->frameHeader->layout()->addWidget(this->labelLock);

    this->headerControls = {this->labelConnect, this->labelSound,
                            this->labelLock};

    // init channels
    this->frameChannels = new QFrame();
    this->frameChannels->setLayout(new QHBoxLayout());
    this->frameChannels->layout()->setSpacing(0);
    this->frameChannels->layout()->setContentsMargins(QMargins());
    dynamic_cast<QGridLayout *>(this->layout())
        ->addWidget(this->frameChannels, 1, 0);
    this->frameChannels->setMinimumHeight(200);

    // init footer
    this->frameFooter = new QFrame();
    this->frameFooter->setLayout(new QHBoxLayout());
    this->frameFooter->layout()->setContentsMargins(0, 0, 0, 0);
    this->frameFooter->layout()->setSpacing(0);

    dynamic_cast<QGridLayout *>(this->layout())
        ->addWidget(this->frameFooter, 2, 0);

    QFrame *frameOVP = new QFrame();
    frameOVP->setLayout(new QHBoxLayout());
    frameOVP->layout()->addWidget(new QLabel("OVP:"));
    this->labelOVPSet = new ClickableLabel();
    this->labelOVPSet->setText("Off");
    this->labelOVPSet->setNoReturnValue(true);
    this->labelOVPSet->setMinimumWidth(20);
    frameOVP->layout()->addWidget(this->labelOVPSet);
    this->frameFooter->layout()->addWidget(frameOVP);
    QObject::connect(this->labelOVPSet, &ClickableLabel::doubleClickNoValue,
                     [this]() {
                         emit this->deviceControlValueChanged(
                             static_cast<int>(globcon::CONTROL::OVP), 0);
                     });

    QFrame *frameOCP = new QFrame();
    frameOCP->setLayout(new QHBoxLayout());
    frameOCP->layout()->addWidget(new QLabel("OCP:"));
    this->labelOCPSet = new ClickableLabel();
    this->labelOCPSet->setText("Off");
    this->labelOCPSet->setNoReturnValue(true);
    this->labelOCPSet->setMinimumWidth(20);
    frameOCP->layout()->addWidget(this->labelOCPSet);
    this->frameFooter->layout()->addWidget(frameOCP);
    QObject::connect(this->labelOCPSet, &ClickableLabel::doubleClickNoValue,
                     [this]() {
                         emit this->deviceControlValueChanged(
                             static_cast<int>(globcon::CONTROL::OCP), 0);
                     });

    QFrame *frameOTP = new QFrame();
    frameOTP->setLayout(new QHBoxLayout());
    frameOTP->layout()->addWidget(new QLabel("OTP:"));
    this->labelOTPSet = new ClickableLabel();
    this->labelOTPSet->setText("Off");
    this->labelOTPSet->setNoReturnValue(true);
    this->labelOTPSet->setMinimumWidth(20);
    frameOTP->layout()->addWidget(this->labelOTPSet);
    this->frameFooter->layout()->addWidget(frameOTP);
    QObject::connect(this->labelOTPSet, &ClickableLabel::doubleClickNoValue,
                     [this]() {
                         emit this->deviceControlValueChanged(
                             static_cast<int>(globcon::CONTROL::OTP), 0);
                     });

    QFrame *frameTracking = new QFrame();
    frameTracking->setLayout(new QHBoxLayout());
    frameTracking->layout()->addWidget(new QLabel("Tracking:"));
    this->labelTrackingSet = new ClickableLabel();
    this->labelTrackingSet->setText("None");
    frameTracking->layout()->addWidget(this->labelTrackingSet);
    this->frameFooter->layout()->addWidget(frameTracking);

    dynamic_cast<QHBoxLayout *>(frameTracking->layout())->addStretch();
}

void DisplayArea::controlValuesDialog(QPoint pos, QWidget *clickedWidget,
                                      global_constants::DATATYPE dt,
                                      double currentValue)
{
    // map widget cursor position to global position (top left)
    QPoint globalPos = clickedWidget->mapToGlobal(pos);
    // move the dialog to the calculated position
    this->valuesDialog->move(globalPos.x(), globalPos.y());
    this->valuesDialog->setDatatype(dt);
    // one must use this after setDataype. Maybe this is not so clever.
    this->valuesDialog->setCurrentValue(currentValue);
}

void DisplayArea::controlStateEnabled(bool state)
{
    // TODO: It would be easier to disable the container frames.
    this->labelLock->setClickable(state);
    this->labelSound->setClickable(state);
    this->labelOCPSet->setClickable(state);
    this->labelOVPSet->setClickable(state);
    this->labelOTPSet->setClickable(state);
    for (auto chanw : this->chanwVector) {
        chanw->outputSet->setClickable(state);
        chanw->voltageSet->setClickable(state);
        chanw->currentSet->setClickable(state);
    }
}

void DisplayArea::paintEvent(ATTR_UNUSED QPaintEvent *event)
{
    QStyleOption o;
    o.initFrom(this);
    QPainter p(this);
    style()->drawPrimitive(QStyle::PE_Widget, &o, &p, this);
}
