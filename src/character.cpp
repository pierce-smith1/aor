#include "character.h"

Character::Character() {
    put_item(Item("globfruit"), 0, 0);
    put_item(Item("globfruit"), 0, 1);
    put_item(Item("globfruit"), 0, 2);
}

CharacterActivity &Character::activity() {
    return state.activity;
}

bool Character::activity_ongoing() {
    return activity().action != None;
}

int Character::energy() {
    return state.energy;
}

int Character::morale() {
    return state.morale;
}

int Character::max_energy() {
    return 50;
}

int Character::max_morale() {
    return 50;
}

void Character::add_energy(int add) {
    if (add > state.energy) {
        state.energy = 0;
        return;
    }

    state.energy += add;
    if (state.energy > max_energy()) {
        state.energy = max_energy();
    }
}

void Character::add_morale(int add) {
    if (add > state.morale) {
        state.morale = 0;
        return;
    }

    state.morale += add;
    if (state.morale > max_morale()) {
        state.morale = max_morale();
    }
}

int Character::energy_gain() {
    int gain;

    switch (activity().action) {
        case Eating: {
            gain = accumulate_property(items_of_intent(Eating), ConsumableEnergyBoost);
            break;
        }
        case Smithing:
        case Foraging:
        case Mining: {
            gain = -tool(activity().action).def()->properties[ToolEnergyCost];
            break;
        }
        default: {
            gain = 0;
            break;
        }
    }

    if (morale() > (double) max_morale() * 0.7) {
        if (gain < 0) {
            gain /= 2;
        }
    }

    return gain;
}

int Character::morale_gain() {
    int gain;

    switch (activity().action) {
        case Eating: {
            gain = (items_of_intent(Eating), ConsumableMoraleBoost);
            break;
        }
        case Praying: {
            gain = std::accumulate(begin(inputs()), end(inputs()), 0, [&](int a, const Item &b) {
                return b.def()->item_level * 10 + a;
            });
            break;
        }
        default: {
            gain = -base_morale_cost();
            break;
        }
    }

    return gain;
}

int Character::base_morale_cost() {
    return 5;
}

Item &Character::item_ref(ItemId id) {
    if (id == EMPTY_ID) {
        Item::empty_item = Item(0);
        return Item::empty_item;
    }

    return state.get_item_ref(id);
}

Item Character::item(ItemId id) {
    return item_ref(id);
}

Item Character::item(int y, int x) {
    return item_ref(state.get_item_id_at(y, x));
}

Item &Character::item_ref(int y, int x) {
    return item_ref(state.get_item_id_at(y, x));
}

void Character::remove_item(ItemId id) {
    state.remove_item_with_id(id);
}

void Character::remove_item(int y, int x) {
    remove_item(state.get_item_id_at(y, x));
}

void Character::put_item(const Item &item, int y, int x) {
    state.copy_item_to(item, y, x);
}

bool Character::give_item(Item item) {
    return state.add_item(item);
}

Item Character::tool(ItemDomain domain) {
    return item(state.tool_ids[domain]);
}

ToolIds &Character::tools() {
    return state.tool_ids;
}

ExternalItemIds &Character::external_items() {
    return state.external_item_ids;
}

Effects &Character::effects() {
    return state.effects;
}

std::vector<Item> Character::inputs() {
    switch (activity().action) {
        case Eating: {
            return items_of_intent(Eating);
        }
        case Smithing: {
            return items_of_intent(Material);
        }
        case Praying: {
            return items_of_intent(Offering);
        }
        case Foraging:
        case Mining: {
            return {};
        }
        default: {
            qFatal("Tried to get inputs for unknown action domain (%d)", activity().action);
        }
    }
}

QString Character::name() {
    return state.name;
}

std::vector<Item> Character::items_of_intent(ItemDomain domain) {
    return state.get_items_of_intent(domain);
}

int Character::accumulate_property(const std::vector<Item> &items, ItemProperty prop) {
    return std::accumulate(begin(items), end(items), 0, [&](int a, const Item& b) {
        return b.def()->properties[prop] + a;
    });
}

bool Character::can_do(ItemDomain action) {
    switch (action) {
        case Eating: {
            return !activity_ongoing();
        }
        case Smithing: {
            const auto &materials {external_items()[Material]};
            bool enough_materials {std::all_of(begin(materials), begin(materials) + SMITHING_SLOTS, [&](ItemId a) {
                return a != EMPTY_ID;
            })};

            return enough_materials && !activity_ongoing();
        }
        case Praying: {
            const auto &offerings {external_items()[Offering]};
            bool enough_offerings {std::any_of(begin(offerings), begin(offerings) + PRAYER_SLOTS, [&](ItemId a) {
                return a != EMPTY_ID;
            })};

            return enough_offerings && !activity_ongoing();
        }
        case Foraging:
        case Mining: {
            return energy() >= tool(action).def()->properties[ToolEnergyCost] && !activity_ongoing();
        }
        default: {
            qFatal("Tried to assess whether character can do invalid action domain (%d)", action);
        }
    }
}
