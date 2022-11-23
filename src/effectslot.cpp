#include "effectslot.h"

EffectSlot::EffectSlot(LKGameWindow *game, int n)
    : ItemSlot(game), n(n), effect_code(0)
{
    setObjectName(make_internal_name("effect_slot", n));
    item_layout->setObjectName(make_internal_name("effect_slot", n));
    item_label->setObjectName(make_internal_name("effect_slot", n));

    game->register_slot_name(objectName());
}

Item EffectSlot::get_item() {
    return game->character.effects[n];
}

void EffectSlot::set_item(const Item &item) {
    if (item.def()->type & Effect) {
        game->character.effects[n] = item;
        return;
    }

    qFatal("Tried to slot non-effect item into effect slot (code %d, slotn %d)", item.code, n);
}

ItemDomain EffectSlot::get_item_slot_type() {
    return Effect;
}

void EffectSlot::refresh_pixmap() {
    ItemSlot::refresh_pixmap();
}

void EffectSlot::insert_effect_slots(LKGameWindow &window) {
    for (int i {0}; i < EFFECT_SLOTS; i++) {
        window.window.effect_group->layout()->addWidget(new EffectSlot(&window, i));
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
