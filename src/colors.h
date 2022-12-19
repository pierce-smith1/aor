#pragma once

#include <map>
#include <vector>
#include <cmath>

#include <QColor>

#include "items.h"

const static int CHERRY_ENERGY_MULTIPLIER = 20;
const static int BLUERASPBERRY_spirit_MULTIPLIER = 20;
const static int BLUEBERRY_ENERGY_BONUS = 10;

enum Color : quint16 {
    Banana = 1,
    Orange,
    Cherry,
    Blueberry,
    Plum,
    Cucumber,
    Lime
};

namespace Colors {
    QString name(Color c);
    QColor qcolor(Color c);
    ItemProperties heritage_properties(Color c, int magnitude = 1);
    ItemProperties blend_heritage(std::multiset<Color> colors);

    template <typename ColorContainer> QColor blend(ColorContainer colors) {
        double r, g, b;

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
