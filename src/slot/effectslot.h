// Copyright 2023 Pierce Smith
// This software is licensed under the terms of the Lesser GNU General Public License.

#pragma once

#include "itemslot.h"

class EffectSlot : public ItemSlot {
public:
    EffectSlot(size_t n);

    virtual bool will_accept_drop(const SlotMessage &message) override;
    virtual void install() override;

protected:
    virtual Item my_item() override;

private:
    size_t m_n;
};
