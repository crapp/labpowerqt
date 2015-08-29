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

#include "clickablelabel.h"

ClickableLabel::ClickableLabel(QWidget *parent, Qt::WindowFlags f)
    : QLabel(parent, f)
{
    this->setAttribute(Qt::WA_Hover, true);
}

ClickableLabel::ClickableLabel(const QString &text, QWidget *parent,
                               Qt::WindowFlags f)
    : QLabel(text, parent, f)
{
    this->setAttribute(Qt::WA_Hover, true);
}

void ClickableLabel::mouseDoubleClickEvent(QMouseEvent *event)
{
    emit this->doubleClick(event->pos());
}

void ClickableLabel::enterEvent(QEvent *event)
{
    if(this->originalStylesheet == "")
        this->originalStylesheet = this->styleSheet();
    this->setStyleSheet("background-color: rgb(82, 82, 82);");
}

void ClickableLabel::leaveEvent(QEvent *event)
{
    this->setStyleSheet(this->originalStylesheet);
}
