// Copyright 2023 Pierce Smith
// This software is licensed under the terms of the Lesser GNU General Public License.

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
#include "locations.h"

class Game;

const static AorUInt MAX_ARRAY_SIZE = std::max({ SMITHING_SLOTS, TRADE_SLOTS, ARTIFACT_SLOTS });
const static AorUInt MAX_QUEUED_ACTIVITIES = 8;
const static AorUInt MAX_SKILLS = 6;

const static AorUInt BASE_MAX_RESOURCE = 10;

const static AorUInt BASE_HOOK_DOMAINS = Artifact | Explorer | Weather | Travelling | Resident;

using Heritage = std::multiset<Color>;
using ExternalItemIds = std::map<ItemDomain, std::array<ItemId, MAX_ARRAY_SIZE>>;
using Effects = std::array<Item, EFFECT_SLOTS>;
using Skills = std::array<Item, MAX_SKILLS>;
using ToolIds = std::map<ItemDomain, ItemId>;

class Character : public Serializable {
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
    LocationId &location_id();
    LocationId &next_location_id();

    ItemProperties heritage_properties();

    void queue_activity(ItemDomain domain, const std::vector<ItemId> &items);
    void queue_travel(LocationId location);
    void die();
    TimedActivity &activity();

    AorUInt egg_find_percent_chance();

    bool can_perform_action(ItemDomain action);

    std::vector<ItemCode> smithable_items();
    ItemCode smithing_result();
    ItemProperties total_material_resources();
    std::vector<Item> equipped_items();

    bool discover(const Item &item);

    void call_hooks(HookType type, const HookPayload &payload, AorUInt int_domain = BASE_HOOK_DOMAINS, const std::vector<Item> &extra_items = {});

    ItemId tool_id(ItemDomain domain);
    ToolIds &tools();
    ExternalItemIds &external_items();
    Skills &skills();

    static Character &mock_character();

    void serialize(QIODevice *dev) const;
    void deserialize(QIODevice *dev);

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
    ToolIds m_tool_ids {
        { SmithingTool, EMPTY_ID },
        { ForagingTool, EMPTY_ID },
        { MiningTool, EMPTY_ID },
    };
    Skills m_skills {};
    CharacterId m_partner = NOBODY;
    bool m_dead = false;
    bool m_can_couple = false;
    LocationId m_location_id = LocationDefinition::get_def("point_entry").id;
    LocationId m_next_location_id = NOWHERE;
};
