#include "colors.h"

QString Colors::name(Color c) {
    switch (c) {
        case Banana: { return "Banana"; }
        case Orange: { return "Orange"; }
        case Cherry: { return "Cherry"; }
        case Blueberry: { return "Blueberry"; }
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
        case Blueberry: { return QColor(0, 153, 255); }
        case Plum: { return QColor(102, 102, 204); }
        case Cucumber: { return QColor(0, 153, 0); }
        case Lime: { return QColor(102, 204, 51); }
    }

    qFatal("unreachable");
    return QColor(0, 0, 0);
}

ItemProperties Colors::heritage_properties(Color c, int magnitude) {
    double factor = pow(magnitude, 1.5);
    switch (c) {
        case Banana: { return { { HeritageActivitySpeedBonus, 7 * factor } }; }
        case Orange: { return { { HeritageInjuryResilience, 7 * factor } }; }
        case Cherry: { return { { HeritageMaxEnergyBoost, 20 * factor } }; }
        case Blueberry: { return { { HeritageMaxMoraleBoost, 20 * factor } }; }
        case Plum: { return { { HeritageToolUsageBoost, 1 * factor } }; }
        case Cucumber: { return { { HeritageConsumableEnergyBoost, 5 * factor } }; }
        case Lime: { return { { HeritageMaterialValueBonus, 7 * factor } }; }
    }

    qFatal("unreachable");
    return {};
}

ItemProperties Colors::blend_heritage(std::multiset<Color> colors) {
    ItemProperties heritage_props;

    for (auto it = begin(colors); it != end(colors);) {
        const ItemProperties &other_props = heritage_properties(*it, colors.count(*it));
        heritage_props.map.insert(std::begin(other_props), std::end(other_props));
        std::advance(it, colors.count(*it));
    }

    return heritage_props;
}
