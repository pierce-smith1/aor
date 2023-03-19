#pragma once

#include <QString>
#include <QObject>

#include <algorithm>
#include <map>
#include <array>
#include <deque>

#include "items.h"
#include "inventory.h"
#include "characteractivity.h"
#include "generators.h"

class Game;

const static AorUInt MAX_ARRAY_SIZE = std::max({ SMITHING_SLOTS, TRADE_SLOTS, ARTIFACT_SLOTS });
const static AorUInt MAX_QUEUED_ACTIVITIES = 8;
const static AorUInt MAX_SKILLS = 6;

const static AorUInt BASE_MAX_ENERGY = 50;
const static AorUInt BASE_MAX_SPIRIT = 50;
const static AorUInt BASE_MAX_RESOURCE = 10;

const static AorUInt BASE_HOOK_DOMAINS = Artifact | Effect | Explorer | Weather;

using Heritage = std::multiset<Color>;
using ExternalItemIds = std::map<ItemDomain, std::array<ItemId, MAX_ARRAY_SIZE>>;
using Effects = std::array<Item, EFFECT_SLOTS>;
using Skills = std::array<Item, MAX_SKILLS>;
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
    CharacterActivity *activity();

    AorInt &energy();
    AorInt &spirit();
    AorInt max_energy();
    AorInt max_spirit();
    AorInt base_spirit_cost();
    void add_energy(AorInt add);
    void add_spirit(AorInt add);
    AorUInt egg_find_percent_chance();

    bool can_perform_action(ItemDomain action);
    AorInt energy_to_gain();
    AorInt spirit_to_gain();

    std::vector<ItemCode> smithable_items();
    ItemCode smithing_result();
    ItemProperties total_material_resources();

    bool clear_last_effect();
    bool push_effect(const Item &effect);
    bool discover(const Item &item);

    void call_hooks(HookType type, const HookPayload &payload, AorUInt int_domain = BASE_HOOK_DOMAINS, const std::vector<Item> &extra_items = {});

    ItemId tool_id(ItemDomain domain);
    ToolIds &tools();
    ExternalItemIds &external_items();
    Effects &effects();
    Skills &skills();

    void serialize(QIODevice *dev) const;
    static Character *deserialize(QIODevice *dev);

private:
    Inventory &inventory();

    AorUInt m_id;
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
    Skills m_skills {};
    CharacterId m_partner = NOBODY;
    bool m_dead = false;
    bool m_can_couple = false;
    AorInt m_energy = 40;
    AorInt m_spirit = 40;
};
