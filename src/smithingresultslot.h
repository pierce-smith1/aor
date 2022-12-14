#pragma once

#include "itemslot.h"

class SmithingResultSlot : public ItemSlot {
public:
    SmithingResultSlot();

    Item get_item() override;
    void set_item(const Item &item) override;
    void refresh_pixmap() override;

protected:
    void mousePressEvent(QMouseEvent *event) override;
    void dragEnterEvent(QDragEnterEvent *event) override;

private:
    ItemCode m_code = 0;
};
