#include "items.h"
#include <chrono>

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

Item::Item(const ItemDefinition &def)
    : code(def.code),
      id(new_instance_id()),
      uses_left(def.default_uses_left),
      intent(ItemIntent::None) { }

Item::Item(ItemDefinitionPtr ptr)
    : Item(*ptr) { }

Item::Item(ItemCode id)
    : Item(def_of(id)) { }

Item::Item(const std::string &name)
    : Item(def_of(name)) { }

QPixmap Item::pixmap_of(ItemCode id) {
    std::string pixmap_name = ":/assets/img/items/" + Item::def_of(id)->internal_name + ".png";
    return QPixmap(QString::fromStdString(pixmap_name));
}

QPixmap Item::pixmap_of(const std::string &name) {
    std::string pixmap_name = ":/assets/img/items/" + name + ".png";
    return QPixmap(QString::fromStdString(pixmap_name));
}

QPixmap Item::pixmap_of(const ItemDefinition &def) {
    std::string pixmap_name = ":/assets/img/items/" + def.internal_name + ".png";
    return QPixmap(QString::fromStdString(pixmap_name));
}

ItemId Item::new_instance_id() {
    auto time = std::chrono::system_clock::now().time_since_epoch();
    auto milliseconds = std::chrono::duration_cast<std::chrono::milliseconds>(time).count();

    static std::random_device rd;
    static std::mt19937 rng(rd());
    static std::uniform_int_distribution<long> dist(0);

    return (milliseconds & 0xffffffff) + ((dist(rng) & 0xffffffff) << 32);
}
