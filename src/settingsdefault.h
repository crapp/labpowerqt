// labpowerqt is a Gui application to control programmable lab power supplies
// Copyright © 2015, 2016 Christian Rapp <0x2a at posteo dot org>

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

#ifndef SETTINGSDEFAULT_H
#define SETTINGSDEFAULT_H

#include <QStandardPaths>
#include <QVariant>

#include <map>
#include <string>

#include "settingsdefinitions.h"

/**
 * @brief Default settings values in a lookup map structure
 */
namespace settings_default
{
const std::map<const char* const, QVariant> general_defaults{
    {settings_constants::GENERAL_EXIT, QVariant(true)},
    {settings_constants::GENERAL_DISC, QVariant(false)},
    {settings_constants::PLOT_ZOOM_MIN, QVariant(60)},
    {settings_constants::PLOT_ZOOM_MAX, QVariant(1800)},
    {settings_constants::RECORD_BUFFER, QVariant(60)},
    {settings_constants::LOG_ENABLED, QVariant(true)},
    {settings_constants::LOG_MIN_SEVERITY, QVariant(1)}};
}

#endif /* SETTINGSDEFAULT_H */
