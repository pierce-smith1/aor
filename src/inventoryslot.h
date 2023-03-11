#pragma once

#include "itemslot.h"

class InventorySlot : public ItemSlot {
public:
    InventorySlot(quint64 y, quint64 x);

    virtual QPixmap pixmap() override;
    virtual bool will_accept_drop(const DropPayload &payload) override;
    virtual void accept_drop(const DropPayload &payload) override;
    virtual void after_dropped_elsewhere(const DropPayload &response_payload) override;
    virtual DropPayload get_payload() override;

private:
    Inventory &inventory();

    quint64 y, x;
};
