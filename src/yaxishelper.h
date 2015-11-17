#ifndef YAXISHELPER_H
#define YAXISHELPER_H

#include <QMap>

#include <vector>
#include <algorithm>

#include "qcustomplot.h"

#include "global.h"

struct YAxisBounds {
    double voltageUpper;
    double voltageLower;
    double currentUpper;
    double currentLower;
    double wattageUpper;
    double wattageLower;

    YAxisBounds()
    {
        voltageUpper = 0;
        voltageLower = 0;
        currentUpper = 0;
        currentLower = 0;
        wattageUpper = 0;
        wattageLower = 0;
    }
};

class YAxisHelper
{
public:
    YAxisHelper();

    YAxisBounds getyAxisBounds(const QCPRange &currentXRange, QCustomPlot *plot,
                               int noChannels);

private:
    std::pair<double, double> lowHighVectorValue(std::vector<double> values);
};

#endif // YAXISHELPER_H
