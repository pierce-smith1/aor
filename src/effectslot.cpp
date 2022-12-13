#include "effectslot.h"

EffectSlot::EffectSlot(int n)
    : ItemSlot(), n(n), m_effect_code(0)
{
    setObjectName(make_internal_name("effect_slot", n));
    m_item_layout->setObjectName(make_internal_name("effect_slot", n));
    m_item_label->setObjectName(make_internal_name("effect_slot", n));

    gw()->register_slot(this);
}

Item EffectSlot::get_item() {
    return gw()->selected_char().effects()[n];
}

void EffectSlot::set_item(const Item &item) {
    if (item.def()->type & Effect) {
        gw()->selected_char().effects()[n] = item;
        return;
    }

    qFatal("Tried to slot non-effect item into effect slot (code %d, slotn %d)", item.code, n);
}

ItemDomain EffectSlot::type() {
    return Effect;
}

void EffectSlot::refresh_pixmap() {
    ItemSlot::refresh_pixmap();
}

void EffectSlot::insert_effect_slots() {
    for (int i = 0; i < EFFECT_SLOTS; i++) {
        gw()->window().effect_group->layout()->addWidget(new EffectSlot(i));
    }
}

QString EffectSlot::make_internal_name(const QString &base, int n) {
    return QString("%1;%2").arg(base).arg(n);
}

void EffectSlot::mousePressEvent(QMouseEvent *) {
    // Don't drag anything out of me!
    return;
}

void EffectSlot::dragEnterEvent(QDragEnterEvent *) {
    // Don't drag anything into me! No! Don't want it!
    return;
}
