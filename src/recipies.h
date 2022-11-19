#pragma once

#include <map>
#include <string>
#include <set>

#include "items.h"

using ToolRecipies = std::map<std::string, std::multiset<std::string>>;

const static ToolRecipies RECIPIES = {
    { "eliding_hatchet", { "stone", "stone", "rusted_bar" } }
};

void validate_recipies();
