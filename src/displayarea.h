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

#ifndef DISPLAYAREA_H
#define DISPLAYAREA_H

#include <QWidget>
#include <QGridLayout>
#include <QHBoxLayout>
#include <QFrame>
#include <QLabel>
#include <QPixmap>
#include <QPainter>
#include <QStyleOption>
#include <QSettings>

#include <vector>

#include "global.h"
#include "settingsdefinitions.h"
#include "clickablelabel.h"

class DisplayArea : public QWidget
{
    Q_OBJECT
public:
    explicit DisplayArea(QWidget *parent = 0);

signals:

public slots:

    void setupChannels();

private:
    QFrame *frameHeader;
    QLabel *labelDeviceName;
    ClickableLabel *labelConnect;
    ClickableLabel *labelSound;
    ClickableLabel *labelLock;

    std::vector<ClickableLabel *> headerControls;

    QFrame *frameChannels;

    QFrame *frameFooter;
    ClickableLabel *labelOVPSet;
    ClickableLabel *labelOCPSet;
    ClickableLabel *labelOTPSet;
    ClickableLabel *labelTrackingSet;

    std::vector<ClickableLabel *> footerControls;

    void setupUI();
    void setupChannels();

    /**
     * @brief paintEvent Reimplement paintEvent to use stylesheets in derived Widgets
     * @param event
     */
    void paintEvent(QPaintEvent * event);
};

#endif // DISPLAYAREA_H
