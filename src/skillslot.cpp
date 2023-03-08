#include "skillslot.h"

Item SkillSlot::get_item() {
    return Item();
}

void SkillSlot::set_item(const Item &) {}

void SkillSlot::insert_skill_slots() {
    gw()->window().skills_box->layout()->addWidget(new SkillSlot());
}

void SkillSlot::refresh_pixmap() {
    m_item_label->setPixmap(Item::pixmap_of(get_item()));
}

void SkillSlot::dragEnterEvent(QDragEnterEvent *) {}
void SkillSlot::dropEvent(QDropEvent *) {}
