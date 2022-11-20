#include "effectslot.h"
#include <qt6/QtWidgets/qwidget.h>
#include <string>

EffectSlot::EffectSlot(LKGameWindow *game, int n)
    : ItemSlot(game), n(n), effect_code(0)
{
    setObjectName(make_internal_name("effect_slot", n));
    item_layout->setObjectName(make_internal_name("effect_slot", n));
    item_label->setObjectName(make_internal_name("effect_slot", n));

    game->register_slot_name(objectName().toStdString());
}

Item EffectSlot::get_item() {
    return game->read_state<Item>([=](const State &state) {
        return state.effects[n];
    });
}

void EffectSlot::set_item(const Item &item) {
    if (Item::def_of(item)->type & IT_EFFECT) {
        game->mutate_state([=](State &state) {
            state.effects[n] = item;
        });
        return;
    }

    qFatal("Tried to slot non-effect item into effect slot (code %d, slotn %d)", item.code, n);
}

SlotType EffectSlot::get_type() {
    return SlotType::EffectSlot;
}

void EffectSlot::refresh_pixmap() {
    ItemSlot::refresh_pixmap();
}

void EffectSlot::insert_effect_slots(LKGameWindow &window) {
    for (int i = 0; i < EFFECT_SLOTS; i++) {
        window.window.effect_group->layout()->addWidget(new EffectSlot(&window, i));
    }
}

QString EffectSlot::make_internal_name(const std::string &base, int n) {
    return QString::fromStdString(base + ";" + std::to_string(n));
}

void EffectSlot::mousePressEvent(QMouseEvent *) {
    // Don't drag anything out of me!
    return;
}

void EffectSlot::dragEnterEvent(QDragEnterEvent *) {
    // Don't drag anything into me! No! Don't want it!
    return;
}
