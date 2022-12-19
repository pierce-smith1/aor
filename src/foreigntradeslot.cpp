#include "foreigntradeslot.h"

ForeignTradeSlot::ForeignTradeSlot(int n)
    : ItemSlot(), n(n)
{
    setObjectName(QString("foreign_slot:%1").arg(n));
    gw()->register_slot(this);
}

Item ForeignTradeSlot::get_item() {
    if (gw()->game().trade_partner() != NOBODY) {
        return gw()->game().accepted_offer().at(n);
    } else if (gw()->selected_tribe_id() == NOBODY) {
        return Item();
    } else {
        return gw()->game().tribes().at(gw()->selected_tribe_id()).offer.at(n);
    }
}

void ForeignTradeSlot::set_item(const Item &item) {
    gw()->game().tribes().at(gw()->selected_tribe_id()).offer.at(n) = item;
    refresh_pixmap();
}

ItemDomain ForeignTradeSlot::type() {
    return ForeignOffering;
}

void ForeignTradeSlot::mousePressEvent(QMouseEvent *) { }

void ForeignTradeSlot::dragEnterEvent(QDragEnterEvent *) { }

