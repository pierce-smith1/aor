#pragma once

#include <QString>
#include <QObject>

#include <algorithm>
#include <map>
#include <array>
#include <deque>

#include "items.h"
#include "inventory.h"
#include "actions.h"
#include "generators.h"

class Game;

const static int MAX_ARRAY_SIZE = std::max({ SMITHING_SLOTS, TRADE_SLOTS, ARTIFACT_SLOTS });
const static int MAX_QUEUED_ACTIVITIES = 8;

const static int BASE_MAX_ENERGY = 50;
const static int BASE_MAX_SPIRIT = 50;
const static int BASE_MAX_RESOURCE = 10;

using Heritage = std::multiset<Color>;
using ExternalItemIds = std::map<ItemDomain, std::array<ItemId, MAX_ARRAY_SIZE>>;
using Effects = std::array<Item, EFFECT_SLOTS>;
using ToolIds = std::map<ItemDomain, ItemId>;

class Character {
public:
    explicit Character();
    explicit Character(CharacterId id, const QString &name, const Heritage &heritage);

    QString &name();
    Heritage &heritage();
    Activities &activities();
    CharacterId id();
    CharacterId &partner();
    bool &dead();
    bool &can_couple();

    ItemProperties heritage_properties();

    void queue_activity(ItemDomain domain, const std::vector<ItemId> &items);
    CharacterActivity &activity();

    quint16 &energy();
    quint16 &spirit();
    int energy_int();
    int spirit_int();
    int max_energy();
    int max_spirit();
    int base_spirit_cost();
    void add_energy(int add);
    void add_spirit(int add);
    quint16 egg_find_percent_chance();

    bool can_perform_action(ItemDomain action);
    int energy_to_gain();
    int spirit_to_gain();

    std::vector<ItemCode> smithable_items();
    ItemCode smithing_result();
    ItemProperties total_material_resources();

    bool clear_last_effect();
    bool push_effect(const Item &effect);

    void call_hooks(HookType type, const HookPayload &payload, quint16 int_domain, const std::vector<Item> &extra_items = {});

    ItemId tool_id(ItemDomain domain);
    ToolIds &tools();
    ExternalItemIds &external_items();
    Effects &effects();

    void serialize(QIODevice *dev) const;
    static Character *deserialize(QIODevice *dev);

private:
    Inventory &inventory();

    quint16 m_id;
    QString m_name;
    Heritage m_heritage;
    Activities m_activities;
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
    bool m_dead = false;
    bool m_can_couple = false;
    quint16 m_energy = 40;
    quint16 m_spirit = 40;
};
