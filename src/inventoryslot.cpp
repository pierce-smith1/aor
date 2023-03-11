#include "inventoryslot.h"

InventorySlot::InventorySlot(quint64 y, quint64 x)
    : y(y), x(x) {}

QPixmap InventorySlot::pixmap() {
    return Item::pixmap_of(inventory().get_item(y, x));
}

bool InventorySlot::will_accept_drop(const DropPayload &payload) {
    return std::holds_alternative<ItemId>(payload);
}

void InventorySlot::accept_drop(const DropPayload &payload) {
    Item dropped_item = inventory().get_item(std::get<ItemId>(payload));

    Item my_item = inventory().get_item(y, x);
    payload.source->after_dropped_elsewhere(DropPayload(my_item.id, this));

    inventory().put_item(dropped_item, y, x);
}

void InventorySlot::after_dropped_elsewhere(const DropPayload &response_payload) {
    Item other_item = inventory().get_item(std::get<ItemId>(response_payload));

    inventory().put_item(other_item, y, x);
}

DropPayload InventorySlot::get_payload() {
    return DropPayload(inventory().get_item(y, x).id, this);
}

Inventory &InventorySlot::inventory() {
    return gw()->game().inventory();
}
