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

#ifndef GLOBAL
#define GLOBAL

#include <QWidget>
#include <QLayout>
#include <QLayoutItem>

#ifdef __GNUC__
#define ATTR_UNUSED __attribute__((unused))
#else
#define ATTR_UNUSED
#endif

namespace global_constants
{
enum PROTOCOL { KORADV2 };
enum CHANNEL { CHANNEL1 = 1, CHANNEL2, CHANNEL3, CHANNEL4 };
enum MODE { CONSTANT_CURRENT, CONSTANT_VOLTAGE };
enum TRACKING { INDEPENDENT, SERIES, PARALELL };
enum DATATYPE { SETVOLTAGE, VOLTAGE, SETCURRENT, CURRENT, WATTAGE };
enum CONTROL { CONNECT, OCP, OVP, OTP, OUTPUT, SOUND, LOCK, DEVICEID };

const char *const ORANGECOLOR = "#FF7E00";
const char *const GREENCOLOR = "#7BCF06";
}

namespace global_utilities
{
inline void clearLayout(QLayout *layout)
{
    while (QLayoutItem *item = layout->takeAt(0)) {
        if (QWidget *widget = item->widget())
            delete widget;

        //           if (QLayout* childLayout = item->layout())
        //               clearLayout(childLayout, deleteWidgets);
        delete item;
    }
};
}

#endif // GLOBAL
