#include "foreigntradeslot.h"

ForeignTradeSlot::ForeignTradeSlot(size_t n)
    : m_n(n) {}

bool ForeignTradeSlot::will_accept_drop(const SlotMessage &) {
    return false;
}

bool ForeignTradeSlot::is_draggable() {
    return false;
}

void ForeignTradeSlot::install() {
    gw()->window().foreign_trade_slot_layout->addWidget(this);
}

Item ForeignTradeSlot::my_item() {
    if (gw()->game().trade_partner() != NOBODY) {
        return gw()->game().accepted_offer().at(m_n);
    } else if (gw()->selected_tribe_id() == NOBODY) {
        return Item();
    } else {
        return gw()->game().tribes()[gw()->selected_tribe_id()].offer.at(m_n);
    }
}
