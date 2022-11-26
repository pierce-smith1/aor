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
#include "generators.h"

using SourcePlayerId = std::uint64_t;
using PlayerIdentity = QString;
const static SourcePlayerId NOBODY = 0xffffffff;

const static std::uint16_t STATE_VERSION {1};
constexpr static int MAX_ARRAY_SIZE {std::max({ SMITHING_SLOTS, PRAYER_SLOTS, ARTIFACT_SLOTS })};

using Inventory = std::array<Item, INVENTORY_SIZE>;
using ExternalItemIds = std::map<ItemDomain, std::array<ItemId, MAX_ARRAY_SIZE>>;
using Effects = std::array<Item, EFFECT_SLOTS>;
using ToolIds = std::map<ItemDomain, ItemId>;
using SeenItems = std::set<ItemCode>;
using ItemForeignSources = std::map<ItemId, SourcePlayerId>;
using PlayerIdentities = std::map<SourcePlayerId, PlayerIdentity>;

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
    bool add_item(const Item &item);
    bool give_item_from(const Item &item, const PlayerIdentity &player);
    ItemId make_item_at(ItemDefinitionPtr def, int y, int x);
    void add_energy(std::uint16_t energy);
    void add_morale(std::uint16_t morale);
    SourcePlayerId add_player_identity(const PlayerIdentity &player);

    std::vector<Item> get_items_of_intent(ItemDomain intent);

    QString name;
    Inventory inventory;
    CharacterActivity activity;
    ExternalItemIds external_item_ids {
        { Material, {} },
        { Offering, {} },
        { KeyOffering, {} },
        { Artifact, {} },
    };
    Effects effects {};
    ToolIds tool_ids {
        { SmithingTool, EMPTY_ID },
        { ForagingTool, EMPTY_ID },
        { MiningTool, EMPTY_ID },
        { PrayerTool, EMPTY_ID },
    };
    SeenItems history;
    ItemForeignSources foreign_sources;
    PlayerIdentities foreign_players;
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
    void put_history(std::ostream &out, const SeenItems &h);
    void put_foreign_sources(std::ostream &out, const ItemForeignSources &s);
    void put_player_identities(std::ostream &out, const PlayerIdentities &i);
    template<size_t N> void put_item_array(std::ostream &out, const std::array<Item, N> &array) {
        put_short(out, array.size());
        for (const Item &item : array) {
            put_short(out, item.code);
            put_long(out, item.id);
            put_char(out, item.uses_left);
            put_short(out, item.intent);
        }
    }
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
    SeenItems get_history(std::istream &in);
    ItemForeignSources get_foreign_sources(std::istream &in);
    PlayerIdentities get_player_identities(std::istream &in);
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
