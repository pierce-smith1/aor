#include "skillslot.h"

Item SkillSlot::get_item() {
    return Item();
}

void SkillSlot::set_item(const Item &) {}

void SkillSlot::insert_skill_slots() {
    gw()->window().skills_box->layout()->addWidget(new SkillSlot());
}
