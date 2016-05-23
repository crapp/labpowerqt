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

#ifndef SETTINGSDEFINITIONS_H
#define SETTINGSDEFINITIONS_H

/**
 * @brief Constant keys for the QSettings object
 */
namespace settings_constants
{
const char *const MAINWINDOW_GROUP = "mainwindow";
const char *const MAINWINDOW_STATE = "state";
const char *const MAINWINDOW_GEO = "geometry";
const char *const MAINWINDOW_ACTIVE_TAB = "tab";

// settings dialog
// general
const char *const GENERAL_GROUP = "main";
const char *const GENERAL_EXIT = "exit";
const char *const GENERAL_DISC = "disconnect";
// device
const char *const DEVICE_GROUP = "device";
const char *const DEVICE_ACTIVE = "active";
const char *const DEVICE_HASH = "hash";
const char *const DEVICE_NAME = "name";
const char *const DEVICE_PROTOCOL = "protocol";
const char *const DEVICE_PORT = "port";
const char *const DEVICE_PORT_BRATE = "baud";
const char *const DEVICE_PORT_FLOW = "flowctl";
const char *const DEVICE_PORT_DBITS = "databits";
const char *const DEVICE_PORT_PARITY = "parity";
const char *const DEVICE_PORT_SBITS = "stopbits";
const char *const DEVICE_PORT_TIMEOUT = "timeout";
const char *const DEVICE_CHANNELS = "channels";
const char *const DEVICE_CURRENT_MIN = "current_min";
const char *const DEVICE_CURRENT_MAX = "current_max";
const char *const DEVICE_CURRENT_ACCURACY = "current_accuracy";
const char *const DEVICE_VOLTAGE_MIN = "voltage_min";
const char *const DEVICE_VOLTAGE_MAX = "voltage_max";
const char *const DEVICE_VOLTAGE_ACCURACY = "voltage_accuracy";
const char *const DEVICE_POLL_FREQ = "poll_freq";
// plot
const char *const PLOT_GROUP = "plot";
const char *const PLOT_ENABLED = "enabled";
const char *const PLOT_SHOW_DATA = "show_data";
const char *const PLOT_SHOW_GRID = "show_grid";
const char *const PLOT_SHOW_LEGEND = "show_legend";
const char *const PLOT_SHOW_TIMESCALE = "show_legend";
const char *const PLOT_GRAPH_VISIBLE = "graph_%1_visible";
const char *const PLOT_GRAPH_COLOR = "graph_%1_color";
const char *const PLOT_GRAPH_LS = "graph_%1_ls";
const char *const PLOT_GRAPH_LINE = "graph_%1_line";
const char *const PLOT_ZOOM_MIN = "zoom_min";
const char *const PLOT_ZOOM_MAX = "zoom_max";
// record
const char *const RECORD_GROUP = "record";
const char *const RECORD_SQLPATH = "sqlpath";
const char *const RECORD_TBLPRE = "tblprefix";
const char *const RECORD_BUFFER = "buffersize";
// log
const char *const LOG_GROUP = "logging";
const char *const LOG_ENABLED = "enabled";
const char *const LOG_DIRECTORY = "directory";
const char *const LOG_MIN_SEVERITY = "severity";
}
#endif  // SETTINGSDEFINITIONS_H
