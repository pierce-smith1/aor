#pragma once

#include <cstdint>
#include <functional>
#include <map>

#include "state.h"

using BuffCode = ItemCode;

struct BuffDefinition {
    std::function<void(State &)> apply_buff;
    std::function<void(State &)> remove_buff;
};

const static std::map<BuffCode, BuffDefinition> BUFF_DEFINITIONS = {

};
