// Copyright 2023 Pierce Smith
// This software is licensed under the terms of the Lesser GNU General Public License.

#include "externalslot.h"

ExternalSlot::ExternalSlot(size_t n)
    : m_n(n) {}

void ExternalSlot::on_right_click(QMouseEvent *) {
    accept_message(SlotMessage(SlotForgetItem, std::monostate(), this));
}

void ExternalSlot::accept_message(const SlotMessage &message) {
    switch (message.type) {
        case SlotUserDrop: {
            message.source->accept_message(SlotMessage(SlotForgetItem, std::monostate(), this));
            accept_message(SlotMessage(SlotSetItem, message, this));
            break;
        } case SlotSetItem: {
            accept_message(SlotMessage(SlotForgetItem, std::monostate(), this));

            if (!will_accept_drop(message)) {
                return;
            }

            my_item_id() = get_item(message).id;
            break;
        } case SlotForgetItem: {
            my_item_id() = EMPTY_ID;
            break;
        } default: {}
    }
}

Item ExternalSlot::my_item() {
    return inventory().get_item(my_item_id());
}

ItemId &ExternalSlot::my_item_id() {
    static ItemId mock_id = EMPTY_ID;
    return mock_id;
}
