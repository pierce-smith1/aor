#pragma once

#include <map>
#include <vector>
#include <cmath>

#include <QColor>

#include "items.h"

const static int CHERRY_ENERGY_MULTIPLIER = 20;
const static int BLUERASPBERRY_MORALE_MULTIPLIER = 20;
const static int BLUEBERRY_ENERGY_BONUS = 10;

enum Color : quint16 {
    Banana,
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

    template <typename ColorContainer> ItemProperties blend_heritage(ColorContainer colors) {
        ItemProperties heritage_props;

        for (Color c : colors) {
            const ItemProperties &other_props = heritage_properties(c);
            heritage_props.map.insert(std::begin(other_props), std::end(other_props));
        }

        return heritage_props;
    }
};
