#include "timedactivity.h"
#include "gamewindow.h"
#include "sounds.h"
#include "studyactivity.h"

TimedActivity TimedActivity::empty_activity;

TimedActivity::TimedActivity()
    : TimedActivity(0, 0, None, {})
{
    id = NO_ACTION;
}

TimedActivity::TimedActivity(AorInt ms_total, AorInt ms_left, ItemDomain type, std::vector<ItemId> owned_item_ids, CharacterId owner_id)
    : id(Generators::activity_id()), ms_total(ms_total), ms_left(ms_left), type(type), owned_item_ids(owned_item_ids), owner_id(owner_id) {}

void TimedActivity::start() {
    if (type == None || explorer_subtype() == None || ms_total == 0 || ms_left == 0) {
        return;
    }

    active = true;

    if (type & Explorer && gw()->game()->settings().sounds_on) {
        Sounds::activity_sounds().at(explorer_subtype())->play();
    }
}

AorInt TimedActivity::percent_complete() const {
    if (ms_total == 0) {
        return 0;
    }

    return (((ms_total - ms_left) * 100) / ms_total);
}

void TimedActivity::complete() {
    active = false;

    if (type & Explorer) {
        CharacterActivity::complete(*this);
    } else if (type & Study) {
        StudyActivity::complete(*this);
    }

    finished = true;
}

void TimedActivity::progress() {
    ms_left -= ACTIVITY_TICK_RATE_MS;
    if (ms_left <= 0) {
        complete();
    }
}

void TimedActivity::update_ui() const {
    if (type & Explorer) {
        CharacterActivity::update_ui(*this);
    } else if (type & Study) {
        StudyActivity::update_ui(*this);
    }
}

ItemDomain TimedActivity::explorer_subtype() const {
    return (ItemDomain) (type & ~Explorer);
}

Character &TimedActivity::owner() const {
    return gw()->game()->character(owner_id);
}

std::vector<Item> TimedActivity::owned_items() const {
    std::vector<Item> items;

    std::transform(owned_item_ids.begin(), owned_item_ids.end(), std::back_inserter(items), [=](ItemId id) {
        return gw()->game()->inventory().get_item(id);
    });

    return items;
}

void TimedActivity::serialize(QIODevice *dev) const {
    Serialize::serialize(dev, id);
    Serialize::serialize(dev, ms_total);
    Serialize::serialize(dev, ms_left);
    Serialize::serialize(dev, active);
    Serialize::serialize(dev, finished);
    Serialize::serialize(dev, type);
    Serialize::serialize(dev, owned_item_ids);
    Serialize::serialize(dev, owner_id);
}

void TimedActivity::deserialize(QIODevice *dev) {
    Serialize::deserialize(dev, id);
    Serialize::deserialize(dev, ms_total);
    Serialize::deserialize(dev, ms_left);
    Serialize::deserialize(dev, active);
    Serialize::deserialize(dev, finished);
    Serialize::deserialize(dev, type);
    Serialize::deserialize(dev, owned_item_ids);
    Serialize::deserialize(dev, owner_id);
}

Activities::value_type Activities::front() {
    if (empty()) {
        return NO_ACTION;
    } else {
        return std::deque<ActivityId>::front();
    }
}
