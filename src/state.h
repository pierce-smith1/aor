#pragma once

#include <string>
#include <array>
#include <map>
#include <iostream>
#include <fstream>

#include "actions.h"
#include "items.h"
#include "trade.h"
#include "generators.h"

const static std::uint16_t STATE_VERSION = 1;

using Inventory = std::array<Item, INVENTORY_SIZE>;

struct State {
    State();

    ItemId get_item_id_at(int y, int x) const;
    Item get_item_instance(ItemId id) const;
    Item get_item_instance_at(int y, int x) const;
    void copy_item_to(const Item &item, int y, int x);
    void remove_item_at(int y, int x);
    ItemId make_item_at(ItemDefinitionPtr def, int y, int x);
    void mutate_item_at(std::function<void(Item &)> action, int y, int x);

    std::string name;
    Inventory inventory;
    CharacterActivity activity;
    std::uint16_t energy;
};

namespace StateSerialize {
    void save_state(const State &state, const std::string &filename);
    State *load_state(const std::string &filename);

    void put_char(std::ostream &out, unsigned char c);
    void put_short(std::ostream &out, std::uint16_t n);
    void put_long(std::ostream &out, std::uint64_t n);
    void put_string(std::ostream &out, const std::string &s);
    void put_inventory(std::ostream &out, const Inventory &i);

    unsigned char get_char(std::istream &in);
    std::uint16_t get_short(std::istream &in);
    std::uint64_t get_long(std::istream &in);
    std::string get_string(std::istream &in);
    Inventory get_inventory(std::istream &in);
}
