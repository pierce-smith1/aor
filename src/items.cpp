#include "items.h"
#include "generators.h"

Item Item::empty_item {Item(0)};

ItemProperties::ItemProperties(std::initializer_list<std::pair<const ItemProperty, quint16>> map)
    : map(map) { }

quint16 ItemProperties::operator[](ItemProperty prop) const {
    try {
        return map.at(prop);
    } catch (std::out_of_range &e) {
        return 0;
    }
}

std::map<ItemProperty, quint16>::const_iterator ItemProperties::begin() const {
    return map.begin();
}

std::map<ItemProperty, quint16>::const_iterator ItemProperties::end() const {
    return map.end();
}

ItemDefinitionPtr Item::def_of(ItemCode code) {
    auto match_code {[code](const ItemDefinition def) -> bool {
        return def.code == code;
    }};
    auto result = std::find_if(begin(ITEM_DEFINITIONS), end(ITEM_DEFINITIONS), match_code);

    if (result == ITEM_DEFINITIONS.end()) {
        qFatal("Tried to get definition for invalid item code (%d)", code);
    }

    return result;
}

ItemDefinitionPtr Item::def_of(const QString &name) {
    auto match_name = [&name](const ItemDefinition def) -> bool { return def.internal_name == name; };
    auto result = std::find_if(begin(ITEM_DEFINITIONS), end(ITEM_DEFINITIONS), match_name);

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
      id(Generators::item_id()),
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
    QString pixmap_name = QString(":/assets/img/items/%1.png").arg(def.internal_name);

    if (!QFile(pixmap_name).exists()) {
        qDebug("Missing item pixmap (%s)", def.internal_name.toStdString().c_str());
        pixmap_name = ":/assets/img/items/missing.png";
    }

    return QPixmap(pixmap_name);
}

QPixmap Item::pixmap_of(const Item &item) {
    return pixmap_of(*def_of(item));
}

QPixmap Item::sil_pixmap_of(ItemCode code) {
    ItemDefinitionPtr def = def_of(code);
    QString pixmap_name = QString(":/assets/img/items/sil/%1.png").arg(def->internal_name);

    if (!QFile(pixmap_name).exists()) {
        qDebug("Missing item sil pixmap (%s)", def->internal_name.toStdString().c_str());
        pixmap_name = ":/assets/img/items/missing_sil.png";
    }

    return QPixmap(pixmap_name);
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
    if (type & SmithingTool) string += "Smithing Tool, ";
    if (type & ForagingTool) string += "Foraging Tool, ";
    if (type & MiningTool) string += "Mining Tool, ";
    if (type & Blessing) string += "Blessing, ";
    if (type & Artifact) string += "Artifact, ";
    if (type & Rune) string += "Curse, ";

    // Chop off the last comma and space
    return string.left(string.length() - 2);
}

QString Item::properties_to_string(const ItemProperties &props) {
    QString string;

    const static std::map<ItemProperty, QString> property_descriptions = {
        { ToolEnergyCost, "Costs <b>%1 energy</b> per use." },
        { ConsumableEnergyBoost, "Gives <b>+%1 energy</b>." },
        { ConsumableMoraleBoost, "Gives <b>+%1 spirit</b>." },
        { ConsumableClearsNumEffects, "Clears up to <b>%1 effect(s)</b>, moving from right to left." },
        { ArtifactMaxEnergyBoost, "You have <b>+%1 max energy</b>." },
        { ArtifactMaxMoraleBoost, "You have <b>+%1 max morale</b>." },
        { ArtifactSpeedBonus, "Your actions complete <b>%1x faster</b>." },
    };

    for (const auto &pair : props.map) {
        if (property_descriptions.find(pair.first) != end(property_descriptions)) {
            string += property_descriptions.at(pair.first).arg(pair.second);
        }
        string += "<br>";
    }

    return string.left(string.size() - 4); // cut off the last <br>
}
