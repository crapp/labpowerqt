// labpowerqt is a Gui application to control programmable lab power supplies
// Copyright © 2015 Christian Rapp <0x2a at posteo.org>

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

#ifndef SETTINGSDEFINITIONS_H
#define SETTINGSDEFINITIONS_H

namespace settings_constants
{
const char *const MAINWINDOW_GROUP = "mainwindow";
const char *const MAINWINDOW_STATE = "state";
const char *const MAINWINDOW_GEO = "geometry";

const char *const DEVICE_GROUP = "device";
const char *const DEVICE_PROTOCOL = "protocol";
const char *const DEVICE_NAME = "name";
const char *const DEVICE_PORT = "port";
const char *const DEVICE_CHANNELS = "channels";
const char *const DEVICE_CURRENT_MIN = "current_min";
const char *const DEVICE_CURRENT_MAX = "current_max";
const char *const DEVICE_CURRENT_ACCURACY = "current_accuracy";
const char *const DEVICE_VOLTAGE_MIN = "voltage_min";
const char *const DEVICE_VOLTAGE_MAX = "voltage_max";
const char *const DEVICE_VOLTAGE_ACCURACY = "voltage_accuracy";
}
#endif // SETTINGSDEFINITIONS_H
