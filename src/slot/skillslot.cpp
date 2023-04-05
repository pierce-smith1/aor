// Copyright 2023 Pierce Smith
// This software is licensed under the terms of the Lesser GNU General Public License.

#include "skillslot.h"

SkillSlot::SkillSlot(size_t n)
    : m_n(n)
{
    make_tall();
}

bool SkillSlot::will_accept_drop(const SlotMessage &) {
    return false;
}

void SkillSlot::install() {
    gw()->window().skills_box->layout()->addWidget(this);
}

Item SkillSlot::my_item() {
    return gw()->selected_char().skills()[m_n];
}
