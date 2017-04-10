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

#include "yaxishelper.h"

namespace globcon = global_constants;

YAxisHelper::YAxisHelper() {}
YAxisBounds YAxisHelper::getyAxisBounds(const QCPRange &currentXRange,
                                        QCustomPlot *plot, int noChannels)
{
    std::vector<double> voltageBounds;
    std::vector<double> currentBounds;
    std::vector<double> wattageBounds;

    YAxisBounds axb;

    // loop through all channels
    for (int i = 1; i <= noChannels; i++) {
        for (int j = 0; j < 5; j++) {
            globcon::LPQ_DATATYPE dt = static_cast<globcon::LPQ_DATATYPE>(j);
            // only visible graphs count
            if (!plot->graph(j)->visible())
                continue;
            QCPDataMap *dataMap = plot->graph(j)->data();
            std::vector<double> graphValues;
            std::pair<double, double> lowHighPair = {0, 0};
            if (dataMap->size() > 1) {
                // get the nearest key to lower
                QCPDataMap::Iterator itbegin =
                    dataMap->upperBound(currentXRange.lower);
                if (itbegin == dataMap->end())
                    itbegin = dataMap->begin();
                // get the nearest key to upper
                QCPDataMap::Iterator itend =
                    dataMap->lowerBound(currentXRange.upper);
                if (itend == dataMap->end())
                    itend = dataMap->end() - 1;
                // extract all the data for the visible time frame and put it in
                // vector
                for (; itbegin != itend; itbegin++) {
                    graphValues.push_back((*itbegin).value);
                }
                lowHighPair = this->lowHighVectorValue(std::move(graphValues));
            } else if (!dataMap->empty()) {
                lowHighPair.first = dataMap->first().value;
                lowHighPair.second = dataMap->first().value;
            }

            if (dt == globcon::LPQ_DATATYPE::SETVOLTAGE ||
                dt == globcon::LPQ_DATATYPE::VOLTAGE) {
                voltageBounds.push_back(lowHighPair.first);
                voltageBounds.push_back(lowHighPair.second);
            }
            if (dt == globcon::LPQ_DATATYPE::SETCURRENT ||
                dt == globcon::LPQ_DATATYPE::CURRENT) {
                currentBounds.push_back(lowHighPair.first);
                currentBounds.push_back(lowHighPair.second);
            }
            if (dt == globcon::LPQ_DATATYPE::WATTAGE ||
                dt == globcon::LPQ_DATATYPE::WATTAGE) {
                wattageBounds.push_back(lowHighPair.first);
                wattageBounds.push_back(lowHighPair.second);
            }
        }
    }

    std::pair<double, double> voltLowHigh =
        lowHighVectorValue(std::move(voltageBounds));
    axb.voltageLower = std::move(voltLowHigh.first);
    axb.voltageUpper = std::move(voltLowHigh.second);
    std::pair<double, double> currentLowHigh =
        lowHighVectorValue(std::move(currentBounds));
    axb.currentLower = std::move(currentLowHigh.first);
    axb.currentUpper = std::move(currentLowHigh.second);
    std::pair<double, double> wattageLowHigh =
        lowHighVectorValue(std::move(wattageBounds));
    axb.wattageLower = std::move(wattageLowHigh.first);
    axb.wattageLower = std::move(wattageLowHigh.second);

    return axb;
}

std::pair<double, double> YAxisHelper::lowHighVectorValue(
    std::vector<double> values)
{
    // TODO: Wouldn't it be easier to use this method to find min max of a vector
    // http://en.cppreference.com/w/cpp/algorithm/minmax_element
    std::pair<double, double> lowHighPair = {0, 0};
    if (!values.empty()) {
        // TODO: Couldn't we pass the original lowHighPair as reference to this
        // function?
        lowHighPair.first = values.at(0);
        lowHighPair.second = values.at(0);
        for (auto d : values) {
            lowHighPair.first = std::min(lowHighPair.first, d);
            lowHighPair.second = std::max(lowHighPair.second, d);
        }
    }

    return lowHighPair;
}
