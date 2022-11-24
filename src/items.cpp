#include "items.h"

Item Item::empty_item {Item(0)};

ItemProperties::ItemProperties(std::initializer_list<std::pair<const ItemProperty, std::uint16_t>> map)
    : map(map) { }

std::uint16_t ItemProperties::operator[](ItemProperty prop) const {
    try {
        return map.at(prop);
    } catch (std::out_of_range &e) {
        return 0;
    }
}

ItemDefinitionPtr Item::def_of(ItemCode code) {
    auto match_code {[code](const ItemDefinition def) -> bool {
        return def.code == code;
    }};
    auto result {std::find_if(begin(ITEM_DEFINITIONS), end(ITEM_DEFINITIONS), match_code)};

    if (result == ITEM_DEFINITIONS.end()) {
        qFatal("Tried to get definition for invalid item code (%d)", code);
    }

    return result;
}

ItemDefinitionPtr Item::def_of(const QString &name) {
    auto match_name {[&name](const ItemDefinition def) -> bool { return def.internal_name == name; }};
    auto result {std::find_if(begin(ITEM_DEFINITIONS), end(ITEM_DEFINITIONS), match_name)};

    if (result == ITEM_DEFINITIONS.end()) {
        qFatal("Tried to get definition for invalid item name (%s)", name.toStdString().c_str());
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
      intent(Ordinary)
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

Item::Item(const QString &name)
    : Item(def_of(name)) { }

TooltipText Item::get_tooltip_text() const {
    ItemDefinitionPtr this_def {def()};

    TooltipText text;
    text.title = QString("<b>%1</b>").arg(this_def->display_name);
    text.description = QString("<i>%1</i>").arg(this_def->description);

    switch (this_def->item_level) {
        case 1: { text.subtext = "Unremarkable "; break; }
        case 2: { text.subtext = "Common "; break; }
        case 3: { text.subtext = "Notable "; break; }
        case 4: { text.subtext = "Rare "; break; }
        case 5: { text.subtext = "Enchanted "; break; }
        case 6: { text.subtext = "Truly Extraordinary "; break; }
        case 7: { text.subtext = "Anomalous "; break; }
        case 8: { text.subtext = "Incomprehensible "; break; }
        default: { break; }
    }

    text.subtext += Item::type_to_string(this_def->type);

    switch (intent) {
        default:
        case None: {
            break;
        }
        case Consumable: {
            text.subtext += " <b><font color=green>(Being eaten)</font></b>";
            break;
        }
        case Material: {
            text.subtext += " <b><font color=green>(Queued for smithing)</font></b>";
            break;
        }
        case Offering: {
            text.subtext += " <b><font color=green>(Queued for offering)</font></b>";
            break;
        }
        case KeyOffering: {
            text.subtext += " <b><font color=#ff7933>(Queued as key offering)</font></b>";
            break;
        }
        case SmithingTool:
        case ForagingTool:
        case MiningTool:
        case PrayerTool:
        case Artifact: {
            text.subtext += " <b><font color=green>(Equipped)</font></b>";
            break;
        }
    }

    for (const std::pair<const ItemProperty, std::uint16_t> &property_pair : this_def->properties.map) {
        switch (property_pair.first) {
            case ConsumableEnergyBoost: {
                text.description += QString("<br><b>On consumed:</b> <font color=#ff3300>+%1 energy</font>").arg(property_pair.second);
                break;
            }
            case ConsumableMoraleBoost: {
                text.description += QString("<br><b>On consumed:</b> <font color=#0099d7>+%1 spirit</font>").arg(property_pair.second);
                break;
            }
            case ConsumableGivesEffect: {
                text.description += QString("<br><b>On consumed:</b> <font color=#6666cc>gives an effect</font>");
                break;
            }
            case ConsumableGivesBuff: {
                text.description += QString("<br><b>On consumed:</b> <font color=#009900>gives a permanent buff</font>");
                break;
            }
            case ToolEnergyCost: {
                text.description += QString("<br>Costs <font color=orangered>%1 energy</font> per use").arg(property_pair.second);
            }
            default: break;
        }
    }

    if (uses_left != 0) {
        text.subtext += QString(" <font color=gray>(%1 uses left)</font>").arg(uses_left);
    }

    return text;
}

ItemDefinitionPtr Item::def() const {
    return def_of(code);
}

QPixmap Item::pixmap_of(ItemCode id) {
    return pixmap_of(*def_of(id));
}

QPixmap Item::pixmap_of(const QString &name) {
    return pixmap_of(*def_of(name));
}

QPixmap Item::pixmap_of(const ItemDefinition &def) {
    QString pixmap_name {QString(":/assets/img/items/%1.png").arg(def.internal_name)};

    if (!QFile(pixmap_name).exists()) {
        qDebug("Missing item pixmap (%s)", def.internal_name.toStdString().c_str());
        pixmap_name = ":/assets/img/items/missing.png";
    }

    return QPixmap(pixmap_name);
}

QPixmap Item::pixmap_of(const Item &item) {
    return pixmap_of(*def_of(item));
}

ItemId Item::new_instance_id() {
    auto time {std::chrono::system_clock::now().time_since_epoch()};
    auto milliseconds {std::chrono::duration_cast<std::chrono::milliseconds>(time).count()};

    return (milliseconds & 0xffffffff) + ((std::uint64_t) Generators::rng()->generate() << 32);
}

Item Item::invalid_item() {
    Item item;
    item.code = INVALID_CODE;
    item.id = INVALID_ID;

    return item;
}

QString Item::type_to_string(ItemType type) {
    QString string;

    if (type & Consumable) string += "Consumable, ";
    if (type & Material) string += "Material, ";
    if (type & ForagingTool) string += "Foraging Tool, ";
    if (type & MiningTool) string += "Mining Tool, ";
    if (type & PrayerTool) string += "Ceremonial Tool, ";
    if (type & Blessing) string += "Blessing, ";
    if (type & Artifact) string += "Artifact, ";
    if (type & Rune) string += "Curse, ";

    // Chop off the last comma and space
    return string.left(string.length() - 2);
}
