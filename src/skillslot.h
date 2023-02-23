#pragma once

#include "itemslot.h"

class SkillSlot : public ItemSlot {
public:
    virtual Item get_item();
    virtual void set_item(const Item &item);

    static void insert_skill_slots();
};
