#pragma once

#include "slot.h"

class EncyclopediaSlot : public Slot {
public:
    EncyclopediaSlot(size_t y, size_t x, ItemCode item_group);

    virtual bool do_hovering() override;
    virtual std::optional<Item> tooltip_item() override;
    virtual std::optional<TooltipInfo> tooltip_info() override;

    virtual QPixmap pixmap() override;

    bool valid();

private:
    bool undiscovered();
    ItemCode code();

    size_t m_y, m_x;
    ItemCode m_item_group;
};
