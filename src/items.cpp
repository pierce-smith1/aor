#include "items.h"
#include "generators.h"
#include "gamewindow.h"
#include "die.h"

Item Item::empty_item = Item();

ItemDefinitionPtr Item::def_of(ItemCode code) {
    auto result = std::find_if(ITEM_DEFINITIONS.begin(), ITEM_DEFINITIONS.end(), [=](const ItemDefinition &def) {
        return def.code == code;
    });

    if (result == ITEM_DEFINITIONS.end()) {
        bugcheck(DefLookupMiss, "code", code);
    }

    return result;
}

ItemDefinitionPtr Item::def_of(const QString &name) {
    auto result = std::find_if(ITEM_DEFINITIONS.begin(), ITEM_DEFINITIONS.end(), [&](const ItemDefinition &def) {
        return def.internal_name == name;
    });

    if (result == ITEM_DEFINITIONS.end()) {
        bugcheck(DefLookupMiss, "name", name);
    }

    return result;
}

ItemDefinitionPtr Item::def_of(const Item &item) {
    return def_of(item.code);
}

ItemCode Item::code_of(const QString &name) {
    auto result = std::find_if(ITEM_DEFINITIONS.begin(), ITEM_DEFINITIONS.end(), [&](const ItemDefinition &def) {
        return name == def.internal_name;
    });

    if (result == ITEM_DEFINITIONS.end()) {
        bugcheck(CodeLookupMiss, "name", name);
    }

    return result->code;
}

Item::Item(const ItemDefinition &def)
    : code(def.code),
      id(Generators::item_id()),
      uses_left(def.default_uses_left)
{
    // Don't give an empty item a unique id
    if (code == EMPTY_CODE) {
        id = EMPTY_ID;
    }
}

Item::Item(ItemDefinitionPtr ptr)
    : Item(*ptr) { }

Item::Item(ItemCode code)
    : Item(def_of(code)) { }

Item::Item(const QString &name)
    : Item(def_of(name)) { }

Item Item::make_egg() {
    Item egg = make_egg(NOBODY, NOBODY);
    egg.instance_properties.map[InstanceEggFoundFlavor] = Generators::color();
    return egg;
}

Item Item::make_egg(CharacterId parent1, CharacterId parent2) {
    Item egg = Item("fennahian_egg");

    egg.instance_properties.map[InstanceEggParent1] = parent1;
    egg.instance_properties.map[InstanceEggParent2] = parent2;
    egg.instance_properties.map[InstanceEggFoundThreatstamp] = gw()->game()->threat();

    return egg;
}

ItemDefinitionPtr Item::def() const {
    return def_of(code);
}

QString Item::instance_properties_to_string() const {
    QString string = "<br>";

    if (instance_properties[InstanceEggParent1] != NOBODY) {
        string += QString("<i>Lovingly made by <b>%1</b> and <b>%2</b>.</i><br>")
            .arg(gw()->game()->character(instance_properties[InstanceEggParent1]).name())
            .arg(gw()->game()->character(instance_properties[InstanceEggParent2]).name());
    }

    if (instance_properties[InstanceEggFoundThreatstamp]) {
        string += QString("Hatches after <b>%1 more threat</b> is accumulated.</i><br>")
            .arg(THREAT_TO_HATCH - (gw()->game()->threat() - instance_properties[InstanceEggFoundThreatstamp]) + 1);
    }

    if (instance_properties[InstanceEggFoundFlavor]) {
        string += QString("<i>Smells like fresh %1...</i><br>")
            .arg(Colors::name((Color) instance_properties[InstanceEggFoundFlavor]));
    }

    return string;
}

QString Item::to_data_string() const {
    QString instance_props_string;

    for (const auto &pair : instance_properties) {
        instance_props_string += QString("%1:%2:").arg(pair.first).arg(pair.second);
    }

    return QString("%1;%2;%3;%4;%5 Hey! Keep the items in the game, man. Thanks for playing AOR! :)")
        .arg(code)
        .arg(id)
        .arg(uses_left)
        .arg(owning_action)
        .arg(instance_props_string);
}

Item Item::from_data_string(const QString &data_string) {
    QStringList chunks = data_string.split(";");

    Item item;
    item.code = chunks[0].toULongLong();
    item.id = chunks[1].toULongLong();
    item.uses_left = chunks[2].toULongLong();
    item.owning_action = chunks[3].toULongLong();

    bool is_key = true;
    QString key;
    std::map<ItemProperty, AorUInt> instance_props;
    for (const QString &key_or_prop : chunks[4].split(':', Qt::SkipEmptyParts)) {
        if (is_key) {
            key = key_or_prop;
        } else {
            instance_props[static_cast<ItemProperty>(key.toULongLong())] = key_or_prop.toULongLong();
        }
        is_key = !is_key;
    }
    item.instance_properties = ItemProperties(instance_props);

    return item;
}

void Item::call_hooks(HookType type, const HookPayload &payload, ItemProperty allowed_prop_type) const {
    def_of(code)->properties.call_hooks(type, payload, def()->type, allowed_prop_type);
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
        qWarning("Missing item pixmap (%s)", def.internal_name.toStdString().c_str());
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
        qWarning("Missing item sil pixmap (%s)", def->internal_name.toStdString().c_str());
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

bool Item::has_resource_value(ItemCode code) {
    const ItemProperties &properties = def_of(code)->properties;
    bool has_value = false;

    for_each_resource_type([&](ItemProperty, ItemProperty, ItemProperty resource_prop) {
        if (properties[resource_prop]) {
            has_value = true;
        }
    });

    return has_value;
}

void Item::for_each_resource_type(const std::function<void(ItemProperty, ItemProperty, ItemProperty)> &fn) {
    for (AorUInt i = 1; i <= 5; i++) {
        ItemProperty cost_prop = (ItemProperty) (Cost + i);
        ItemProperty max_prop = (ItemProperty) (ToolMaximum + i);
        ItemProperty resource_prop = (ItemProperty) (Resource + i);
        fn(cost_prop, max_prop, resource_prop);
    }
}

void Item::for_each_tool_discover(const std::function<void(ItemProperty, ItemProperty)> &fn) {
    for (AorUInt i = 0; i < 9; i++) {
        ItemProperty can_discover_prop = (ItemProperty) (ToolCanDiscover1 + i);
        ItemProperty weight_prop = (ItemProperty) (ToolDiscoverWeight1 + i);
        fn(can_discover_prop, weight_prop);
    }
}

QString Item::type_to_string(ItemType type) {
    QString string;

    if (type & Untradeable) { string += "Untradeable "; }
    if (type & Signature) { string += "Signature "; }

    if (type & Consumable) { string += "Consumable, "; }
    if (type & Material)  { string += "Material, "; }
    if (type & SmithingTool) { string += "Smithing Tool, "; }
    if (type & ForagingTool) { string += "Foraging Tool, "; }
    if (type & MiningTool) { string += "Mining Tool, "; }
    if (type & Skill) { string += "Skill, "; }
    if (type & Artifact) { string += "Artifact, "; }
    if (type & Effect) { string += "Injury, "; }
    if (type & Weather) { string += "Environment Effect, "; }
    if (type & Curse) { string += "Curse, "; }

    // Chop off the last comma and space
    return string.left(string.length() - 2);
}

QString Item::properties_to_string(const ItemProperties &props) {
    QString string;

    for (const auto &pair : props.map) {
        if (pair.first == ItemLevel) {
            continue;
        }

        if (property_definitions().find(pair.first) != end(property_definitions())) {
            QString description;

            const PropertyDefinition &def = property_definitions().at(pair.first);

            if (pair.first & HoldsItemCode) {
                description = def.description.arg(Item::def_of(pair.second)->display_name);
            } else if (pair.first == PropertyIfLore) {
                ItemProperties target_properties = {{ (ItemProperty) pair.second, props[PropertyIfLoreValue] }};
                description = def.description.arg(props[PropertyLoreRequirement]).arg(properties_to_string(target_properties));
            } else {
                description = def.description.arg(pair.second);
            }

            if (!description.isEmpty()) {
                string += description;
                string += "<br>";
            }
        }
    }

    return string.left(string.size() - 4); // cut off the last <br>
}

void Item::serialize(QIODevice *dev) const {
    Serialize::serialize(dev, code);
    Serialize::serialize(dev, id);
    Serialize::serialize(dev, uses_left);
    Serialize::serialize(dev, owning_action);
    Serialize::serialize(dev, instance_properties);
}

void Item::deserialize(QIODevice *dev) {
    Serialize::deserialize(dev, code);
    Serialize::deserialize(dev, id);
    Serialize::deserialize(dev, uses_left);
    Serialize::deserialize(dev, owning_action);
    Serialize::deserialize(dev, instance_properties);
}
