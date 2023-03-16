#pragma once

#include "slot.h"

class ItemSlot : public Slot {
public:
    ItemSlot() = default;

    virtual bool do_hovering() override;
    virtual std::optional<Item> tooltip_item() override;

    virtual QPixmap pixmap() override;
    virtual bool is_draggable() override;
    virtual bool will_accept_drop(const SlotMessage &message) override;
    virtual PayloadVariant user_drop_data() override;

protected:
    virtual Item my_item();
    virtual Item get_item(const SlotMessage &message);
    Inventory &inventory();
};
