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

#include "devicewizardintro.h"

DeviceWizardIntro::DeviceWizardIntro(QWidget *parent) : QWizardPage(parent)
{
    this->setTitle("Add Power Supply");
    this->setSubTitle(tr("Add a new Power Supply."));
    this->setPixmap(QWizard::LogoPixmap, QPixmap(":/icons/device32.png"));

    QLabel *infotext = new QLabel();
    infotext->setWordWrap(true);

    infotext->setText(
        "You can use this wizard to add a new Power Supply Device. "
        "Make sure you have the appropriate permissions to access the device.");
    this->setLayout(new QVBoxLayout());
    this->layout()->addWidget(infotext);
}
