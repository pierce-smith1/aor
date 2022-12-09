#include "icons.h"

const std::map<ItemDomain, QPixmap> &Icons::activity_icons() {
    static std::map<ItemDomain, QPixmap> icons = {
        { Foraging, QPixmap(":/assets/img/icons/leaf.png") },
        { Mining, QPixmap(":/assets/img/icons/crystals.png") },
        { Trading, QPixmap(":/assets/img/icons/focussigil2.png") },
        { Smithing, QPixmap(":/assets/img/icons/hammer.png") },
        { Eating, QPixmap(":/assets/img/icons/apple.png") },
        { None, QPixmap(":/assets/img/icons/blank.png") },
    };

    return icons;
};

const std::map<ItemDomain, QPixmap> &Icons::active_status_icons() {
    static std::map<ItemDomain, QPixmap> icons = {
        { Foraging, QPixmap(":/assets/img/icons/sq_green_filled.png") },
        { Mining, QPixmap(":/assets/img/icons/sq_purple_filled.png") },
        { Trading, QPixmap(":/assets/img/icons/sq_blue_filled.png") },
        { Smithing, QPixmap(":/assets/img/icons/sq_pink_filled.png") },
        { Ordinary, QPixmap(":/assets/img/icons/sq_black_filled.png") },
    };

    return icons;
}

const std::map<ItemDomain, QPixmap> &Icons::inactive_status_icons() {
    static std::map<ItemDomain, QPixmap> icons = {
        { Foraging, QPixmap(":/assets/img/icons/sq_green_unfilled.png") },
        { Mining, QPixmap(":/assets/img/icons/sq_purple_unfilled.png") },
        { Trading, QPixmap(":/assets/img/icons/sq_blue_unfilled.png") },
        { Smithing, QPixmap(":/assets/img/icons/sq_pink_unfilled.png") },
        { Ordinary, QPixmap(":/assets/img/icons/sq_black_unfilled.png") },
    };

    return icons;
}

const std::map<ItemProperty, QPixmap> &Icons::resource_icons() {
    static std::map<ItemProperty, QPixmap> icons = {
        { StoneResource, QPixmap(":/assets/img/icons/stone.png") },
        { MetallicResource, QPixmap(":/assets/img/icons/metallic.png") },
        { CrystallineResource, QPixmap(":/assets/img/icons/crystalline.png") },
        { RunicResource, QPixmap(":/assets/img/icons/runic.png") },
        { LeafyResource, QPixmap(":/assets/img/icons/leafy.png") },
    };

    return icons;
}
