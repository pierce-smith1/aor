#include "inventoryslot.h"

InventorySlot::InventorySlot(quint64 y, quint64 x)
    : m_y(y), m_x(x) {}

void InventorySlot::refresh() {
    Slot::refresh();

    if (my_item().intent != None) {
        m_opacity_effect->setOpacity(0.5);
    } else {
        m_opacity_effect->setOpacity(1.0);
    }
}

bool InventorySlot::is_draggable() {
    return ItemSlot::is_draggable() && my_item().intent == None;
}

void InventorySlot::accept_drop(const DropPayload &payload) {
    Item item = dropped_item(payload);

    // If I'm being dragged into an inventory slot, I should not have any intent.
    item.intent = None;

    payload.source->after_dropped_elsewhere(DropPayload(my_item().id, this));

    if (payload.source->type() & Material) {
        inventory().get_item_ref(item.id) = Item();
    }

    inventory().put_item(item, m_y, m_x);
}

void InventorySlot::after_dropped_elsewhere(const DropPayload &response_payload) {
    if (response_payload.source->type() & Resident) {
        inventory().put_item(dropped_item(response_payload), m_y, m_x);
    }
}

void InventorySlot::install() {
    QGridLayout *inventory_grid = dynamic_cast<QGridLayout *>(gw()->window().inventory_slots->layout());
    inventory_grid->addWidget(this, m_y, m_x);
}

Item InventorySlot::my_item() {
    return inventory().get_item(m_y, m_x);
}

ItemDomain InventorySlot::type() {
    return Resident;
}
