#include "foreigntradeslot.h"

ForeignTradeSlot::ForeignTradeSlot(int n)
    : ItemSlot(), n(n)
{
    setObjectName(QString("foreign_slot:%1").arg(n));
    gw()->register_slot(this);
}

Item ForeignTradeSlot::get_item() {
    if (gw()->selected_tribe_id() == NOBODY) {
        return Item();
    } else {
        return gw()->game().tribes()[gw()->selected_tribe_id()].offer[n];
    }
}

void ForeignTradeSlot::set_item(const Item &item) {
    gw()->game().tribes().at(gw()->selected_tribe_id()).offer[n] = item;
    refresh_pixmap();
}

ItemDomain ForeignTradeSlot::type() {
    return ForeignOffering;
}

void ForeignTradeSlot::mousePressEvent(QMouseEvent *) { }

void ForeignTradeSlot::dragEnterEvent(QDragEnterEvent *) { }

