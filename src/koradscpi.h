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

#ifndef KORADSCPI_H
#define KORADSCPI_H

#include <QObject>
#include <QDebug>
#include <QString>

#include "powersupplyscpi.h"

namespace KoradSCPI_constants
{
namespace powcon = PowerSupplySCPI_constants;
const std::map<int, QString> SERIALCOMMANDMAP = {
    {powcon::SETCURRENT, "ISET%1:%2"},     /**< Set current */
    {powcon::GETCURRENT, "ISET%1?"},       /**< Get current that has been set */
    {powcon::SETVOLTAGE, "VSET%1:%2"},     /**< Set voltage */
    {powcon::GETVOLTAGE, "VSET%1?"},       /**< Get voltage that has been set */
    {powcon::GETACTUALCURRENT, "IOUT%1?"}, /**< Get actual current */
    {powcon::GETACTUALVOLTAGE, "VOUT%1?"}, /**< Get actual Voltage */
    {powcon::SETCHANNELTRACKING,
     "TRACK%1"}, /**< Selects the operation mode: independent, trackingseries,
                    or tracking parallel. */
    {powcon::SETBEEP, "BEEP%1"},         // turn beep on or off
    {powcon::SETOUT, "OUT%1"},           // turn output on or off
    {powcon::GETSTATUS, "STATUS?"},      // request status
    {powcon::GETIDN, "*IDN?"},           // get device identification string
    {powcon::GETSAVEDSETTINGS, "RCL%1"}, // set device to memorized settings
    {powcon::SAVESETTINGS, "SAV%1"}, // save current settings on memory position
    {powcon::SETOCP, "OCP%1"},       // switch over current protection
    {powcon::SETOVP, "OVP%1"},       // switch over voltage protection
    {powcon::SETDUMMY, "DUMMY"},     // just some dummy command
};
}

class KoradSCPI : public PowerSupplySCPI
{

    Q_OBJECT

public:
    /**
     * @brief KoradSPIC
     * @param serialPortName
     * @throw std::runtime_error when Serial Port could not be opened
     */
    KoradSCPI(const QString serialPortName);
    ~KoradSCPI();

    // LabPowerSupply Interface
    void getIdentification();
    void getStatus();
    void setVoltage(const int &channel, const double &value);
    void setCurrent(const int &channel, const double &value);
    void setOCP(bool status);
    void setOVP(bool status);
    void setLocked(bool status);
    void setBeep(bool status);
    void setOutput(bool status);

signals:

private:
    // LabPowerSupply Interface
    QByteArray prepareCommand(const std::shared_ptr<SerialCommand> &com);
    void processStatusCommands(std::shared_ptr<PowerSupplyStatus> &status,
                               const std::shared_ptr<SerialCommand> &com);
};

#endif // KORADSCPI_H
