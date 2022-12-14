#pragma once

#include <QObject>
#include <QTimerEvent>
#include <QString>

#include <cstdint>
#include <vector>
#include <algorithm>
#include <set>

#include "items.h"

class LKGameWindow;

const static qint64 ACTIVITY_TICK_RATE_MS = 500;

class CharacterActivity {
public:
    CharacterActivity(CharacterId id, ItemDomain action, qint64 ms_total = 0, qint64 ms_left = 0);

    ItemDomain &action();
    qint64 &ms_left();
    qint64 &ms_total();
    int timer_id();

    double percent_complete();
    bool ongoing();

    void progress(qint64 ms);

private:
    void complete();

    void exhaust_reagents();
    void exhaust_item(ItemId id);
    std::vector<Item> products();
    void give(const std::vector<Item> &items);
    void give_bonuses();

    ItemDomain m_action;
    qint64 m_ms_left;
    qint64 m_ms_total;
    int m_timer_id = 0;
    CharacterId m_char_id;

    friend class Character;
};
