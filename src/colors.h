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

namespace Colors {
    QString name(Color c);
    QColor qcolor(Color c);
};
