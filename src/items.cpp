#include "items.h"
#include <algorithm>
#include <chrono>
#include <string>

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
      intent(NoIntent)
{
    // Don't give an empty item a unique id
    if (code == 0) {
        id = 0;
    }
}

Item::Item(ItemDefinitionPtr ptr)
    : Item(*ptr) { }

Item::Item(ItemCode code)
    : Item(def_of(code)) { }

Item::Item(const std::string &name)
    : Item(def_of(name)) { }

TooltipText Item::get_tooltip_text() {
    TooltipText text;

    ItemDefinitionPtr this_def = def_of(code);

    text.title = "<b>" + this_def->display_name + "</b>";
    text.description = "<i>" + this_def->description + "</i>";
    text.subtext = Item::type_to_string(this_def->type);

    switch (intent) {
        default:
        case NoIntent: {
            break;
        }
        case ToBeMaterial: {
            text.subtext += " <b><font color=green>(Queued for smithing)</font></b>";
            break;
        }
        case ToBeOffered: {
            text.subtext += " <b><font color=green>(Queued for offering)</font></b>";
            break;
        }
        case UsingAsTool: {
            text.subtext += " <b><font color=green>(Current tool)</font></b>";
            break;
        }
        case UsingAsArtifact: {
            text.subtext += " <b><font color=green>(Active artifact)</font></b>";
            break;
        }
    }

    for (const std::pair<const ItemProperty, int> &property_pair : this_def->properties.map) {
        switch (property_pair.first) {
            case EnergyBoost: {
                text.description += "<br>On consumed: +" + std::to_string(property_pair.second) + " energy";
                break;
            }
            case EnergyCost: {
                text.description += "<br>Costs " + std::to_string(property_pair.second) + " energy per use";
            }
            default: break;
        }
    }

    if (uses_left != 0) {
        text.subtext += " <font color=gray>(" + std::to_string(uses_left) + " uses left)</font>";
    }

    return text;
}

ItemDefinitionPtr Item::def() {
    return def_of(code);
}

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
    std::string string;

    if (type & IT_CONSUMABLE) string += "Consumable, ";
    if (type & IT_MATERIAL) string += "Material, ";
    if (type & IT_FORAGING_TOOL) string += "Foraging Tool, ";
    if (type & IT_MINING_TOOL) string += "Mining Tool, ";
    if (type & IT_PRAYER_TOOL) string += "Ceremonial Tool, ";
    if (type & IT_BLESSING) string += "Blessing, ";
    if (type & IT_ARTIFACT) string += "Artifact, ";
    if (type & IT_RUNE) string += "Curse, ";

    // Chop off the last comma and space
    return string.substr(0, string.length() - 2);
}
