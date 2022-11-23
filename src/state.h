#pragma once

#include <QMutex>

#include <string>
#include <array>
#include <map>
#include <iostream>
#include <fstream>
#include <algorithm>
#include <cstdlib>

#include "actions.h"
#include "items.h"
#include "trade.h"
#include "generators.h"

const static std::uint16_t STATE_VERSION {1};
constexpr static int MAX_ARRAY_SIZE {std::max({ SMITHING_SLOTS, PRAYER_SLOTS, ARTIFACT_SLOTS })};

const static std::uint16_t BASE_MAX_ENERGY {50};
const static std::uint16_t BASE_MAX_MORALE {50};

using Inventory = std::array<Item, INVENTORY_SIZE>;
using ExternalItemIds = std::map<ItemDomain, std::array<ItemId, MAX_ARRAY_SIZE>>;
using Effects = std::array<Item, EFFECT_SLOTS>;
using ToolIds = std::map<ItemDomain, ItemId>;
using Buffs = std::multiset<ItemCode>;

struct State {
    State();

    ItemId get_item_id_at(int y, int x) const;
    Item &get_item_ref(ItemId id);
    Item &get_item_ref_at(int y, int x);
    Item get_item_instance(ItemId id) const;
    Item get_item_instance_at(int y, int x) const;
    void copy_item_to(const Item &item, int y, int x);
    void remove_item_at(int y, int x);
    void remove_item_with_id(ItemId id);
    bool add_item(const Item &def);
    ItemId make_item_at(ItemDefinitionPtr def, int y, int x);
    void mutate_item_at(std::function<void(Item &)> action, int y, int x);
    void add_energy(std::uint16_t energy);
    void add_morale(std::uint16_t morale);

    std::vector<Item> get_items_of_intent(ItemDomain intent);

    QString name;
    Inventory inventory;
    CharacterActivity activity;
    ExternalItemIds external_item_ids {
        { Material, {} },
        { Offering, {} },
        { Artifact, {} },
    };
    Effects effects {};
    ToolIds tool_ids {
        { SmithingTool, EMPTY_ID },
        { ForagingTool, EMPTY_ID },
        { MiningTool, EMPTY_ID },
        { PrayerTool, EMPTY_ID },
    };
    Buffs buffs {};
    std::uint16_t energy {40};
    std::uint16_t morale {40};
};

namespace StateSerialize {
    void save_state(const State &state, const QString &filename);
    State *load_state(const QString &filename);

    void put_char(std::ostream &out, unsigned char c);
    void put_short(std::ostream &out, std::uint16_t n);
    void put_long(std::ostream &out, std::uint64_t n);
    void put_string(std::ostream &out, const QString &s);
    template<size_t N> void put_item_array(std::ostream &out, const std::array<Item, N> &array) {
        put_short(out, array.size());
        for (const Item &item : array) {
            put_short(out, item.code);
            put_long(out, item.id);
            put_char(out, item.uses_left);
            put_short(out, item.intent);
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
    QString get_string(std::istream &in);
    template<size_t N> std::array<Item, N> get_item_array(std::istream &in) {
        std::uint16_t size {get_short(in)};
        std::array<Item, N> array {};

        for (std::uint16_t i {0}; i < size; i++) {
            Item item;
            item.code = get_short(in);
            item.id = get_long(in);
            item.uses_left = get_char(in);
            item.intent = (ItemDomain) get_short(in);

            array[i] = item;
        }

        return array;
    }
    template<size_t N> std::array<ItemId, N> get_id_array(std::istream &in) {
        std::uint16_t size {get_short(in)};
        std::array<ItemId, N> array {};

        for (std::uint16_t i {0}; i < size; i++) {
            array[i] = get_long(in);
        }

        return array;
    }
}
