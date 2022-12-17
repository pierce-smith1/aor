#pragma once

#include <QProgressBar>

#include <algorithm>
#include <set>

#include "character.h"
#include "inventory.h"
#include "generators.h"
#include "io.h"

const static int MAX_EXPLORERS = 11;

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
    quint64 &actions_done();

    bool add_character(const QString &name, const std::multiset<Color> &heritage);
    bool add_item(const Item &item);
    void check_hatch();
    int trade_level();
    int foreign_trade_level(GameId tribe_id);
    ItemProperties total_resources();

    QString id_to_name(CharacterId char_id);

    void refresh_ui_bars(QProgressBar *activity, QProgressBar *morale, QProgressBar *energy, CharacterId char_id);

    void serialize(QIODevice *dev);
    static Game *deserialize(QIODevice *dev);
    static Game *new_game();

private:
    Characters m_explorers;
    Inventory m_inventory;
    ForeignTribes m_tribes;
    Offer m_trade_offer {};
    bool m_accepting_trade = false;
    RemoteOffer m_accepted_offer {};
    GameId m_trade_partner = NOBODY;
    GameId m_game_id;
    QString m_tribe_name;
    ItemHistory m_history;
    quint64 m_actions_done;
};
