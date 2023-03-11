#include "inventoryslot.h"

InventorySlot::InventorySlot(quint64 y, quint64 x)
    : y(y), x(x) {}

QPixmap InventorySlot::pixmap() {
    return Item::pixmap_of(inventory().get_item(y, x));
}

bool InventorySlot::will_accept_drop(const DropPayload &payload) {
    return std::holds_alternative<ItemId>(payload);
}

bool InventorySlot::is_draggable() {
    return my_item().id != EMPTY_ID;
}

void InventorySlot::accept_drop(const DropPayload &payload) {
    Item dropped_item = inventory().get_item(std::get<ItemId>(payload));

    payload.source->after_dropped_elsewhere(DropPayload(my_item().id, this));

    inventory().put_item(dropped_item, y, x);
}

void InventorySlot::after_dropped_elsewhere(const DropPayload &response_payload) {
    Item other_item = inventory().get_item(std::get<ItemId>(response_payload));

    inventory().put_item(other_item, y, x);
}

DropPayload InventorySlot::get_payload() {
    return DropPayload(my_item().id, this);
}

void InventorySlot::install() {
    QGridLayout *inventory_grid = dynamic_cast<QGridLayout *>(gw()->window().inventory_slots->layout());
    inventory_grid->addWidget(this, y, x);
}

bool InventorySlot::do_hovering() {
    return my_item().id != EMPTY_ID;
}

std::optional<Item> InventorySlot::tooltip_item() {
    return std::optional<Item>(my_item());
}

Inventory &InventorySlot::inventory() {
    return gw()->game().inventory();
}

Item InventorySlot::my_item() {
    return inventory().get_item(y, x);
}
