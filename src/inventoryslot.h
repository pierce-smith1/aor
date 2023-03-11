#pragma once

#include "itemslot.h"

class InventorySlot : public ItemSlot {
public:
    InventorySlot(quint64 y, quint64 x);

    virtual QPixmap pixmap() override;
    virtual bool will_accept_drop(const DropPayload &payload) override;
    virtual bool is_draggable() override;
    virtual void accept_drop(const DropPayload &payload) override;
    virtual void after_dropped_elsewhere(const DropPayload &response_payload) override;
    virtual DropPayload get_payload() override;
    virtual void install() override;

    virtual bool do_hovering() override;
    virtual std::optional<Item> tooltip_item() override;

private:
    Inventory &inventory();
    Item my_item();

    quint64 y, x;
};
