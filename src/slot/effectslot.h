#pragma once

#include "itemslot.h"

class EffectSlot : public ItemSlot {
public:
    EffectSlot(size_t n);

    virtual void install() override;

protected:
    virtual Item my_item() override;

private:
    size_t m_n;
};
