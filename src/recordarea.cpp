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

void RecordArea::recordExternal(bool status, QString name)
{
    this->recordToggle->setCheckable(false);
    this->recordToggle->setChecked(status);
    this->recordName->setDisabled(!status);
    if (name != "")
        this->recordName->setText(name);
    this->recordToggle->setCheckable(true);
}

void RecordArea::setupUI()
{
    this->setLayout(new QVBoxLayout());
    QGroupBox *recordBox = new QGroupBox("Record data");
    QGridLayout *recordBoxLayout = new QGridLayout();
    recordBox->setLayout(recordBoxLayout);
    this->layout()->addWidget(recordBox);
    QLabel *recordLabel = new QLabel("Recording: ");
    this->recordToggle = new SwitchButton();
    this->recordToggle->setToolTip("Toggle recording on/off");
    this->recordToggle->setSizePolicy(QSizePolicy::Policy::Fixed,
                                      QSizePolicy::Policy::Fixed);
    recordBoxLayout->addWidget(recordLabel, 0, 0);
    recordBoxLayout->addWidget(this->recordToggle, 0, 1);
    this->recordName = new QLineEdit();
    this->recordName->setPlaceholderText("Set a name for the current recording");
    this->recordName->setToolTip("Set a name for the current recording");
    QDateTime dt = QDateTime::currentDateTime();
    this->recordName->setText(QString("recording_") +
                              dt.toString("yyyy-MM-dd_HH:MM"));
    recordBoxLayout->addWidget(this->recordName, 1, 0, 1, 2);

    QObject::connect(this->recordToggle, &SwitchButton::toggled, this,
                     &RecordArea::recordStateToggled);
}

void RecordArea::recordStateToggled(bool checked)
{
    if (checked) {
        this->recordName->setDisabled(true);
        if (this->recordName->text() != "") {
            emit this->record(checked, this->recordName->text());
        } else {
            // TODO: Show a messagebox!
            QMessageBox::warning(
                this, "Record Name invalid",
                "You have chosen an invalid name for the recording",
                QMessageBox::StandardButton::Ok);
            this->recordToggle->setCheckable(false);
            this->recordToggle->setChecked(false);
            this->recordToggle->setCheckable(true);
            this->recordName->setDisabled(false);
        }
    } else {
        this->recordName->setDisabled(false);
        emit this->record(checked, "");
    }
}
