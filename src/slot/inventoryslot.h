#pragma once

#include "itemslot.h"

class InventorySlot : public ItemSlot {
public:
    InventorySlot(AorUInt y, AorUInt x);

    virtual void refresh() override;
    virtual bool is_draggable() override;
    virtual bool will_accept_drop(const SlotMessage &message) override;
    virtual void accept_message(const SlotMessage &message) override;
    virtual void install() override;

protected:
    virtual Item my_item() override;

private:
    AorUInt m_y, m_x;
};
