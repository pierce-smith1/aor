#pragma once

#include <QString>

#include <algorithm>
#include <map>
#include <array>

#include "items.h"
#include "inventory.h"
#include "actions.h"
#include "generators.h"

class Game;

constexpr static int MAX_ARRAY_SIZE {std::max({ SMITHING_SLOTS, PRAYER_SLOTS, ARTIFACT_SLOTS })};

const static int BASE_MAX_ENERGY = 50;
const static int BASE_MAX_MORALE = 50;

using ExternalItemIds = std::map<ItemDomain, std::array<ItemId, MAX_ARRAY_SIZE>>;
using Effects = std::array<Item, EFFECT_SLOTS>;
using ToolIds = std::map<ItemDomain, ItemId>;

struct Character {
    Character(CharacterId id, const QString &name, Game *game);

    QString &name();
    QColor &color();
    CharacterActivity &activity();
    CharacterId id();

    bool activity_ongoing();
    double activity_percent_complete();
    std::uint64_t activity_time();

    std::uint16_t &energy();
    std::uint16_t &morale();
    int max_energy();
    int max_morale();
    void add_energy(int add);
    void add_morale(int add);

    bool can_perform_action(ItemDomain action);
    int energy_to_gain();
    int morale_to_gain();
    std::vector<Item> input_items();

    bool clear_last_effect();
    bool push_effect(const Item &effect);

    ItemId tool_id();
    ItemId tool_id(ItemDomain domain);
    ToolIds &tools();
    ExternalItemIds &external_items();
    Effects &effects();

private:
    QString m_name;
    QColor m_color;
    CharacterActivity m_activity {};
    ExternalItemIds m_external_item_ids {
        { Material, {} },
        { Offering, {} },
        { KeyOffering, {} },
        { Artifact, {} },
    };
    Effects m_effects {};
    ToolIds m_tool_ids {
        { SmithingTool, EMPTY_ID },
        { ForagingTool, EMPTY_ID },
        { MiningTool, EMPTY_ID },
        { PrayerTool, EMPTY_ID },
    };
    std::uint16_t m_energy {40};
    std::uint16_t m_morale {40};
    std::uint16_t m_id;

    Game *m_game;
};
