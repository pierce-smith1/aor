#include "items.h"
#include "generators.h"
#include "gamewindow.h"
#include "die.h"

Item Item::empty_item = Item(0);

ItemDefinitionPtr Item::def_of(ItemCode code) {
    auto result = std::find_if(
        begin(ITEM_DEFINITIONS),
        end(ITEM_DEFINITIONS),
        [=](const ItemDefinition &def) {
            return def.code == code;
        }
    );

    if (result == ITEM_DEFINITIONS.end()) {
        bugcheck(DefLookupMiss, "code", code);
    }

    return result;
}

ItemDefinitionPtr Item::def_of(const QString &name) {
    auto match_name = [&name](const ItemDefinition def) -> bool { return def.internal_name == name; };
    auto result = std::find_if(begin(ITEM_DEFINITIONS), end(ITEM_DEFINITIONS), match_name);

    if (result == ITEM_DEFINITIONS.end()) {
        bugcheck(DefLookupMiss, "name", name);
    }

    return result;
}

ItemDefinitionPtr Item::def_of(const Item &item) {
    return def_of(item.code);
}

ItemCode Item::code_of(const QString &name) {
    auto result = std::find_if(begin(ITEM_DEFINITIONS), end(ITEM_DEFINITIONS), [=](const ItemDefinition &def) {
        return name == def.internal_name;
    });

    if (result == end(ITEM_DEFINITIONS)) {
        bugcheck(CodeLookupMiss, "name", name);
    }

    return result->code;
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

Item Item::make_egg() {
    Item egg = make_egg(NOBODY, NOBODY);
    egg.instance_properties.map[InstanceEggFoundFlavor] = Generators::color();
    return egg;
}

Item Item::make_egg(CharacterId parent1, CharacterId parent2) {
    Item egg = Item("fennahian_egg");

    egg.instance_properties.map[InstanceEggParent1] = parent1;
    egg.instance_properties.map[InstanceEggParent2] = parent2;
    egg.instance_properties.map[InstanceEggFoundActionstamp] = gw()->game().actions_done();

    return egg;
}

ItemDefinitionPtr Item::def() const {
    return def_of(code);
}

QString Item::instance_properties_to_string() const {
    QString string = "<br>";

    if (instance_properties[InstanceEggParent1] != NOBODY) {
        string += QString("<i>Lovingly made by <b>%1</b> and <b>%2</b>.</i><br>")
            .arg(gw()->game().character(instance_properties[InstanceEggParent1]).name())
            .arg(gw()->game().character(instance_properties[InstanceEggParent2]).name());
    }

    if (instance_properties[InstanceEggFoundActionstamp]) {
        string += QString("Hatches after <b>%1 more actions.</b></i><br>")
            .arg(ACTIONS_TO_HATCH - (gw()->game().actions_done() - instance_properties[InstanceEggFoundActionstamp]) + 1);
    }

    if (instance_properties[InstanceEggFoundFlavor]) {
        string += QString("<i>Smells like fresh %1...</i><br>")
            .arg(Colors::name((Color) instance_properties[InstanceEggFoundFlavor]));
    }

    return string;
}

void Item::call_hooks(HookType type, const HookPayload &payload) const {
    def_of(code)->properties.call_hooks(type, payload, def()->type);
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
    for (quint16 i = 1; i <= 5; i++) {
        ItemProperty cost_prop = (ItemProperty) (Cost + i);
        ItemProperty max_prop = (ItemProperty) (ToolMaximum + i);
        ItemProperty resource_prop = (ItemProperty) (Resource + i);
        fn(cost_prop, max_prop, resource_prop);
    }
}

void Item::for_each_tool_discover(const std::function<void(ItemProperty, ItemProperty)> &fn) {
    for (quint16 i = 0; i < 9; i++) {
        ItemProperty can_discover_prop = (ItemProperty) (ToolCanDiscover1 + i);
        ItemProperty weight_prop = (ItemProperty) (ToolDiscoverWeight1 + i);
        fn(can_discover_prop, weight_prop);
    }
}

QString Item::type_to_string(ItemType type) {
    QString string;

    if (type & Untradeable) { string += "Untradeable "; }

    if (type & Consumable) { string += "Consumable, "; }
    if (type & Material)  { string += "Material, "; }
    if (type & SmithingTool) { string += "Smithing Tool, "; }
    if (type & ForagingTool) { string += "Foraging Tool, "; }
    if (type & MiningTool) { string += "Mining Tool, "; }
    if (type & Blessing) { string += "Blessing, "; }
    if (type & Artifact) { string += "Artifact, "; }
    if (type & Rune) { string += "Curse, "; }
    if (type & Effect) { string += "Injury, "; }

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
            string += property_definitions().at(pair.first).description.arg(pair.second);
        }
        string += "<br>";
    }

    return string.left(string.size() - 4); // cut off the last <br>
}
