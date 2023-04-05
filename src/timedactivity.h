// Copyright 2023 Pierce Smith
// This software is licensed under the terms of the Lesser GNU General Public License.

#pragma once

#include <QTimer>
#include <deque>

#include "types.h"
#include "items.h"

const static AorInt ACTIVITY_TICK_RATE_MS = 500;

class TimedActivity : public Serializable {
public:
    TimedActivity();
    TimedActivity(AorInt ms_total, AorInt ms_left, ItemDomain type, std::vector<ItemId> owned_item_ids, CharacterId owner_id = NOBODY);

    void start();
    AorInt percent_complete() const;
    void complete();
    void progress();
    void update_ui() const;

    ItemDomain explorer_subtype() const;
    Character &owner() const;
    std::vector<Item> owned_items() const;

    void serialize(QIODevice *dev) const;
    void deserialize(QIODevice *dev);

    ActivityId id = NO_ACTION;
    AorInt ms_total;
    AorInt ms_left;
    bool active = false;
    bool finished = false;
    ItemDomain type;
    std::vector<ItemId> owned_item_ids;
    CharacterId owner_id = NOBODY;

    static TimedActivity empty_activity;
};

class Activities : public std::deque<ActivityId> {
public:
    Activities() = default;
    template <typename InputIt> Activities(InputIt first, InputIt last)
        : std::deque<ActivityId>(first, last) {}

    value_type front();
};
