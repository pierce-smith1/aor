#pragma once

#include "items.h"

const static quint64 ACTIONS_TO_HATCH = 20;
const static quint16 EGG_CODE = CT_OTHER | 0;

struct EggItem : public Item {
    EggItem(CharacterId parent1, CharacterId parent2);
    EggItem();

    static void check_hatch();
};
