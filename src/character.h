#pragma once

#include <QString>
#include <QObject>

#include <algorithm>
#include <map>
#include <array>

#include "items.h"
#include "inventory.h"
#include "actions.h"
#include "generators.h"

class Game;

const static int MAX_ARRAY_SIZE = std::max({ SMITHING_SLOTS, TRADE_SLOTS, ARTIFACT_SLOTS });

const static int BASE_MAX_ENERGY = 50;
const static int BASE_MAX_MORALE = 50;
const static int BASE_MAX_RESOURCE = 20;

using Heritage = std::multiset<Color>;
using ExternalItemIds = std::map<ItemDomain, std::array<ItemId, MAX_ARRAY_SIZE>>;
using Effects = std::array<Item, EFFECT_SLOTS>;
using ToolIds = std::map<ItemDomain, ItemId>;

struct Character {
    explicit Character();
    explicit Character(CharacterId id, const QString &name);

    QString &name();
    Heritage &heritage();
    CharacterActivity &activity();
    CharacterId id();
    CharacterId &partner();

    void start_activity(ItemDomain domain);

    quint16 &energy();
    quint16 &morale();
    int max_energy();
    int max_morale();
    int base_morale_cost();
    void add_energy(int add);
    void add_morale(int add);

    bool can_perform_action(ItemDomain action);
    int energy_to_gain();
    int morale_to_gain();

    std::vector<ItemCode> smithable_items();
    ItemCode smithing_result();

    bool clear_last_effect();
    bool push_effect(const Item &effect);

    ItemId tool_id();
    ItemId tool_id(ItemDomain domain);
    ToolIds &tools();
    ExternalItemIds &external_items();
    Effects &effects();

    void serialize(QIODevice *dev) const;
    static Character *deserialize(QIODevice *dev);

private:
    quint16 m_id;
    QString m_name;
    Heritage m_heritage;
    CharacterActivity m_activity;
    ExternalItemIds m_external_item_ids {
        { Material, {} },
        { Artifact, {} },
    };
    Effects m_effects {};
    ToolIds m_tool_ids {
        { SmithingTool, EMPTY_ID },
        { ForagingTool, EMPTY_ID },
        { MiningTool, EMPTY_ID },
    };
    CharacterId m_partner = NOBODY;
    quint16 m_energy = 40;
    quint16 m_morale = 40;
};
