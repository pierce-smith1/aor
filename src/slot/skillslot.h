// Copyright 2023 Pierce Smith
// This software is licensed under the terms of the Lesser GNU General Public License.

#pragma once

#include "itemslot.h"

class SkillSlot : public ItemSlot {
public:
    SkillSlot(size_t m_n);

    virtual bool will_accept_drop(const SlotMessage &message) override;
    virtual void install() override;

protected:
    virtual Item my_item() override;

private:
    size_t m_n;
};
