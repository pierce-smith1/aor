#include "actions.h"

CharacterActivity::CharacterActivity(CharacterAction action, std::int64_t ms)
    : action(action), ms_left(ms), ms_total(ms) { }

std::vector<Item> Actions::smith(const std::vector<Item> &materials) {
    return {};
}
