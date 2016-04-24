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

#ifndef VALUEDOUBLESPINBOX_H
#define VALUEDOUBLESPINBOX_H

#include <QDoubleSpinBox>
#include <QKeyEvent>

/**
 * @brief Own Implementation of a DoubleSpinBox
 *
 * @details
 * Implements a new signal that will be emitted when the enter or return keys are
 * pressed
 */
class ValueDoubleSpinBox : public QDoubleSpinBox
{
    Q_OBJECT

public:
    explicit ValueDoubleSpinBox(QWidget *parent = 0);

signals:
    void enterKeyPressed();

protected:
    void keyPressEvent(QKeyEvent *event)
    {
        QDoubleSpinBox::keyPressEvent(event);
        switch (event->key()) {
        case Qt::Key_Return:
        case Qt::Key_Enter:
            emit this->enterKeyPressed();
            break;
        default:
            break;
        }
    }
};

#endif  // VALUEDOUBLESPINBOX_H
