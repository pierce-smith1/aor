// Copyright 2023 Pierce Smith
// This software is licensed under the terms of the Lesser GNU General Public License.

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

class CharacterActivity {
public:
    static void complete(const TimedActivity &activity);
    static void update_ui(const TimedActivity &activity);

    static void refresh_ui_bars(Character &character);
    static QString domain_to_action_string(ItemDomain domain);

    static void discover_waste_item(Character &character, ItemDomain domain, std::vector<WeightedVector<Item>> &discoverable_set);

private:
    static void exhaust_reagents(const TimedActivity &activity);
    static void exhaust_item(const TimedActivity &activity, ItemId id);
    static std::vector<Item> products(const TimedActivity &activity);
    static void give(const TimedActivity &activity, const std::vector<Item> &items);
    static void give_bonuses(const TimedActivity &activity);
    static void increase_threat(const TimedActivity &activity);
    static void start_next_action(const TimedActivity &activity);
    static void add_bonus_items(const TimedActivity &activity, std::vector<Item> &items);
};
