#pragma once

#include <string>
#include <array>
#include <map>

#include "items.h"
#include "trade.h"

const static int STATE_VERSION = 1;

struct CharacterState {
    std::string name;
    std::array<Item, INVENTORY_SIZE> inventory;
    std::map<std::string, SaleListing> sale_listings;
    SaleListing *active_sale_listing;
};
