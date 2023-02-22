#pragma once

#include <QObject>
#include <QTimerEvent>
#include <QString>

#include <cstdint>
#include <vector>
#include <algorithm>
#include <set>
#include <deque>

#include "items.h"

class LKGameWindow;
class Character;

const static qint64 ACTIVITY_TICK_RATE_MS = 500;

class CharacterActivity {
public:
    CharacterActivity();
    CharacterActivity(
        CharacterId id,
        ItemDomain action,
        const std::vector<ItemId> &owned_items,
        qint64 ms_total = 0,
        qint64 ms_left = 0
    );

    void start();

    ActivityId id();
    ItemDomain &action();
    qint64 &ms_left();
    qint64 &ms_total();
    const std::vector<ItemId> &owned_item_ids();
    const std::vector<Item> owned_items();
    int timer_id();
    bool &started();
    Character &character();

    double percent_complete();
    bool ongoing();
    void progress(qint64 ms);

    static QString domain_to_action_string(ItemDomain domain);

    void serialize(QIODevice *dev) const;
    static CharacterActivity *deserialize(QIODevice *dev);

private:
    void complete();

    void exhaust_reagents();
    void exhaust_character();
    void exhaust_item(ItemId id);
    std::vector<Item> products();
    void give(const std::vector<Item> &items);
    void give_bonuses();
    void give_injuries();

    ActivityId m_id;
    ItemDomain m_action;
    qint64 m_ms_left;
    qint64 m_ms_total;
    std::vector<ItemId> m_owned_items;
    bool m_started = false;
    int m_timer_id = 0;
    CharacterId m_char_id;

    friend class Character;
};

class Activities : public std::deque<CharacterActivity> {
public:
    reference front();
    static CharacterActivity empty_activity;
};
