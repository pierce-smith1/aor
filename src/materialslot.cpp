#include "materialslot.h"

MaterialSlot::MaterialSlot(size_t n)
    : ExternalSlot(n) {}

bool MaterialSlot::will_accept_drop(const DropPayload &payload) {
    if (!ExternalSlot::will_accept_drop(payload)) {
        return false;
    }

    return Item::has_resource_value(dropped_item(payload).code);
}

void MaterialSlot::accept_drop(const DropPayload &payload) {
    payload.source->after_dropped_elsewhere(DropPayload(my_item().id, this));

    if (my_item().id != EMPTY_ID && !(payload.source->type() & Material)) {
        // Whatever item is in this slot right now is going to be evicted,
        // unless the source is also a material slot.
        inventory().get_item_ref(my_item().id).intent = None;
    }

    external_ids()[Material][m_n] = dropped_item(payload).id;

    if (my_item().id != EMPTY_ID) {
        inventory().get_item_ref(my_item().id).intent = Material;
    }
}

void MaterialSlot::after_dropped_elsewhere(const DropPayload &response_payload) {
    Item item = dropped_item(response_payload);

    // If my item was dragged into a slot that also contained that item,
    // we handle that by evicting the item from me.
    if (my_item().id == item.id) {
        inventory().get_item_ref(my_item().id).intent = None;
        external_ids()[Material][m_n] = EMPTY_ID;
        return;
    }

    if (my_item().id != EMPTY_ID) {
        inventory().get_item_ref(my_item().id).intent = None;
    }

    external_ids()[Material][m_n] = item.id;

    if (my_item().id != EMPTY_ID) {
        inventory().get_item_ref(my_item().id).intent = Material;
    }
}

void MaterialSlot::install() {
    gw()->window().smith_layout->addWidget(this, m_n / SMITHING_SLOTS_PER_ROW + 1, m_n % SMITHING_SLOTS_PER_ROW);
}

void MaterialSlot::on_right_click(QMouseEvent *) {
    if (my_item().id != EMPTY_ID) {
        inventory().get_item_ref(my_item().id).intent = None;
    }

    external_ids()[Material][m_n] = EMPTY_ID;
}

Item MaterialSlot::my_item() {
    return inventory().get_item(external_ids()[Material][m_n]);
}

ItemDomain MaterialSlot::type() {
    return Material;
}
