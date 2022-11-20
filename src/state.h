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
using MaterialIds = std::array<ItemId, SMITHING_SLOTS>;
using OfferingIds = std::array<ItemId, PRAYER_SLOTS>;
using ArtifactIds = std::array<ItemId, ARTIFACT_SLOTS>;
using Effects = std::array<Item, EFFECT_SLOTS>;

struct State {
    State();

    ItemId get_item_id_at(int y, int x) const;
    Item &get_item_ref(ItemId id);
    Item &get_item_ref_at(int y, int x);
    Item get_item_instance(ItemId id) const;
    Item get_item_instance_at(int y, int x) const;
    void copy_item_to(const Item &item, int y, int x);
    void remove_item_at(int y, int x);
    ItemId make_item(ItemDefinitionPtr def);
    ItemId make_item_at(ItemDefinitionPtr def, int y, int x);
    void mutate_item_at(std::function<void(Item &)> action, int y, int x);

    int get_max_energy();

    std::string name;
    Inventory inventory;
    CharacterActivity activity;
    MaterialIds materials = {};
    OfferingIds offered_items = {};
    ArtifactIds artifacts = {};
    Effects effects = {};
    ItemId tool = EMPTY_ID;
    std::uint16_t energy = 40;
    std::uint16_t morale = 40;
};

namespace StateSerialize {
    void save_state(const State &state, const std::string &filename);
    State *load_state(const std::string &filename);

    void put_char(std::ostream &out, unsigned char c);
    void put_short(std::ostream &out, std::uint16_t n);
    void put_long(std::ostream &out, std::uint64_t n);
    void put_string(std::ostream &out, const std::string &s);
    template<size_t N> void put_item_array(std::ostream &out, const std::array<Item, N> &array) {
        put_short(out, array.size());
        for (const Item &item : array) {
            put_short(out, item.code);
            put_long(out, item.id);
            put_char(out, item.uses_left);
            put_char(out, item.intent);
        }
    }
    void put_inventory(std::ostream &out, const Inventory &i);
    template<size_t N> void put_id_array(std::ostream &out, const std::array<ItemId, N> &array) {
        put_short(out, array.size());
        for (ItemId id : array) {
            put_long(out, id);
        }
    }

    unsigned char get_char(std::istream &in);
    std::uint16_t get_short(std::istream &in);
    std::uint64_t get_long(std::istream &in);
    std::string get_string(std::istream &in);
    template<size_t N> std::array<Item, N> get_item_array(std::istream &in) {
        std::uint16_t size = get_short(in);
        std::array<Item, N> array = {};

        for (std::uint16_t i = 0; i < size; i++) {
            Item item;
            item.code = get_short(in);
            item.id = get_long(in);
            item.uses_left = get_char(in);
            item.intent = (ItemIntent) get_char(in);

            array[i] = item;
        }

        return array;
    }
    template<size_t N> std::array<ItemId, N> get_id_array(std::istream &in) {
        std::uint16_t size = get_short(in);
        std::array<ItemId, N> array;

        for (std::uint16_t i = 0; i < size; i++) {
            array[i] = get_long(in);
        }

        return array;
    }
}
