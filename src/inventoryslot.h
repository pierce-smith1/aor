#pragma once

#include "itemslot.h"

class InventorySlot : public ItemSlot {
public:
    InventorySlot(quint64 y, quint64 x);

    virtual void refresh() override;
    virtual bool is_draggable() override;
    virtual void accept_drop(const DropPayload &payload) override;
    virtual void after_dropped_elsewhere(const DropPayload &response_payload) override;
    virtual void install() override;
    virtual ItemDomain type() override;

protected:
    virtual Item my_item() override;

private:
    quint64 m_y, m_x;
};
