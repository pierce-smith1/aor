#pragma once

#include <map>

#include <QColor>

enum Color : quint16 {
    Banana,
    Orange,
    Cherry,
    Blueberry,
    BlueRaspberry,
    Plum,
    Cucumber,
    Lime
};

static const std::map<Color, QColor> COLORS = {
    { Banana, QColor(255, 204, 0) },
    { Orange, QColor(255, 153, 51) },
    { Cherry, QColor(255, 51, 0) },
    { Blueberry, QColor(0, 102, 204) },
    { BlueRaspberry, QColor(0, 153, 255) },
    { Plum, QColor(102, 102, 204) },
    { Cucumber, QColor(0, 153, 0) },
    { Lime, QColor(102, 204, 51) },
};
