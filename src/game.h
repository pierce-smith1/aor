#pragma once

#include <QProgressBar>

#include <algorithm>

#include "character.h"
#include "inventory.h"
#include "generators.h"

using Characters = std::map<CharacterId, Character>;

class Game {
public:
    Game();

    Characters &characters();
    Inventory &inventory();
    GameId game_id();

    void add_character(const QString &name);
    void add_item(const Item &item);
    TooltipText tooltip_text_for(const Item &item);

    QString id_to_name(CharacterId char_id);

    void refresh_ui_bars(QProgressBar *activity, QProgressBar *morale, QProgressBar *energy, CharacterId char_id);

private:
    Characters m_explorers;
    Inventory m_inventory;
    GameId m_game_id;
};
