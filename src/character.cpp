// Copyright 2023 Pierce Smith
// This software is licensed under the terms of the Lesser GNU General Public License.

#include "character.h"
#include "game.h"
#include "gamewindow.h"
#include "die.h"

Character::Character()
    : m_id(NOBODY),
      m_name("Nobody") {}

Character::Character(CharacterId id, const QString &name, const Heritage &heritage)
    : m_id(id),
      m_name(name),
      m_heritage(heritage) {}

QString &Character::name() {
    return m_name;
}

Heritage &Character::heritage() {
    return m_heritage;
}

Activities &Character::activities() {
    return m_activities;
}

CharacterId Character::id() {
    return m_id;
}

CharacterId &Character::partner() {
    return m_partner;
}

bool &Character::dead() {
    return m_dead;
}

bool &Character::can_couple() {
    return m_can_couple;
}

LocationId &Character::location_id() {
    return m_location_id;
}

LocationId &Character::next_location_id() {
    return m_next_location_id;
}

ItemProperties Character::heritage_properties() {
    return Colors::blend_heritage(m_heritage);
}

void Character::queue_activity(ItemDomain domain, const std::vector<ItemId> &items) {
    if (domain == None || m_id == NOBODY || m_dead) {
        return;
    }

    AorInt activity_ms;
    if (gw()->game()->fast_actions()) {
        activity_ms = 10 * 120;
    } else {
        activity_ms = (1000 / 8) * 120;
    }

    // Don't do any sort of time adjustment if we're coupling - both participants
    // should finish at the same time.
    if (domain != Coupling) {
        call_hooks(HookCalcActivityTime, { &activity_ms }, BASE_HOOK_DOMAINS | domain);
    }

    TimedActivity new_activity(activity_ms, activity_ms, (ItemDomain) (Explorer | domain), items, m_id);
    gw()->game()->register_activity(new_activity);
    m_activities.push_back(new_activity.id);
    for (ItemId id : items) {
        inventory().get_item_ref(id).owning_action = new_activity.id;
    }

    // If this was the first activity added, get it started!
    if (m_activities.size() == 1) {
        // If we're doing something else, we shouldn't be accepting trades.
        if (gw()->selected_tribe_id() != NO_TRIBE) {
            gw()->connection().agreement_changed(gw()->selected_tribe_id(), false);
            gw()->game()->accepting_trade() = false;
        }
        activity().start();
    }

    gw()->refresh_ui_buttons();
    gw()->refresh_slots();
}

void Character::queue_travel(LocationId location) {
    m_next_location_id = location;
    queue_activity(Travelling, {});
}

void Character::die() {
    call_hooks(HookPostDeath, { this });
    m_dead = true;
}

TimedActivity &Character::activity() {
    return gw()->game()->activity(m_activities.front());
}

AorUInt Character::egg_find_percent_chance() {
    return 15;
}

bool Character::can_perform_action(ItemDomain domain) {
    if (m_dead) {
        return false;
    }

    bool can_do = true;

    switch (domain) {
        case None:
        case Eating:
        case Defiling: {
            break;
        } case Smithing: {
            can_do = smithing_result() != EMPTY_CODE;
            break;
        } case Foraging: {
            can_do = gw()->game()->forageables_left(m_location_id) > gw()->game()->total_queued_character_activities(Foraging);
            call_hooks(HookCanDoActionCheck, { &can_do }, domain);
            break;
        } case Mining: {
            can_do = gw()->game()->mineables_left(m_location_id) > gw()->game()->total_queued_character_activities(Mining);
            call_hooks(HookCanDoActionCheck, { &can_do }, domain);
            break;
        } case Travelling: {
            can_do = gw()->game()->map().path_exists_between(m_location_id, m_next_location_id);
            call_hooks(HookCanDoActionCheck, { &can_do }, domain);
            break;
        } default: {
            return false;
        }
    }

    return can_do;
}

std::vector<ItemCode> Character::smithable_items() {
    qreal material_bonus = 0.0;
    call_hooks(HookCalcMaterialBonus, { &material_bonus });

    ItemDefinitionPtr smithing_def = inventory().get_item(tool_id(SmithingTool)).def();

    std::vector<ItemCode> smithable_codes;
    for (const ItemDefinition &def : ITEM_DEFINITIONS) {
        bool tool_is_sufficient = true;
        bool is_smithable = false;

        Item::for_each_resource_type([&](ItemProperty cost_prop, ItemProperty max_prop, ItemProperty) {
            // If at least one cost is non-zero, the item is smithable
            if (def.properties[cost_prop] > 0) {
                is_smithable = true;
            }

            if (smithing_def->properties[max_prop] < def.properties[cost_prop]
                && def.properties[cost_prop] > BASE_MAX_RESOURCE
            ) {
                tool_is_sufficient = false;
            }
        });

        if (!tool_is_sufficient || !is_smithable) {
            continue;
        }

        bool can_smith = true;

        Item::for_each_resource_type([&](ItemProperty cost_prop, ItemProperty, ItemProperty resource_prop) {
            AorUInt resource_budget = std::accumulate(
                begin(external_items().at(Material)),
                end(external_items().at(Material)),
                0,
                [=](AorUInt a, ItemId b) {
                    AorUInt item_resource = inventory().get_item(b).def()->properties[resource_prop];
                    item_resource += (item_resource * material_bonus);
                    return item_resource + a;
                }
            );

            if (resource_budget < def.properties[cost_prop]) {
                can_smith = false;
            }
        });

        if (can_smith) {
            smithable_codes.push_back(def.code);
        }
    }

    return smithable_codes;
}

ItemCode Character::smithing_result() {
    std::vector<ItemCode> possible_smiths = smithable_items();

    // Choose the highest-costing smith.
    auto result = std::max_element(
        begin(possible_smiths),
        end(possible_smiths),
        [](ItemCode a, ItemCode b) {
            AorInt total_cost_a = 0;
            AorInt total_cost_b = 0;

            Item::for_each_resource_type([&](ItemProperty cost_prop, ItemProperty, ItemProperty) {
                total_cost_a += Item::def_of(a)->properties[cost_prop];
                total_cost_b += Item::def_of(b)->properties[cost_prop];
            });

            return total_cost_a < total_cost_b;
        }
    );

    if (result == end(possible_smiths)) {
        return 0;
    } else {
        return *result;
    }
}

ItemProperties Character::total_material_resources() {
    ItemProperties resources;

    for (ItemId material_id : m_external_item_ids[Material]) {
        Item::for_each_resource_type([&](ItemProperty, ItemProperty, ItemProperty resource_prop) {
            resources.map[resource_prop] += inventory().get_item(material_id).def()->properties[resource_prop];
        });
    }

    return resources;
}

std::vector<Item> Character::equipped_items() {
    std::vector<Item> equipped_items;

    for (auto &pair : tools()) {
        if (pair.second != EMPTY_ID) {
            equipped_items.push_back(gw()->game()->inventory().get_item(pair.second));
        }
    }

    for (ItemId id : external_items()[Artifact]) {
        if (id != EMPTY_ID) {
            equipped_items.push_back(gw()->game()->inventory().get_item(id));
        }
    }

    return equipped_items;
}

bool Character::discover(const Item &item) {
    if (!gw()->game()->add_item(item)) {
        gw()->notify(Warning, QString("%1 discovered %3 %2, but the inventory was too full to accept it!")
            .arg(name())
            .arg(item.def()->display_name)
            .arg(item.def()->display_name.toCaseFolded().startsWith('a')
                || item.def()->display_name.toCaseFolded().startsWith('e')
                || item.def()->display_name.toCaseFolded().startsWith('i')
                || item.def()->display_name.toCaseFolded().startsWith('o')
                || item.def()->display_name.toCaseFolded().startsWith('u') ? "an" : "a"
            )
        );
        return false;
    } else {
        gw()->notify(Discovery, QString("%1 discovered %3 %2!")
            .arg(name())
            .arg(item.def()->display_name)
            .arg(item.def()->display_name.toCaseFolded().startsWith('a')
                || item.def()->display_name.toCaseFolded().startsWith('e')
                || item.def()->display_name.toCaseFolded().startsWith('i')
                || item.def()->display_name.toCaseFolded().startsWith('o')
                || item.def()->display_name.toCaseFolded().startsWith('u') ? "an" : "a"
            )
        );
        return true;
    }
}

void Character::call_hooks(HookType type, const HookPayload &payload, AorUInt AorInt_domain, const std::vector<Item> &extra_items) {
    if (m_id == NOBODY || m_dead) {
        return;
    }

    ItemDomain domain = (ItemDomain) AorInt_domain;

    if (domain & Tool) {
        inventory().get_item(m_tool_ids[(ItemDomain) (domain & Tool)]).call_hooks(type, payload);
    }

    if (domain & Artifact) {
        for (ItemId id : m_external_item_ids[Artifact]) {
            inventory().get_item(id).call_hooks(type, payload);
        }
    }

    if (domain & Explorer) {
        heritage_properties().call_hooks(type, payload, Item());
    }

    if (domain & Travelling) {
        LocationDefinition::get_def(m_next_location_id).properties.call_hooks(type, payload, Item());
    }

    for (const Item &item : extra_items) {
        item.call_hooks(type, payload);
    }

    gw()->game()->call_global_hooks(type, payload, AorInt_domain);
}

ItemId Character::tool_id(ItemDomain domain) {
    return m_tool_ids[domain];
}

ToolIds &Character::tools() {
    return m_tool_ids;
}

ExternalItemIds &Character::external_items() {
    return m_external_item_ids;
}

Skills &Character::skills() {
    return m_skills;
}

Character &Character::mock_character() {
    static Character mock;
    mock = Character();

    return mock;
}

void Character::serialize(QIODevice *dev) const {
    Serialize::serialize(dev, m_id);
    Serialize::serialize(dev, m_name);
    Serialize::serialize(dev, m_heritage);
    Serialize::serialize(dev, m_activities);
    Serialize::serialize(dev, m_external_item_ids);
    Serialize::serialize(dev, m_tool_ids);
    Serialize::serialize(dev, m_skills);
    Serialize::serialize(dev, m_partner);
    Serialize::serialize(dev, m_dead);
    Serialize::serialize(dev, m_can_couple);
    Serialize::serialize(dev, m_location_id);
    Serialize::serialize(dev, m_next_location_id);
}

void Character::deserialize(QIODevice *dev) {
    Serialize::deserialize(dev, m_id);
    Serialize::deserialize(dev, m_name);
    Serialize::deserialize(dev, m_heritage);
    Serialize::deserialize(dev, m_activities);
    Serialize::deserialize(dev, m_external_item_ids);
    Serialize::deserialize(dev, m_tool_ids);
    Serialize::deserialize(dev, m_skills);
    Serialize::deserialize(dev, m_partner);
    Serialize::deserialize(dev, m_dead);
    Serialize::deserialize(dev, m_can_couple);
    Serialize::deserialize(dev, m_location_id);
    Serialize::deserialize(dev, m_next_location_id);
}

Inventory &Character::inventory() {
    return gw()->game()->inventory();
}
