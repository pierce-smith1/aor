// Copyright 2023 Pierce Smith
// This software is licensed under the terms of the Lesser GNU General Public License.

#include "materialslot.h"

MaterialSlot::MaterialSlot(size_t n)
    : ExternalSlot(n) {}

bool MaterialSlot::will_accept_drop(const SlotMessage &message) {
    if (!std::holds_alternative<Item>(message)) {
        return false;
    }

    return Item::has_resource_value(get_item(message).code);
}

void MaterialSlot::install() {
    gw()->window().smith_layout->addWidget(this, m_n / SMITHING_SLOTS_PER_ROW + 1, m_n % SMITHING_SLOTS_PER_ROW);
}

ItemId &MaterialSlot::my_item_id() {
    return gw()->selected_char().external_items()[Material][m_n];
}
