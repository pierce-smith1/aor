#pragma once

#include "itemslot.h"

class ForeignTradeSlot : public ItemSlot {
public:
    ForeignTradeSlot(int n);

    Item get_item() override;
    void set_item(const Item &item) override;
    ItemDomain type() override;

    int n;
protected:
    void mousePressEvent(QMouseEvent *event) override;
    void dragEnterEvent(QDragEnterEvent *event) override;
};
