#pragma once

#include <QProgressBar>

#include <algorithm>

#include "character.h"
#include "inventory.h"
#include "generators.h"

struct TradeState {
    bool remote_accepted = false;
    std::array<Item, TRADE_SLOTS> offer {};
};

using Characters = std::map<CharacterId, Character>;
using ForeignTribes = std::map<GameId, TradeState>;

class Game {
public:
    Game();

    Characters &characters();
    Inventory &inventory();
    ForeignTribes &tribes();
    GameId game_id();
    QString &tribe_name();

    void add_character(const QString &name);
    void add_item(const Item &item);
    TooltipText tooltip_text_for(const Item &item);

    QString id_to_name(CharacterId char_id);

    void refresh_ui_bars(QProgressBar *activity, QProgressBar *morale, QProgressBar *energy, CharacterId char_id);

private:
    Characters m_explorers;
    Inventory m_inventory;
    ForeignTribes m_tribes;
    GameId m_game_id;
    QString m_tribe_name;
};
