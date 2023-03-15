#include "materialslot.h"

MaterialSlot::MaterialSlot(size_t n)
    : ExternalSlot(n) {}

bool MaterialSlot::will_accept_drop(const SlotMessage &message) {
    if (!ExternalSlot::will_accept_drop(message)) {
        return false;
    }

    return Item::has_resource_value(get_item(message).code);
}

void MaterialSlot::accept_message(const SlotMessage &message) {
    switch (message.type) {
        case SlotUserDrop: {
            message.source->accept_message(SlotMessage(SlotForgetItem, std::monostate(), this));
            accept_message(SlotMessage(SlotSetItem, message, this));
            break;
        } case SlotSetItem: {
            accept_message(SlotMessage(SlotForgetItem, std::monostate(), this));
            external_ids()[Material][m_n] = get_item(message).id;
            break;
        } case SlotForgetItem: {
            external_ids()[Material][m_n] = EMPTY_ID;
            break;
        } default: {}
    }
}

void MaterialSlot::install() {
    gw()->window().smith_layout->addWidget(this, m_n / SMITHING_SLOTS_PER_ROW + 1, m_n % SMITHING_SLOTS_PER_ROW);
}

void MaterialSlot::on_right_click(QMouseEvent *) {
    accept_message(SlotMessage(SlotForgetItem, std::monostate(), this));
}

Item MaterialSlot::my_item() {
    return inventory().get_item(external_ids()[Material][m_n]);
}

ItemDomain MaterialSlot::type() {
    return Material;
}
