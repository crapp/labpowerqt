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

#include "displayarea.h"

namespace globcon = global_constants;
namespace setcon = settings_constants;

DisplayArea::DisplayArea(QWidget *parent) : QWidget(parent)
{
    this->setupUI();
    this->setupChannels();
}

void DisplayArea::setupChannels()
{
    QSettings settings;
    settings.beginGroup(setcon::DEVICE_GROUP);
    if (settings.contains(setcon::DEVICE_PORT)) {
        for (int i = 1; i <= settings.value(setcon::DEVICE_CHANNELS).toInt();
             i++) {
            QFrame *channelFrame = new QFrame();
            this->frameChannels->layout()->addWidget(channelFrame);
            channelFrame->setStyleSheet(QString("border: 1px solid ") +
                                        globcon::GREENCOLOR + ";");
            channelFrame->setMinimumSize(QSize(50, 50));
        }
    }
}

void DisplayArea::setupUI()
{

    this->setLayout(new QGridLayout());
    this->setStyleSheet("background-color: rgb(47, 47, 47); color: " +
                        QString(globcon::GREENCOLOR) + ";");
    this->setAutoFillBackground(true);

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
    this->frameHeader->layout()->addWidget(this->labelConnect);

    this->labelSound = new ClickableLabel();
    this->labelSound->setPixmap(QPixmap(":/icons/speaker_orange.png"));
    this->frameHeader->layout()->addWidget(this->labelSound);

    this->labelConnect = new ClickableLabel();
    this->labelConnect->setPixmap(QPixmap(":/icons/lock-open_orange.png"));
    this->frameHeader->layout()->addWidget(this->labelConnect);

    this->headerControls = {this->labelConnect, this->labelSound};

    // init channels
    this->frameChannels = new QFrame();
    this->frameChannels->setLayout(new QHBoxLayout());
    dynamic_cast<QGridLayout *>(this->layout())
        ->addWidget(this->frameChannels, 1, 0);

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
    frameOVP->layout()->addWidget(this->labelOVPSet);
    this->frameFooter->layout()->addWidget(frameOVP);

    QFrame *frameOCP = new QFrame();
    frameOCP->setLayout(new QHBoxLayout());
    frameOCP->layout()->addWidget(new QLabel("OCP:"));
    this->labelOCPSet = new ClickableLabel();
    this->labelOCPSet->setText("Off");
    frameOCP->layout()->addWidget(this->labelOCPSet);
    this->frameFooter->layout()->addWidget(frameOCP);

    QFrame *frameOTP = new QFrame();
    frameOTP->setLayout(new QHBoxLayout());
    frameOTP->layout()->addWidget(new QLabel("OTP:"));
    this->labelOTPSet = new ClickableLabel();
    this->labelOTPSet->setText("Off");
    frameOTP->layout()->addWidget(this->labelOTPSet);
    this->frameFooter->layout()->addWidget(frameOTP);

    QFrame *frameTracking = new QFrame();
    frameTracking->setLayout(new QHBoxLayout());
    frameTracking->layout()->addWidget(new QLabel("Tracking:"));
    this->labelTrackingSet = new ClickableLabel();
    this->labelTrackingSet->setText("None");
    frameTracking->layout()->addWidget(this->labelTrackingSet);
    this->frameFooter->layout()->addWidget(frameTracking);

    dynamic_cast<QHBoxLayout *>(frameTracking->layout())->addStretch();
}

void DisplayArea::paintEvent(QPaintEvent *event)
{
    QStyleOption o;
    o.initFrom(this);
    QPainter p(this);
    style()->drawPrimitive(QStyle::PE_Widget, &o, &p, this);
}
