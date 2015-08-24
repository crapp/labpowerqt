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

namespace PowerSupplyStatus_constants
{
enum MODE { CONSTANT_CURRENT, CONSTANT_VOLTAGE };
enum TRACKING { INDEPENDENT, SERIES, PARALELL };
typedef std::pair<int, double> CHANNELVALUE;
}

struct PowerSupplyStatus {

public:
    PowerSupplyStatus(){};

    void setBeeper(bool beep) { this->beeper = beep; }
    bool getBeeper() { return this->beeper; }
    void setLocked(bool locked) { this->locked = locked; }
    bool getLocked() { return this->output; }
    void setOutput(bool output) { this->output = output; }
    bool getOuput() { return this->output; }
    void setOvp(bool ovp) { this->ovp = ovp; }
    bool getOvp() { return this->ovp; }
    void setOcp(bool ocp) { this->ocp = ocp; }
    bool getOcp() { return this->ocp; }

    void setActualCurrent(const PowerSupplyStatus_constants::CHANNELVALUE &value)
    {
        this->actual_current.insert(value);
    }
    /**
     * @brief getActualCurrent Get value for channel
     * @param channel Get value for channel
     * @return actual current as double
     * @throw [std::out_of_range](http://en.cppreference.com/w/cpp/error/out_of_range) if there is no value for the specified channel
     */
    const double &getActualCurrent(const int &channel)
    {
        return this->actual_current.at(channel);
    }
    void
    setAdjustedCurrent(const PowerSupplyStatus_constants::CHANNELVALUE &value)
    {
        this->adjusted_current.insert(value);
    }
    /**
     * @brief getAdjustedCurrent
     * @param channel
     * @return Adjusted Current as double
     * @throw [std::out_of_range](http://en.cppreference.com/w/cpp/error/out_of_range) if there is no value for the specified channel
     */
    const double &getAdjustedCurrent(const int &channel)
    {
        return this->adjusted_current.at(channel);
    }

    void setActualVoltage(const PowerSupplyStatus_constants::CHANNELVALUE &value)
    {
        this->actual_voltage.insert(value);
    }
    /**
     * @brief getActualVoltage
     * @param channel
     * @return Actual Voltage as double
     * @throw [std::out_of_range](http://en.cppreference.com/w/cpp/error/out_of_range) if there is no value for the specified channel
     */
    const double &getActualVoltage(const int &channel) {
        return this->actual_voltage.at(channel);
    }
    void
    setAdjustedVoltage(const PowerSupplyStatus_constants::CHANNELVALUE &value)
    {
        this->adjusted_current.insert(value);
    }
    /**
     * @brief getAdjustedVoltage Get value for channel
     * @param channel
     * @return Adjusted Voltage as double
     * @throw [std::out_of_range](http://en.cppreference.com/w/cpp/error/out_of_range) if there is no value for the specified channel
     */
    const double &getAdjustedVoltage(const int &channel)
    {
        return this->actual_current.at(channel);
    }

private:
    bool beeper;
    bool locked;
    bool output;
    bool ovp;
    bool ocp;

    /**
     * @brief actual_current Map holds actual current for all channels
     */
    std::map<int, double> actual_current;
    std::map<int, double> adjusted_current;
    /**
     * @brief actual_voltage Map holds actual voltage for all channels
     */
    std::map<int, double> actual_voltage;
    std::map<int, double> adjusted_voltage;
};

// Register our metatype. Needed to send this kind of object wrapped in a std
// smart pointer via SIGNAL/SLOT mechanism
Q_DECLARE_METATYPE(std::shared_ptr<PowerSupplyStatus>)

#endif // POWERSUPPLYSTATUS
