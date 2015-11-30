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

#include "recordarea.h"

RecordArea::RecordArea(QWidget *parent) : QWidget(parent) { this->setupUI(); }

void RecordArea::setupUI()
{
    this->setLayout(new QVBoxLayout());
    this->recordBox = new QGroupBox("Record data");
    this->recordBoxLayout = new QGridLayout();
    this->recordBox->setLayout(this->recordBoxLayout);
    this->recordBox->setCheckable(true);
    this->recordBox->setChecked(false);
    this->layout()->addWidget(this->recordBox);
    this->recordName = new QLineEdit();
    this->recordBoxLayout->addWidget(this->recordName, 0, 0,
                                     Qt::AlignmentFlag::AlignTop);
}
