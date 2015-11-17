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
            globcon::DATATYPE dt = static_cast<globcon::DATATYPE>(j);
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
                for (; itbegin != itend; itbegin++) {
                    graphValues.push_back((*itbegin).value);
                }
                lowHighPair = this->lowHighVectorValue(std::move(graphValues));
            } else if (!dataMap->empty()) {
                lowHighPair.first = dataMap->first().value;
                lowHighPair.second = dataMap->first().value;
            }

            if (dt == globcon::DATATYPE::VOLTAGE ||
                dt == globcon::DATATYPE::ACTUALVOLTAGE) {
                voltageBounds.push_back(lowHighPair.first);
                voltageBounds.push_back(lowHighPair.second);
            }
            if (dt == globcon::DATATYPE::CURRENT ||
                dt == globcon::DATATYPE::ACTUALCURRENT) {
                currentBounds.push_back(lowHighPair.first);
                currentBounds.push_back(lowHighPair.second);
            }
            if (dt == globcon::DATATYPE::WATTAGE ||
                dt == globcon::DATATYPE::WATTAGE) {
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

std::pair<double, double>
YAxisHelper::lowHighVectorValue(std::vector<double> values)
{
    std::pair<double, double> lowHighPair = {0, 0};
    if (!values.empty()) {
        lowHighPair.first = values.at(0);
        lowHighPair.second = values.at(0);
        for (auto d : values) {
            lowHighPair.first = std::min(lowHighPair.first, d);
            lowHighPair.second = std::max(lowHighPair.second, d);
        }
    }

    return lowHighPair;
}
