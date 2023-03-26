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
        activity_ms = 100 * 120;
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

TimedActivity &Character::activity() {
    return gw()->game()->activity(m_activities.front());
}

ClampedResource &Character::energy() {
    return m_energy;
}

ClampedResource &Character::spirit() {
    return m_spirit;
}

AorInt Character::base_spirit_cost() {
    return 5;
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
            call_hooks(HookCanDoActionCheck, { &can_do, &m_energy }, SmithingTool);
            break;
        } case Foraging: {
            AorInt queued_forages = std::count_if(m_activities.begin(), m_activities.end(), [=](ActivityId aid) {
                return gw()->game()->activity(aid).explorer_subtype() == Foraging;
            });
            can_do = gw()->game()->forageables_left() > queued_forages;
            call_hooks(HookCanDoActionCheck, { &can_do, &m_energy }, domain);
            break;
        } case Mining: {
            AorInt queued_mines = std::count_if(m_activities.begin(), m_activities.end(), [=](ActivityId aid) {
                return gw()->game()->activity(aid).explorer_subtype() == Mining;
            });
            can_do = gw()->game()->mineables_left() > queued_mines;
            call_hooks(HookCanDoActionCheck, { &can_do, &m_energy }, domain);
            break;
        } case Travelling: {
            return true;
        } default: {
            return false;
        }
    }

    return can_do;
}

AorInt Character::energy_to_gain() {
    if (gw()->game()->no_exhaustion()) {
        return 0;
    }

    AorInt gain = 0;

    call_hooks(HookCalcEnergyGain, { &gain }, BASE_HOOK_DOMAINS | activity().explorer_subtype(), activity().owned_items());

    switch (activity().explorer_subtype()) {
        case Eating: {
            call_hooks(HookCalcBonusConsumableEnergy, { &gain });
            break;
        } case Coupling: {
            gain = -energy().max(this);
            break;
        } default: {
            break;
        }
    }

    return gain;
}

AorInt Character::spirit_to_gain() {
    if (gw()->game()->no_exhaustion()) {
        return 0;
    }

    AorInt gain = 0;

    switch (activity().explorer_subtype()) {
        case Eating:
        case Defiling:
        case Travelling: {
            call_hooks(HookCalcSpiritGain, { &gain }, BASE_HOOK_DOMAINS, activity().owned_items());
            break;
        } default: {
            gain -= base_spirit_cost();
            break;
        }
    }

    return gain;
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

std::vector<Item> Character::nonempty_injuries() {
    std::vector<Item> injuries;

    std::copy_if(m_effects.begin(), m_effects.end(), std::back_inserter(injuries), [=](const Item &effect) {
        return effect.id != EMPTY_ID;
    });

    return injuries;
}

bool Character::push_effect(const Item &effect) {
    if (effect.id == EMPTY_ID) {
        return false;
    }

    if (!std::any_of(begin(gw()->game()->history()), end(gw()->game()->history()), [=](ItemCode code) {
        return Item::def_of(code)->type & Effect;
    })) {
        gw()->tutorial(
            "<b>I just suffered an injury...</b><br>"
            "<br>"
            "<b>Injuries</b> inflict our explorers with negative effects.<br>"
            "They are encountered randomly after taking actions, and are more frequent when using higher level tools.<br>"
            "They may also be triggered by certain events, such as running out of energy or spirit.<br>"
            "They heal over time, and can be healed faster by eating <b>consumables</b>.<br>"
            "<br>"
            "Injuries should not be left to fester; once an explorer fills all her injury slots, she will <b>die</b>. Please don't let this happen."
        );
    }
    gw()->game()->history().insert(effect.code);

    AorInt current_effects = std::count_if(begin(m_effects), end(m_effects), [](const Item &item) {
        return item.id != EMPTY_ID;
    });

    if (current_effects == EFFECT_SLOTS - 1) {
        gw()->notify(Warning, QString("%1 has been lost to the world.")
            .arg(m_name)
        );
        m_dead = true;
    }

    for (AorUInt i = 0; i < EFFECT_SLOTS; i++) {
        if (m_effects[i].id == EMPTY_ID) {
            m_effects[i] = effect;
            gw()->notify(Warning, QString("%1 suffered an injury...").arg(name()));
            return true;
        }
    }

    return false;
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

    if (domain & Effect) {
        for (const Item &effect : m_effects) {
            effect.call_hooks(type, payload);
        }
    }

    if (domain & Explorer) {
        heritage_properties().call_hooks(type, payload);
    }

    if (domain & Weather) {
        for (AorUInt i = 0; i < WEATHER_EFFECTS; i++) {
            ItemCode weather = gw()->game()->current_location().properties[(ItemProperty) (WeatherEffect1 + i)];
            Item::def_of(weather)->properties.call_hooks(type, payload);
        }
    }

    if (domain & Travelling) {
        LocationDefinition::get_def(gw()->game()->next_location_id()).properties.call_hooks(type, payload);
    }

    for (const Item &item : extra_items) {
        item.call_hooks(type, payload);
    }
}

bool Character::clear_last_effect() {
    for (AorInt i = EFFECT_SLOTS - 1; i >= 0; i--) {
        if (effects()[i].id != EMPTY_ID) {
            effects()[i] = Item();
            return true;
        }
    }

    return false;
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

Effects &Character::effects() {
    return m_effects;
}

Skills &Character::skills() {
    return m_skills;
}

void Character::serialize(QIODevice *dev) const {
    Serialize::serialize(dev, m_id);
    Serialize::serialize(dev, m_name);
    Serialize::serialize(dev, m_heritage);
    Serialize::serialize(dev, m_activities);
    Serialize::serialize(dev, m_external_item_ids);
    Serialize::serialize(dev, m_effects);
    Serialize::serialize(dev, m_tool_ids);
    Serialize::serialize(dev, m_skills);
    Serialize::serialize(dev, m_partner);
    Serialize::serialize(dev, m_dead);
    Serialize::serialize(dev, m_can_couple);
    Serialize::serialize(dev, m_energy);
    Serialize::serialize(dev, m_spirit);
}

void Character::deserialize(QIODevice *) {

}

Inventory &Character::inventory() {
    return gw()->game()->inventory();
}
