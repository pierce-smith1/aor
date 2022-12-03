#include "foreigntradeslot.h"

ForeignTradeSlot::ForeignTradeSlot(LKGameWindow *game_window, int n)
    : ItemSlot(game_window), n(n)
{
    setObjectName(QString("foreign_slot:%1").arg(n));
    game_window->register_slot_name(objectName());
}

Item ForeignTradeSlot::get_item() {
    if (m_game_window->selected_tribe_id() == NOBODY) {
        return Item();
    } else {
        return m_game_window->game().tribes()[m_game_window->selected_tribe_id()].offer[n];
    }
}

void ForeignTradeSlot::set_item(const Item &item) {
    m_game_window->game().tribes().at(m_game_window->selected_tribe_id()).offer[n] = item;
    refresh_pixmap();
}

ItemDomain ForeignTradeSlot::type() {
    return ForeignOffering;
}

void ForeignTradeSlot::mousePressEvent(QMouseEvent *) { }

void ForeignTradeSlot::dragEnterEvent(QDragEnterEvent *) { }

