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

#ifndef CLICKABLELABEL_H
#define CLICKABLELABEL_H

#include <QLabel>
#include <QMouseEvent>
#include <QString>

#include "global.h"

/**
 * @brief The ClickableLabel class provides a QLabel that registers mouse double
 * click events and notifies a listener
 */
class ClickableLabel : public QLabel
{
    Q_OBJECT

public:
    ClickableLabel(QWidget *parent = nullptr, Qt::WindowFlags f = {});
    ClickableLabel(const QString &text, QWidget *parent = nullptr,
                   Qt::WindowFlags f = {});

    void setClickable(bool status);
    bool getClickable();
    void setNoReturnValue(bool status);
    bool getNoReturnValue();

signals:
    void doubleClick(QPoint pos, double value);
    void doubleClickNoValue();

public slots:

private:
    QString originalStylesheet;
    bool clickable;
    bool noReturnValue;

    void initLabel();

    void mouseDoubleClickEvent(QMouseEvent *event) override;

    void enterEvent(QEvent *event) override;
    void leaveEvent(QEvent *event) override;
};

#endif  // CLICKABLELABEL_H
