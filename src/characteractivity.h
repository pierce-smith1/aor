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
#include "timedactivity.h"

class LKGameWindow;
class Character;

class CharacterActivity : public TimedActivity {
public:
    CharacterActivity();
    CharacterActivity(
        CharacterId id,
        ItemDomain action,
        const std::vector<ItemId> &owned_items,
        AorInt ms_total = 0,
        AorInt ms_left = 0
    );
    CharacterActivity(const CharacterActivity &) = delete;
    CharacterActivity &operator = (const CharacterActivity &) = delete;

    ActivityId id();
    ItemDomain &action();
    const std::vector<ItemId> &owned_item_ids();
    const std::vector<Item> owned_items();
    Character &character();

    virtual void complete() override;
    virtual void update_ui() override;

    static void refresh_ui_bars(Character &character);
    static QString domain_to_action_string(ItemDomain domain);

    void serialize(QIODevice *dev) const;
    static CharacterActivity *deserialize(QIODevice *dev);

private:
    void exhaust_reagents();
    void exhaust_character();
    void exhaust_item(ItemId id);
    std::vector<Item> products();
    void give(const std::vector<Item> &items);
    void give_bonuses();
    void give_injuries();
    void clear_injuries();

    ActivityId m_id;
    ItemDomain m_action;
    std::vector<ItemId> m_owned_items;
    CharacterId m_char_id;

    friend class Character;
};

class Activities : public std::deque<CharacterActivity *> {
public:
    reference front();
};
