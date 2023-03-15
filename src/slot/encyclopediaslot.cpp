#include "encyclopediaslot.h"
#include "../encyclopedia.h"

EncyclopediaSlot::EncyclopediaSlot(size_t y, size_t x, ItemCode item_group)
    : m_y(y), m_x(x), m_item_group(item_group) {}

bool EncyclopediaSlot::do_hovering() {
    return valid();
}

std::optional<Item> EncyclopediaSlot::tooltip_item() {
    if (undiscovered()) {
        return std::optional<Item>();
    } else {
        return std::optional<Item>(Item(code()));
    }
}

std::optional<TooltipInfo> EncyclopediaSlot::tooltip_info() {
    if (undiscovered()) {
        return std::optional<TooltipInfo>({
            "???",
            "Undiscovered",
            "You haven't discovered this item yet.",
            QPixmap(":assets/img/items/sil/missing.png"),
            Item::def_of(code())->properties
        });
    } else {
        return std::optional<TooltipInfo>();
    }
}

QPixmap EncyclopediaSlot::pixmap() {
    if (!valid()) {
        return Item::pixmap_of(EMPTY_CODE);
    }

    if (undiscovered()) {
        return Item::sil_pixmap_of(code());
    }

    return Item::pixmap_of(code());
}

bool EncyclopediaSlot::valid() {
    return std::find_if(ITEM_DEFINITIONS.begin(), ITEM_DEFINITIONS.end(), [=](const ItemDefinition &def) {
        return code() == def.code;
    }) != ITEM_DEFINITIONS.end();
}

bool EncyclopediaSlot::undiscovered() {
    return gw()->game().history().find(code()) == gw()->game().history().end();
}

ItemCode EncyclopediaSlot::code() {
    return m_item_group + (m_y * ENCYCLOPEDIA_GROUP_COLS + m_x);
}
