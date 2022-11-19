#include "items.h"
#include <algorithm>
#include <chrono>

ItemProperties::ItemProperties(std::initializer_list<std::pair<const ItemProperty, int>> map)
    : map(map) { }

int ItemProperties::operator[](ItemProperty prop) const {
    try {
        return map.at(prop);
    } catch (std::out_of_range &e) {
        return 0;
    }
}

ItemDefinitionPtr Item::def_of(ItemCode code) {
    auto match_code = [code](const ItemDefinition def) -> bool { return def.code == code; };
    auto result = std::find_if(begin(ITEM_DEFINITIONS), end(ITEM_DEFINITIONS), match_code);

    if (result == ITEM_DEFINITIONS.end()) {
        qFatal("Tried to get definition for invalid item code (%d)", code);
    }

    return result;
}

ItemDefinitionPtr Item::def_of(const std::string &name) {
    auto match_name = [&name](const ItemDefinition def) -> bool { return def.internal_name == name; };
    auto result = std::find_if(begin(ITEM_DEFINITIONS), end(ITEM_DEFINITIONS), match_name);

    if (result == ITEM_DEFINITIONS.end()) {
        qFatal("Tried to get definition for invalid item name (%s)", name.c_str());
    }

    return result;
}

ItemDefinitionPtr Item::def_of(const Item &item) {
    return def_of(item.code);
}

Item::Item(const ItemDefinition &def)
    : code(def.code),
      id(new_instance_id()),
      uses_left(def.default_uses_left),
      intent(ItemIntent::None)
{
    // Don't give an empty item a unique id
    if (code == 0) {
        id = 0;
    }
}

Item::Item(ItemDefinitionPtr ptr)
    : Item(*ptr) { }

Item::Item(ItemCode id)
    : Item(def_of(id)) { }

Item::Item(const std::string &name)
    : Item(def_of(name)) { }

QPixmap Item::pixmap_of(ItemCode id) {
    return pixmap_of(*def_of(id));
}

QPixmap Item::pixmap_of(const std::string &name) {
    return pixmap_of(*def_of(name));
}

QPixmap Item::pixmap_of(const ItemDefinition &def) {
    QString pixmap_name = QString::fromStdString(":/assets/img/items/" + def.internal_name + ".png");

    if (!QFile(pixmap_name).exists()) {
        qDebug("Missing item pixmap (%s)", def.internal_name.c_str());
        pixmap_name = QString::fromStdString(":/assets/img/items/missing.png");
    }

    return QPixmap(pixmap_name);
}

QPixmap Item::pixmap_of(const Item &item) {
    return pixmap_of(*def_of(item));
}

ItemId Item::new_instance_id() {
    auto time = std::chrono::system_clock::now().time_since_epoch();
    auto milliseconds = std::chrono::duration_cast<std::chrono::milliseconds>(time).count();

    static std::random_device rd;
    static std::mt19937 rng(rd());
    static std::uniform_int_distribution<long> dist(0);

    return (milliseconds & 0xffffffff) + ((dist(rng) & 0xffffffff) << 32);
}

Item Item::invalid_item() {
    Item item;
    item.code = INVALID_CODE;
    item.id = INVALID_ID;

    return item;
}

std::string Item::type_to_string(ItemType type) {
    switch (type) {
        case NoType: return "Empty slot";
        case Consumable: return "Consumable";
        case Material: return "Material";
        case ForagingTool: return "Foraging Tool";
        case SmithingTool: return "Smithing Tool";
        case TradingTool: return "Trading Tool";
        case PrayerTool: return "Ceremonial Tool";
        case Artifact: return "Artifact";
        case Blessing: return "Blessing";
        default: return "???";
    }
}
