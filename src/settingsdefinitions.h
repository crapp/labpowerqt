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

#ifndef SETTINGSDEFINITIONS_H
#define SETTINGSDEFINITIONS_H

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
const char *const DEVICE_CHANNELS = "channels";
const char *const DEVICE_CURRENT_MIN = "current_min";
const char *const DEVICE_CURRENT_MAX = "current_max";
const char *const DEVICE_CURRENT_ACCURACY = "current_accuracy";
const char *const DEVICE_VOLTAGE_MIN = "voltage_min";
const char *const DEVICE_VOLTAGE_MAX = "voltage_max";
const char *const DEVICE_VOLTAGE_ACCURACY = "voltage_accuracy";
// plot
const char *const PLOT_GROUP = "plot";
const char *const PLOT_SHOW_DATA = "show_data";
const char *const PLOT_SHOW_GRID = "show_grid";
const char *const PLOT_SHOW_LEGEND = "show_legend";
const char *const PLOT_GROUP_CHANNEL = "channel_";
const char *const PLOT_GRAPH_V_VISIBLE = "voltage_visible";
const char *const PLOT_GRAPH_V_COLOR = "voltage_color";
const char *const PLOT_GRAPH_V_LS = "voltage_ls"; /**< line style */
const char *const PLOT_GRAPH_V_LINE = "voltage_line"; /**< line thickness */
const char *const PLOT_GRAPH_VS_VISIBLE = "voltageset_visible";
const char *const PLOT_GRAPH_VS_COLOR = "voltageset_color";
const char *const PLOT_GRAPH_VS_LS = "voltageset_ls"; /**< line style */
const char *const PLOT_GRAPH_VS_LINE = "voltageset_line"; /**< line thickness */
const char *const PLOT_GRAPH_A_VISIBLE = "current_visible";
const char *const PLOT_GRAPH_A_COLOR = "current_color";
const char *const PLOT_GRAPH_A_LS = "current_ls"; /**< line style */
const char *const PLOT_GRAPH_A_LINE = "current_line"; /**< line thickness */
const char *const PLOT_GRAPH_AS_VISIBLE = "currentset_visible";
const char *const PLOT_GRAPH_AS_COLOR = "currentset_color";
const char *const PLOT_GRAPH_AS_LS = "currentset_ls"; /**< line style */
const char *const PLOT_GRAPH_AS_LINE = "currentset_line"; /**< line thickness */
const char *const PLOT_GRAPH_W_VISIBLE = "wattage_visible";
const char *const PLOT_GRAPH_W_COLOR = "wattage_color";
const char *const PLOT_GRAPH_W_LS = "wattage_ls"; /**< line style */
const char *const PLOT_GRAPH_W_LINE = "wattage_line"; /**< line thickness */
// record
const char *const RECORD_GROUP = "record";
const char *const RECORD_SQLPATH = "sqlpath";
const char *const RECORD_TBLPRE = "tblprefix";
const char *const RECORD_BUFFER = "buffersize";
}
#endif // SETTINGSDEFINITIONS_H
