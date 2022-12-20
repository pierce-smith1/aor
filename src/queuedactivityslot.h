#pragma once

#include "itemslot.h"

class QueuedActivitySlot : public ItemSlot {
public:
    QueuedActivitySlot(size_t n);

    Item get_item() override;
    void set_item(const Item &item) override;
    void refresh_pixmap() override;

    static void insert_queued_activity_slots();

    size_t n;

protected:
    bool do_hovering() override;
    std::optional<Item> tooltip_item() override;
    std::optional<TooltipInfo> tooltip_info() override;

    void mousePressEvent(QMouseEvent *event) override;
    void dragEnterEvent(QDragEnterEvent *event) override;

private:
    bool no_activity();
};
