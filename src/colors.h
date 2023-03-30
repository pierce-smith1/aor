#pragma once

#include <map>
#include <vector>
#include <cmath>
#include <set>

#include <QColor>

#include "itemproperties.h"

enum Color : AorUInt {
    Banana = 1,
    Orange,
    Cherry,
    Blueberry,
    Plum,
    Cucumber,
    Lime,
    Sugarplum,
};

namespace Colors {
    QString name(Color c);
    QColor qcolor(Color c);
    ItemProperties heritage_properties(Color c, int magnitude = 1);
    ItemProperties blend_heritage(std::multiset<Color> colors);

    template <typename ColorContainer> QColor blend(ColorContainer colors) {
        double r = 0.0, g = 0.0, b = 0.0;

        for (Color c : colors) {
            r += qcolor(c).redF();
            g += qcolor(c).greenF();
            b += qcolor(c).blueF();
        }

        r /= colors.size();
        g /= colors.size();
        b /= colors.size();

        return QColor(r * 255, g * 255, b * 255);
    }
};
