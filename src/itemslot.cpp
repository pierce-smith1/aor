#include "itemslot.h"

QPixmap ItemSlot::pixmap() {
    return Item::pixmap_of(my_item());
}

bool ItemSlot::is_draggable() {
    return my_item().id != EMPTY_ID;
}

bool ItemSlot::will_accept_drop(const DropPayload &payload) {
    return std::holds_alternative<ItemId>(payload);
}

bool ItemSlot::do_hovering() {
    return my_item().id != EMPTY_ID;
}

DropPayload ItemSlot::get_payload() {
    return DropPayload(my_item().id, this);
}

std::optional<Item> ItemSlot::tooltip_item() {
    return std::optional<Item>(my_item());
}

Item ItemSlot::my_item() { return Item(); }

Item ItemSlot::dropped_item(const DropPayload &payload) {
    return inventory().get_item(std::get<ItemId>(payload));
}

Inventory &ItemSlot::inventory() {
    return gw()->game().inventory();
}
