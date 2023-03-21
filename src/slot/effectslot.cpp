#include "effectslot.h"

EffectSlot::EffectSlot(size_t n)
    : m_n(n)
{
    make_wide();
}

bool EffectSlot::will_accept_drop(const SlotMessage &) {
    return false;
}

void EffectSlot::install() {
    gw()->window().effect_group->layout()->addWidget(this);
}

Item EffectSlot::my_item() {
    return gw()->selected_char().effects()[m_n];
}
