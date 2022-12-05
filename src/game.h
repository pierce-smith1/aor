#pragma once

#include <QProgressBar>

#include <algorithm>

#include "character.h"
#include "inventory.h"
#include "generators.h"
#include "io.h"

struct TradeState {
    bool remote_accepted = false;
    std::array<Item, TRADE_SLOTS> offer {};
};

using Characters = std::map<CharacterId, Character>;
using ForeignTribes = std::map<GameId, TradeState>;
using Offer = std::array<ItemId, TRADE_SLOTS>;
using TradePurchases = std::array<Item, TRADE_SLOTS>;

class Game {
public:
    Game();

    Characters &characters();
    Inventory &inventory();
    ForeignTribes &tribes();
    Offer &trade_offer();
    bool &accepting_trade();
    GameId &trade_partner();
    GameId game_id();
    QString &tribe_name();

    void add_character(const QString &name);
    void add_item(const Item &item);
    TooltipText tooltip_text_for(const Item &item);

    QString id_to_name(CharacterId char_id);

    void refresh_ui_bars(QProgressBar *activity, QProgressBar *morale, QProgressBar *energy, CharacterId char_id);

    void serialize(QIODevice *dev);
    static Game *deserialize(QIODevice *dev);

private:
    Characters m_explorers;
    Inventory m_inventory;
    ForeignTribes m_tribes;
    Offer m_trade_offer {};
    bool m_accepting_trade = false;
    GameId m_trade_partner = NOBODY;
    GameId m_game_id;
    QString m_tribe_name;
};
