#include "skillslot.h"

SkillSlot::SkillSlot(size_t n)
    : m_n(n)
{
    make_tall();
}

void SkillSlot::install() {
    gw()->window().skills_box->layout()->addWidget(this);
}

Item SkillSlot::my_item() {
    return gw()->selected_char().skills()[m_n];
}
