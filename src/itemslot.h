#pragma once

#include "slot.h"

class ItemSlot : public Slot {
public:
    ItemSlot() = default;

    virtual QPixmap pixmap() override;
    virtual bool is_draggable() override;
    virtual bool will_accept_drop(const DropPayload &payload) override;
    virtual DropPayload get_payload() override;

    virtual bool do_hovering() override;
    virtual std::optional<Item> tooltip_item() override;

protected:
    virtual Item my_item();
    virtual Item dropped_item(const DropPayload &payload);
    Inventory &inventory();
};
