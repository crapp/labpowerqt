// labpowerqt is a Gui application to control programmable lab power supplies
// Copyright © 2015 Christian Rapp <0x2a at posteo dot org>

// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.

// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.

// You should have received a copy of the GNU General Public License
// along with this program.  If not, see <http://www.gnu.org/licenses/>.

#ifndef RECORDAREA_H
#define RECORDAREA_H

#include <QWidget>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGridLayout>
#include <QGroupBox>
#include <QLineEdit>
#include <QPushButton>
#include <QSettings>
#include <QDebug>

#include "global.h"

class RecordArea : public QWidget
{
    Q_OBJECT
public:
    explicit RecordArea(QWidget *parent = 0);

signals:

public slots:
    /**
     * @brief Tell the Record Area there is a working device config
     * @param status
     */
    void canRecord(bool status);

private:

    QGroupBox *recordBox;
    QGridLayout *recordBoxLayout;
    QLineEdit *recordName;
    QPushButton *setRecordName;

    void setupUI();
};

#endif // RECORDAREA_H
