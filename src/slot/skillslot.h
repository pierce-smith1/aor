#pragma once

#include "itemslot.h"

class SkillSlot : public ItemSlot {
public:
    SkillSlot(size_t m_n);

    virtual void install() override;

private:
    size_t m_n;
};
