#include "actions.h"

CharacterActivity::CharacterActivity(ItemDomain action, std::int64_t ms)
    : action(action), ms_left(ms), ms_total(ms) { }
