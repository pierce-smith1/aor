#pragma once

#include <cstdint>
#include <vector>

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

struct CharacterActivity {
    CharacterAction action;
    std::int64_t ms_left;
};

struct SmithingPower {
    int smithing;
    int foraging;
    int mining;
    int catching;
    int trading;
    int praying;
};
