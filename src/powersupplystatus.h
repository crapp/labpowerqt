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

#ifndef POWERSUPPLYSTATUS
#define POWERSUPPLYSTATUS

#include <map>
#include <utility>
#include <memory>
#include <chrono>

#include "global.h"

namespace PowerSupplyStatus_constants
{
typedef std::pair<int, global_constants::MODE> CHANNELMODE;
typedef std::pair<int, double> CHANNELVALUE;
typedef std::pair<int, bool> CHANNELOUTPUT;
}

struct PowerSupplyStatus {

public:
    PowerSupplyStatus()
    {
        this->beeper = false;
        this->locked = false;
        this->ovp = false;
        this->ocp = false;
        this->otp = false;
        this->time = std::chrono::system_clock::now();
    };

    void setBeeper(bool beep) { this->beeper = beep; }
    bool getBeeper() { return this->beeper; }
    void setLocked(bool locked) { this->locked = locked; }
    bool getLocked() { return this->locked; }
    void setOvp(bool ovp) { this->ovp = ovp; }
    bool getOvp() { return this->ovp; }
    void setOcp(bool ocp) { this->ocp = ocp; }
    bool getOcp() { return this->ocp; }
    void setOtp(bool otp) { this->otp = otp; }
    bool getOtp() { return this->otp; }
    void setTime(const std::chrono::system_clock::time_point &t)
    {
        this->time = t;
    }
    std::chrono::system_clock::time_point getTime()
    {
        return this->time;
    }

    void setActualCurrent(const PowerSupplyStatus_constants::CHANNELVALUE &value)
    {
        this->actualCurrent.insert(value);
    }
    /**
     * @brief getActualCurrent Get value for channel
     * @param channel Get value for channel
     * @return actual current as double
     * @throw [std::out_of_range](http://en.cppreference.com/w/cpp/error/out_of_range) if there is no value for the specified channel
     */
    const double &getActualCurrent(const int &channel)
    {
        return this->actualCurrent.at(channel);
    }
    void
    setAdjustedCurrent(const PowerSupplyStatus_constants::CHANNELVALUE &value)
    {
        this->adjustedCurrent.insert(value);
    }
    /**
     * @brief getAdjustedCurrent
     * @param channel
     * @return Adjusted Current as double
     * @throw [std::out_of_range](http://en.cppreference.com/w/cpp/error/out_of_range) if there is no value for the specified channel
     */
    const double &getAdjustedCurrent(const int &channel)
    {
        return this->adjustedCurrent.at(channel);
    }

    void setActualVoltage(const PowerSupplyStatus_constants::CHANNELVALUE &value)
    {
        this->actualVoltage.insert(value);
    }
    /**
     * @brief getActualVoltage
     * @param channel
     * @return Actual Voltage as double
     * @throw [std::out_of_range](http://en.cppreference.com/w/cpp/error/out_of_range) if there is no value for the specified channel
     */
    const double &getActualVoltage(const int &channel)
    {
        return this->actualVoltage.at(channel);
    }

    void
    setAdjustedVoltage(const PowerSupplyStatus_constants::CHANNELVALUE &value)
    {
        this->adjustedVoltage.insert(value);
    }
    /**
     * @brief getAdjustedVoltage Get value for channel
     * @param channel
     * @return Adjusted Voltage as double
     * @throw [std::out_of_range](http://en.cppreference.com/w/cpp/error/out_of_range) if there is no value for the specified channel
     */
    const double &getAdjustedVoltage(const int &channel)
    {
        return this->adjustedVoltage.at(channel);
    }

    void setWattage(const PowerSupplyStatus_constants::CHANNELVALUE &value)
    {
        this->wattage.insert(value);
    }
    const double &getWattage(const int &channel)
    {
        return this->wattage.at(channel);
    }

    void setChannelMode(const PowerSupplyStatus_constants::CHANNELMODE &mode)
    {
        this->channelMode.insert(mode);
    }
    const global_constants::MODE &getChannelMode(const int &channel)
    {
        return this->channelMode.at(channel);
    }

    void
    setChannelOutput(const PowerSupplyStatus_constants::CHANNELOUTPUT &output)
    {
        this->channelOutput.insert(output);
    }
    bool getChannelOutput(const int &channel)
    {
        return this->channelOutput.at(channel);
    }

private:
    bool beeper;
    bool locked;
    bool ovp;
    bool ocp;
    bool otp;

    std::chrono::system_clock::time_point time;

    /**
     * @brief actual_current Map holds actual current for all channels
     */
    std::map<int, double> actualCurrent;
    std::map<int, double> adjustedCurrent;
    /**
     * @brief actual_voltage Map holds actual voltage for all channels
     */
    std::map<int, double> actualVoltage;
    std::map<int, double> adjustedVoltage;

    std::map<int, double> wattage;

    std::map<int, global_constants::MODE> channelMode;

    std::map<int, bool> channelOutput;
};

// Register our metatype. Needed to send this kind of object wrapped in a std
// smart pointer via SIGNAL/SLOT mechanism
Q_DECLARE_METATYPE(std::shared_ptr<PowerSupplyStatus>)

#endif // POWERSUPPLYSTATUS
