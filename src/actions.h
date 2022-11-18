#pragma once

#include <cstdint>
#include <vector>
#include <algorithm>

#include "items.h"

enum CharacterAction : unsigned char {
    Nothing = 0,
    Smithing,
    Eating,
    Foraging,
    Mining,
    Trading,
    Praying,
    Sleeping,
};

const static std::int64_t ACTIVITY_TICK_RATE_MS = 1000;

struct CharacterActivity {
    CharacterAction action;
    std::int64_t ms_left;
};

namespace Actions {
    std::vector<Item> smith(const std::vector<Item> &materials);
}
