// Copyright 2023 Pierce Smith
// This software is licensed under the terms of the Lesser GNU General Public License.

#include <cstdlib>

#include "characteractivity.h"
#include "slot/externalslot.h"
#include "gamewindow.h"
#include "die.h"
#include "sounds.h"

QString CharacterActivity::domain_to_action_string(ItemDomain domain) {
    switch (domain) {
        case Eating: { return "Eating"; }
        case Smithing: { return "Smithing"; }
        case Mining: { return "Mining"; }
        case Foraging: { return "Foraging"; }
        case Trading: { return "Trading"; }
        case Defiling: { return "Defiling"; }
        case Coupling: { return "Coupling"; }
        case Travelling: { return "Traveling"; }
        default: {}
    }

    return "";
}

void CharacterActivity::complete(const TimedActivity &activity) {
    if (activity.explorer_subtype() == None) {
        return;
    }

    gw()->notify(ActionComplete, QString("%1 finished %2.")
        .arg(activity.owner().name())
        .arg(domain_to_action_string(activity.explorer_subtype()).toCaseFolded())
    );

    if (activity.explorer_subtype() == Trading) {
        gw()->connection().agreement_changed(gw()->game()->trade_partner(), false);
        for (AorUInt i = 0; i < TRADE_SLOTS; i++) {
            gw()->connection().offer_changed(Item(), i);
        }
        gw()->game()->accepting_trade() = false;
        gw()->game()->trade_partner() = NO_TRIBE;
    }

    give_bonuses(activity);
    std::vector<Item> items = products(activity);
    add_bonus_items(activity, items);
    exhaust_reagents(activity);
    give(activity, items);
    increase_threat(activity);

    start_next_action(activity);

    gw()->game()->check_hatch();

    if (activity.explorer_subtype() == Foraging) {
        gw()->game()->forageable_waste()[activity.owner().location_id()]++;
    } else if (activity.explorer_subtype() == Mining) {
        gw()->game()->mineable_waste()[activity.owner().location_id()]++;
    } else if (activity.explorer_subtype() == Travelling) {
        activity.owner().location_id() = activity.owner().next_location_id();
        activity.owner().next_location_id() = NOWHERE;
    }

    activity.owner().call_hooks(HookPostActivity, { &activity.owner() });
    gw()->refresh_ui();
}

void CharacterActivity::update_ui(const TimedActivity &activity) {
    if (gw()->selected_char_id() == activity.owner_id) {
        refresh_ui_bars(activity.owner());
    }
}

void CharacterActivity::refresh_ui_bars(Character &character) {
    QProgressBar *activity_bar = gw()->window().activity_time_bar;

    activity_bar->setMaximum(100);
    activity_bar->setValue(character.activity().percent_complete());
}

std::vector<Item> CharacterActivity::products(const TimedActivity &activity) {
    std::vector<WeightedVector<Item>> discoverable_set;

    switch (activity.explorer_subtype()) {
        case Smithing: {
            ItemCode smithing_result = activity.owner().smithing_result();

            if (smithing_result == EMPTY_CODE) {
                break;
            }

            Item result = Item(smithing_result);

            AorInt use_bonus = 0;
            activity.owner().call_hooks(HookCalcBonusProductUse, { &use_bonus });
            result.uses_left += use_bonus;

            discoverable_set.push_back({{ result, 1.0 }});
            break;
        } case Foraging:
          case Mining: {
            discover_waste_item(activity.owner(), activity.explorer_subtype(), discoverable_set);
            gw()->game()->waste_action_counts()[activity.owner().location_id()]++;
            break;
        } case Coupling: {
            auto &characters = gw()->game()->characters();

            // Coupling actions always end in pairs.
            // To avoid making two eggs, the first one that finishes their
            // coupling action resets their partner's partner member.
            // (That happens later in this very function.)
            // Here, we check to see if our partner has already made an egg
            // and break if so.
            auto partner = std::find_if(begin(characters), end(characters), [&](Character &other) {
                return other.partner() == activity.owner().id();
            });

            if (partner == end(characters)) {
                break;
            }

            discoverable_set.push_back({{ Item::make_egg(activity.owner_id, partner->id()), 1.0 }});

            partner->partner() = NOBODY;
            activity.owner().partner() = NOBODY;

            break;
        } case Trading: {
            for (const Item &item : gw()->game()->accepted_offer()) {
                discoverable_set.push_back({{ item, 1.0 }});
            }
            break;
        } default: {
            break;
        }
    }

    activity.owner().call_hooks(HookDecideProducts, { &discoverable_set, &activity.owner() });
    activity.owner().call_hooks(HookPostDecideProducts, { &discoverable_set, &activity.owner() });

    std::vector<Item> final_items;

    for (const WeightedVector<Item> &weighted_discoverables : discoverable_set) {
        final_items.push_back(Generators::sample_with_weights<Item>(weighted_discoverables));
    }

    return final_items;
}

void CharacterActivity::discover_waste_item(Character &character, ItemDomain domain, std::vector<WeightedVector<Item>> &discoverable_set) {
    Item tool = gw()->game()->inventory().get_item(character.tool_id(domain));
    const ItemProperties &tool_props = tool.def()->properties;

    if (tool.id == EMPTY_ID) {
        if (domain == Foraging) {
            discoverable_set.push_back({{ Item("globfruit"), 1.0 }, { Item("byteberry"), 1.0 }});
        } else if (domain == Mining) {
            discoverable_set.push_back({{ Item("oolite"), 1.0 }, { Item("obsilicon"), 1.0 }});
        }
    } else {
        WeightedVector<Item> possible_items;

        Item::for_each_tool_discover([&](ItemProperty product_prop, ItemProperty weight_prop) {
            if (tool_props[weight_prop] != 0) {
                possible_items.push_back({ Item(tool_props[product_prop]), tool_props[weight_prop] });
            }
        });

        discoverable_set.push_back(possible_items);

        if (domain == Foraging && Generators::percent_chance(character.egg_find_percent_chance())) {
            discoverable_set.push_back({{ Item::make_egg(), 1.0 }});
        }
    }

    LocationId here_id = character.location_id();
    LocationDefinition here = LocationDefinition::get_def(here_id);

    Item signature_item = gw()->game()->next_signature(here_id);
    if (signature_item.id != EMPTY_ID) {
        discoverable_set.push_back({{ signature_item, 1.0 }});
    }
}

void CharacterActivity::exhaust_reagents(const TimedActivity &activity) {
    if (activity.explorer_subtype() == Smithing) {
        for (ItemId id : activity.owner().external_items().at(Material)) {
            exhaust_item(activity, id);
        }
    } else if (activity.explorer_subtype() == Trading) {
        for (ItemId &id : gw()->game()->trade_offer()) {
            gw()->game()->inventory().remove_item(id);
            id = EMPTY_ID;
        }
    } else if (activity.explorer_subtype() == Eating || activity.explorer_subtype() == Defiling) {
        for (const ItemId id : activity.owned_item_ids) {
            exhaust_item(activity, id);
        }
    }

    exhaust_item(activity, activity.owner().tool_id(activity.explorer_subtype()));
}

void CharacterActivity::exhaust_item(const TimedActivity &activity, ItemId id) {
    if (id == EMPTY_ID) {
        return;
    }

    Item &item = gw()->game()->inventory().get_item_ref(id);

    if (item.uses_left > 0) {
        item.uses_left -= 1;
        if (item.uses_left == 0 && item.code & CT_TOOL) {
            activity.owner().tools().at(activity.explorer_subtype()) = EMPTY_ID;
            gw()->game()->inventory().remove_item(id);
        } else if (item.uses_left == 0) {
            gw()->game()->inventory().remove_item(id);
        }
    }

    if (activity.explorer_subtype() == Smithing) {
        for (ItemId &mid : activity.owner().external_items().at(Material)) {
            if (id == mid) {
                mid = EMPTY_ID;
            }
        }
    } else if (activity.explorer_subtype() == Trading) {
        for (ItemId &oid : gw()->game()->trade_offer()) {
            if (id == oid) {
                oid = EMPTY_ID;
            }
        }
    }

    item.owning_action = NO_ACTION;
}

void CharacterActivity::give(const TimedActivity &activity, const std::vector<Item> &items) {
    for (const Item &item : items) {
        activity.owner().discover(item);
    }
}

void CharacterActivity::give_bonuses(const TimedActivity &activity) {
    if (activity.explorer_subtype() == Eating) {
        for (const ItemId id : activity.owned_item_ids) {
            Item item = gw()->game()->inventory().get_item(id);
            activity.owner().call_hooks(HookPostEat, { &activity.owner() }, BASE_HOOK_DOMAINS, { item });
        }
    }
}

void CharacterActivity::increase_threat(const TimedActivity &activity) {
    AorInt threat = 5;
    activity.owner().call_hooks(HookCalcThreatGain, { &threat });
    gw()->game()->threat() += threat;
}

void CharacterActivity::start_next_action(const TimedActivity &activity) {
    activity.owner().activities().pop_front();
    TimedActivity *next = &activity.owner().activity();
    while (next->explorer_subtype() != None) {
        if (activity.owner().can_perform_action(next->explorer_subtype())) {
            next->start();
            break;
        }

        for (ItemId owned_item_id : next->owned_item_ids) {
            gw()->game()->inventory().get_item_ref(owned_item_id).owning_action = NO_ACTION;
        }
        activity.owner().activities().pop_front();
        next = &activity.owner().activity();
    }
}

void CharacterActivity::add_bonus_items(const TimedActivity &activity, std::vector<Item> &items) {
    AorInt item_double_chance = 0;
    activity.owner().call_hooks(HookCalcItemDoubleChance, { &item_double_chance });

    if (Generators::percent_chance(item_double_chance)) {
        std::vector<Item> items_copy = items;
        std::transform(items_copy.begin(), items_copy.end(), items_copy.begin(), [=](Item item) {
            Item new_item = item;
            new_item.id = Generators::item_id();
            return new_item;
        });
        items.insert(end(items), begin(items_copy), end(items_copy));
    }
}
