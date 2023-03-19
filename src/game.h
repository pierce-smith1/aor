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

const static AorUInt MAX_EXPLORERS = 12;

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
using RunningActivities = std::multimap<ItemDomain, TimedActivity *>;

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
    ItemId &scan_item_id();
    AorUInt &actions_done();
    RunningActivities &running_activities();
    bool &fast_actions();
    bool &no_exhaustion();

    bool add_character(const QString &name, const std::multiset<Color> &heritage);
    bool add_item(const Item &item);
    void register_activity(ItemDomain at, TimedActivity *action);
    void unregister_activity(TimedActivity *action);
    void check_hatch();
    void check_tutorial(ItemDomain domain);
    AorInt trade_level();
    AorInt foreign_trade_level(GameId tribe_id);
    ItemProperties total_resources();
    ItemDomain intent_of(ItemId item_id);

    Character &character(CharacterId id);
    CharacterActivity *activity(ActivityId id);

    void refresh_ui_bars(QProgressBar *activity, QProgressBar *spirit, QProgressBar *energy, CharacterId char_id);

    void serialize(QIODevice *dev);
    static Game *deserialize(QIODevice *dev);
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
    ItemId m_scan_item_id;
    LocationId m_current_location_id = LocationDefinition::get_def("stochastic_forest").id;
    ConsumableWaste m_consumable_waste;
    MineableWaste m_mineable_waste;
    RunningActivities m_running_activities;

    // Transient
    ForeignTribes m_tribes;
    bool m_fast_actions = false;
    bool m_no_exhaustion = false;
};
