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

#ifndef DEVICEWIZARD_H
#define DEVICEWIZARD_H

#include <QWidget>
#include <QWizard>

#include "devicewizardintro.h"
#include "devicewizardoptions.h"
#include "devicewizardconnection.h"
#include "devicewizardfinal.h"
#include "settingsdefinitions.h"

class DeviceWizard : public QWizard
{

    Q_OBJECT

public:
    DeviceWizard(QWidget *parent = 0);

    void accept() Q_DECL_OVERRIDE;
};

#endif // DEVICEWIZARD_H
