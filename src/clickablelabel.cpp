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

#include "clickablelabel.h"

namespace globcon = global_constants;

ClickableLabel::ClickableLabel(QWidget *parent, Qt::WindowFlags f)
    : QLabel(parent, f)
{
    this->initLabel();
}

ClickableLabel::ClickableLabel(const QString &text, QWidget *parent,
                               Qt::WindowFlags f)
    : QLabel(text, parent, f)
{
    this->initLabel();
}

void ClickableLabel::setClickable(bool status)
{
    this->clickable = status;
    if (status) {
        this->setCursor(Qt::PointingHandCursor);
    } else {
        this->setCursor(Qt::ForbiddenCursor);
    }
}

bool ClickableLabel::getClickable() { return this->clickable; }

void ClickableLabel::setNoReturnValue(bool status)
{
    this->noReturnValue = status;
}

bool ClickableLabel::getNoReturnValue() { return this->noReturnValue; }

void ClickableLabel::initLabel()
{
    this->setAttribute(Qt::WA_Hover, true);
    this->clickable = false;
    this->setCursor(Qt::ForbiddenCursor);
    this->noReturnValue = false;
    this->setStyleSheet(QString("ClickableLabel {color: ") +
                        globcon::ORANGECOLOR + ";}");
}

void ClickableLabel::mouseDoubleClickEvent(QMouseEvent *event)
{
    if (this->clickable) {
        if (this->noReturnValue) {
            emit this->doubleClickNoValue();
        } else {
            emit this->doubleClick(event->pos(), this->text().toDouble());
        }
    }
}

void ClickableLabel::enterEvent(ATTR_UNUSED QEvent *event)
{
    if (this->clickable) {
        if (this->originalStylesheet == "")
            this->originalStylesheet = this->styleSheet();
        this->setStyleSheet(QString("ClickableLabel {color: ") +
                            globcon::ORANGECOLOR +
                            "; background-color: rgb(82, 82, 82);}");
    }
}

void ClickableLabel::leaveEvent(ATTR_UNUSED QEvent *event)
{
    if (this->clickable) {
        this->setStyleSheet(this->originalStylesheet);
    }
}
