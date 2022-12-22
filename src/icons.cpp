#include "icons.h"

const std::map<ItemDomain, QPixmap> &Icons::activity_icons() {
    static std::map<ItemDomain, QPixmap> icons = {
        { Foraging, QPixmap(":/assets/img/icons/leaf.png") },
        { Mining, QPixmap(":/assets/img/icons/crystals.png") },
        { Trading, QPixmap(":/assets/img/icons/focussigil2.png") },
        { Smithing, QPixmap(":/assets/img/icons/hammer.png") },
        { Eating, QPixmap(":/assets/img/icons/apple.png") },
        { Defiling, QPixmap(":/assets/img/icons/spirit.png") },
        { Coupling, QPixmap(":/assets/img/icons/heart.png") },
        { None, QPixmap(":/assets/img/icons/blank.png") },
    };

    return icons;
};

const std::map<ItemDomain, QPixmap> &Icons::activity_icons_big() {
    static std::map<ItemDomain, QPixmap> icons = {
        { Foraging, QPixmap(":/assets/img/icons/leaf_big.png") },
        { Mining, QPixmap(":/assets/img/icons/rock_big.png") },
        { Trading, QPixmap(":/assets/img/icons/focussigil_big.png") },
        { Smithing, QPixmap(":/assets/img/icons/hammer_big.png") },
        { Eating, QPixmap(":/assets/img/icons/apple_big.png") },
        { Defiling, QPixmap(":/assets/img/icons/spirit_big.png") },
        { Coupling, QPixmap(":/assets/img/icons/heart_big.png") },
        { None, QPixmap(":/assets/img/icons/blank.png") },
    };

    return icons;
};

const std::map<ItemDomain, QPixmap> &Icons::activity_portraits() {
    static std::map<ItemDomain, QPixmap> icons = {
        { Foraging, QPixmap(":/assets/img/lk/foraging.png") },
        { Mining, QPixmap(":/assets/img/lk/mining.png") },
        { Trading, QPixmap(":/assets/img/lk/trading.png") },
        { Smithing, QPixmap(":/assets/img/lk/smithing.png") },
        { Eating, QPixmap(":/assets/img/lk/eating.png") },
        { Defiling, QPixmap(":/assets/img/lk/defiling.png") },
        { Coupling, QPixmap(":/assets/img/lk/coupling.png") },
        { None, QPixmap(":/assets/img/lk/idle.png") },
    };

    return icons;
};

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
