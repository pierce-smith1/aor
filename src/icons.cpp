#include "icons.h"

const std::map<ItemDomain, QIcon> Icons::activity_icons() {
    static std::map<ItemDomain, QIcon> icons = {
        { Foraging, QIcon(":/assets/img/icons/leaf.png") },
        { Mining, QIcon(":/assets/img/icons/crystals.png") },
        { Praying, QIcon(":/assets/img/icons/focussigil2.png") },
        { Smithing, QIcon(":/assets/img/icons/hammer.png") },
        { Eating, QIcon(":/assets/img/icons/apple.png") },
        { None, QIcon(":/assets/img/icons/blank.png") },
    };

    return icons;
};
