#pragma once

#include "itemslot.h"

class ExternalSlot : public ItemSlot {
public:
    ExternalSlot(size_t n);

protected:
    ExternalItemIds &external_ids();

    size_t m_n;
};
