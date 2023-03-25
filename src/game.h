#pragma once

#include <QProgressBar>

#include <algorithm>
#include <set>

#include "character.h"
#include "inventory.h"
#include "generators.h"
#include "io.h"
#include "hooks.h"
#include "map.h"
#include "settings.h"

const static AorUInt MAX_EXPLORERS = 12;
const static AorUInt ACTIONS_UNTIL_WELCHIAN = 400;
const static AorUInt STUDY_SLOTS_PER_DOMAIN = 2;
const static AorInt LORE_PER_SCAN = 5;
const static AorInt BASE_MAX_LORE = 50;

using RemoteOffer = std::array<Item, TRADE_SLOTS>;

struct TradeState {
    bool remote_accepted = false;
    RemoteOffer offer {};
};

using Characters = std::array<Character, MAX_EXPLORERS>;
using ForeignTribes = std::map<GameId, TradeState>;
using Offer = std::array<ItemId, TRADE_SLOTS>;
using TradePurchases = std::array<Item, TRADE_SLOTS>;
using ItemHistory = std::set<ItemCode>;
using ConsumableWaste = std::map<LocationId, AorUInt>;
using MineableWaste = std::map<LocationId, AorUInt>;
using RunningActivities = std::vector<TimedActivity *>;
using StudiedItems = std::map<ItemDomain, std::array<ItemId, STUDY_SLOTS_PER_DOMAIN>>;

class Game {
public:
    Game();

    Characters &characters();
    Inventory &inventory();
    ForeignTribes &tribes();
    Offer &trade_offer();
    bool &accepting_trade();
    RemoteOffer &accepted_offer();
    GameId &trade_partner();
    GameId game_id();
    QString &tribe_name();
    ItemHistory &history();
    WorldMap &map();
    LocationId &current_location_id();
    AorUInt &actions_done();
    RunningActivities &running_activities();
    ConsumableWaste &forageable_waste();
    MineableWaste &mineable_waste();
    StudiedItems &studied_items();
    ClampedResource &lore();

    Settings &settings();

    bool &fast_actions();
    bool &no_exhaustion();

    bool add_character(const QString &name, const std::multiset<Color> &heritage);
    bool add_item(const Item &item);
    void register_activity(TimedActivity *action);
    void unregister_activity(TimedActivity *action);
    void check_hatch();
    void check_tutorial(ItemDomain domain);
    AorInt trade_level();
    AorInt foreign_trade_level(GameId tribe_id);
    ItemProperties total_smithing_resources(CharacterId character_id);
    ItemProperties total_resources();
    ItemDomain intent_of(ItemId item_id);

    template <typename ActivityType> std::vector<ActivityType *> activities_of_type(ItemDomain type) {
        std::vector<TimedActivity *> activities;
        std::vector<ActivityType *> typed_activities;

        auto &acts = m_running_activities;
        std::copy_if(acts.begin(), acts.end(), std::back_inserter(activities), [=](TimedActivity *act) {
            return act->type() == type;
        });

        std::transform(activities.begin(), activities.end(), std::back_inserter(typed_activities), [=](TimedActivity *act) {
            return dynamic_cast<ActivityType *>(act);
        });

        return typed_activities;
    }

    void start_scan();
    bool can_scan();
    bool can_travel(LocationId id);
    void start_travel(LocationId id);
    AorInt forageables_left(LocationId id);
    AorInt forageables_left();
    AorInt mineables_left(LocationId id);
    AorInt mineables_left();

    Character &character(CharacterId id);
    CharacterActivity *activity(ActivityId id);

    void serialize(QIODevice *dev);
    void deserialize(QIODevice *dev);

    static Game *new_game();

private:
    Characters m_explorers;
    Inventory m_inventory;
    Offer m_trade_offer {};
    bool m_accepting_trade = false;
    RemoteOffer m_accepted_offer {};
    GameId m_trade_partner = NO_TRIBE;
    GameId m_game_id;
    QString m_tribe_name;
    ItemHistory m_history;
    AorUInt m_actions_done = 0;
    WorldMap m_map;
    LocationId m_current_location_id = LocationDefinition::get_def("stochastic_forest").id;
    ConsumableWaste m_consumable_waste;
    MineableWaste m_mineable_waste;
    RunningActivities m_running_activities;
    StudiedItems m_studied_items {};
    ClampedResource m_lore = ClampedResource(0, BASE_MAX_LORE, HookCalcMaxLore);

    Settings m_settings;

    // Transient
    ForeignTribes m_tribes;
    bool m_fast_actions = false;
    bool m_no_exhaustion = false;
};
