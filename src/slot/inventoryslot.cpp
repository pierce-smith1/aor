#include "inventoryslot.h"

InventorySlot::InventorySlot(AorUInt y, AorUInt x)
    : m_y(y), m_x(x) {}

void InventorySlot::refresh() {
    Slot::refresh();

    if (gw()->game()->intent_of(my_item().id) != None) {
        m_opacity_effect->setOpacity(0.5);
    } else {
        m_opacity_effect->setOpacity(1.0);
    }
}

bool InventorySlot::is_draggable() {
    return ItemSlot::is_draggable() && gw()->game()->intent_of(my_item().id) == None;
}

bool InventorySlot::will_accept_drop(const SlotMessage &message) {
    return std::holds_alternative<Item>(message);
}

void InventorySlot::accept_message(const SlotMessage &message) {
    switch (message.type) {
        case SlotUserDrop: {
            message.source->accept_message(SlotMessage(SlotSetItem, my_item(), this));
            accept_message(SlotMessage(SlotSetItem, message, this));
            break;
        } case SlotSetItem: {
            Item item = get_item(message);

            // If another item with the same id already exists, destroy that bitch
            inventory().get_item_ref(item.id) = Item();

            inventory().put_item(item, m_y, m_x);
            break;
        } default: {}
    }
}

void InventorySlot::install() {
    QGridLayout *inventory_grid = dynamic_cast<QGridLayout *>(gw()->window().inventory_slots->layout());
    inventory_grid->addWidget(this, m_y, m_x);
}

Item InventorySlot::my_item() {
    return inventory().get_item(m_y, m_x);
}
