#include "actions.h"

std::vector<Item> Actions::smith(const std::vector<Item> &materials) {
    int total_power = 0;

    for (const Item &item : materials) {
        total_power += Item::def_of(item)->properties[ItemProperty::MaterialPower];
    }
}
