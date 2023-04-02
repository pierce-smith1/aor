#pragma once

#include "itemslot.h"

class ForeignTradeSlot : public ItemSlot {
public:
    ForeignTradeSlot(size_t n);

    virtual bool will_accept_drop(const SlotMessage &message) override;
    virtual bool is_draggable() override;
    virtual void install() override;

protected:
    virtual Item my_item() override;

private:
    size_t m_n;
};
