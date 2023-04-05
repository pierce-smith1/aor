// Copyright 2023 Pierce Smith
// This software is licensed under the terms of the Lesser GNU General Public License.

#include "itemslot.h"

QPixmap ItemSlot::pixmap() {
    return Item::pixmap_of(my_item());
}

bool ItemSlot::do_hovering() {
    return my_item().id != EMPTY_ID;
}

std::optional<Item> ItemSlot::tooltip_item() {
    return std::optional<Item>(my_item());
}

bool ItemSlot::is_draggable() {
    return my_item().id != EMPTY_ID;
}

bool ItemSlot::will_accept_drop(const SlotMessage &message) {
    return std::holds_alternative<Item>(message);
}

PayloadVariant ItemSlot::user_drop_data() {
    return my_item();
}

Item ItemSlot::my_item() { return Item(); }

Item ItemSlot::get_item(const SlotMessage &message) {
    return std::get<Item>(message);
}

Inventory &ItemSlot::inventory() {
    return gw()->game()->inventory();
}
