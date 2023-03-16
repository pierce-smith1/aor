#pragma once

#include "itemslot.h"

class SkillSlot : public ItemSlot {
public:
    SkillSlot(size_t m_n);

    virtual void install() override;

protected:
    virtual Item my_item() override;

private:
    size_t m_n;
};
