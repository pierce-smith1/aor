#include "colors.h"

QString Colors::name(Color c) {
    switch (c) {
        case Banana: { return "Banana"; }
        case Orange: { return "Orange"; }
        case Cherry: { return "Cherry"; }
        case Blueberry: { return "Blueberry"; }
        case BlueRaspberry: { return "Blue Raspberry"; }
        case Plum: { return "Plum"; }
        case Cucumber: { return "Cucumber"; }
        case Lime: { return "Lime"; }
    }

    qFatal("unreachable");
    return "";
}

QColor Colors::qcolor(Color c) {
    switch (c) {
        case Banana: { return QColor(255, 204, 0); }
        case Orange: { return QColor(255, 153, 51); }
        case Cherry: { return QColor(255, 51, 0); }
        case Blueberry: { return QColor(0, 102, 204); }
        case BlueRaspberry: { return QColor(0, 153, 255); }
        case Plum: { return QColor(102, 102, 204); }
        case Cucumber: { return QColor(0, 153, 0); }
        case Lime: { return QColor(102, 204, 51); }
    }

    qFatal("unreachable");
    return QColor(0, 0, 0);
}
