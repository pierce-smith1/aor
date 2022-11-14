#pragma once

#include <string>
#include <vector>

#include "items.h"

struct SaleListing {
    std::string from;
    std::vector<ForeignItemId> items;
};
