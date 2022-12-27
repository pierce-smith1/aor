#include "character.h"
#include "game.h"
#include "gamewindow.h"
#include "die.h"

Character::Character()
    : m_id(NOBODY),
      m_name("Nobody") { }

Character::Character(CharacterId id, const QString &name, const Heritage &heritage)
    : m_id(id),
      m_name(name)
{
    m_heritage = heritage;
}

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
    if (domain == None) {
        return;
    }

    qint64 activity_ms;
    if (gw()->game().fast_actions()) {
        activity_ms = 10 * 120;
    } else {
        activity_ms = 1000 * 120;
    }

    // Don't do any sort of time adjustment if we're coupling - both participants
    // should finish at the same time.
    if (domain != Coupling) {
        double heritage_boost = heritage_properties()[HeritageActivitySpeedBonus] / 100.0;
        double injury_penalty = std::accumulate(begin(m_effects), end(m_effects), 0, [](int a, const Item &effect) {
            return a + effect.def()->properties[PersistentSpeedPenalty];
        }) / 100.0;

        int tool_level = gw()->game().inventory().get_item(m_tool_ids[domain]).def()->item_level;

        activity_ms *= tool_level == 0 ? 1 : tool_level;
        activity_ms -= activity_ms * heritage_boost;
        activity_ms += activity_ms * injury_penalty;
    }

    CharacterActivity new_activity = CharacterActivity(m_id, domain, items, activity_ms, activity_ms);
    m_activities.push_back(new_activity);
    for (ItemId id : items) {
        gw()->game().inventory().get_item_ref(id).owning_action = new_activity.id();
    }

    // If this was the first activity added, get it started!
    if (m_activities.size() == 1) {
        // If we're doing something else, we shouldn't be accepting trades
        if (gw()->selected_tribe_id() != NOBODY) {
            gw()->connection().agreement_changed(gw()->selected_tribe_id(), false);
            gw()->game().accepting_trade() = false;
        }
        m_activities.front().start();
    }

    gw()->refresh_ui_buttons();
    gw()->refresh_slots();
}

CharacterActivity &Character::activity() {
    return m_activities.front();
}

quint16 &Character::energy() {
    if (m_energy > max_energy()) {
        m_energy = max_energy();
    }

    if (m_dead) {
        m_energy = 0;
    }

    return m_energy;
}

quint16 &Character::spirit() {
    if (m_spirit > max_spirit()) {
        m_spirit = max_spirit();
    }

    if (m_dead) {
        m_spirit = 0;
    }

    return m_spirit;
}

int Character::max_energy() {
    int energy = BASE_MAX_ENERGY;

    const auto &artifacts = external_items().at(Artifact);
    int artifact_boost = std::accumulate(begin(artifacts), end(artifacts), 0, [](int a, ItemId b) {
        return a + gw()->game().inventory().get_item(b).def()->properties[PersistentMaxEnergyBoost];
    });
    energy += artifact_boost;

    int heritage_boost = heritage_properties()[HeritageMaxEnergyBoost];
    energy += heritage_boost;

    return energy;
}

int Character::max_spirit() {
    int spirit = BASE_MAX_SPIRIT;

    const auto &artifacts = external_items().at(Artifact);
    int artifact_boost = std::accumulate(begin(artifacts), end(artifacts), 0, [this](int a, ItemId b) {
        return a + gw()->game().inventory().get_item(b).def()->properties[PersistentMaxSpiritBoost];
    });
    spirit += artifact_boost;

    int heritage_boost = heritage_properties()[HeritageMaxSpiritBoost];
    spirit += heritage_boost;

    return spirit;
}

int Character::base_spirit_cost() {
    return 5;
}

void Character::add_energy(int add) {
    if (-add > m_energy) {
        m_energy = 0;
        return;
    }

    m_energy += add;
    if (m_energy > max_energy()) {
        m_energy = max_energy();
    }
}

void Character::add_spirit(int add) {
    if (-add > m_spirit) {
        m_spirit = 0;
        return;
    }

    m_spirit += add;
    if (m_spirit > max_spirit()) {
        m_spirit = max_spirit();
    }
}

quint16 Character::egg_find_percent_chance() {
    return 15;
}

bool Character::can_perform_action(ItemDomain domain) {
    if (m_dead) {
        return false;
    }

    switch (domain) {
        case None:
        case Eating:
        case Defiling: {
            return true;
        }
        case Smithing: {
            Item tool = gw()->game().inventory().get_item(tool_id(domain));
            return smithing_result() != 0
                && energy() >= tool.def()->properties[ToolEnergyCost];
        }
        case Foraging:
        case Mining: {
            Item tool = gw()->game().inventory().get_item(tool_id(domain));
            return energy() >= tool.def()->properties[ToolEnergyCost];
        }
        default: {
            bugcheck(AssessmentForUnknownDomain, m_id, domain);
            return false;
        }
    }
}

int Character::energy_to_gain() {
    int gain;
    CharacterActivity &activity = m_activities.front();
    switch (activity.action()) {
        case Eating: {
            std::vector<ItemId> inputs = activity.owned_items();
            gain = std::accumulate(begin(inputs), end(inputs), 0, [](int a, ItemId b) {
                return a + gw()->game().inventory().get_item(b).def()->properties[ConsumableEnergyBoost];
            });
            gain += heritage_properties()[HeritageConsumableEnergyBoost];
            break;
        }
        case Smithing:
        case Foraging:
        case Mining: {
            Item tool = gw()->game().inventory().get_item(tool_id(activity.action()));
            gain = -tool.def()->properties[ToolEnergyCost];
            break;
        }
        case Coupling: {
            gain = -max_energy();
            break;
        }
        default: {
            gain = 0;
            break;
        }
    }

    gain -= std::accumulate(begin(m_effects), end(m_effects), 0, [](int a, const Item &item) {
        return a + item.def()->properties[PersistentEnergyPenalty];
    });

    return gain;
}

int Character::spirit_to_gain() {
    int gain;
    CharacterActivity &activity = m_activities.front();
    switch (activity.action()) {
        case Eating: {
            std::vector<ItemId> inputs = activity.owned_items();
            gain = std::accumulate(begin(inputs), end(inputs), 0, [](int a, ItemId b) {
                return a + gw()->game().inventory().get_item(b).def()->properties[ConsumableSpiritBoost];
            });
            break;
        }
        case Defiling: {
            std::vector<ItemId> inputs = activity.owned_items();
            gain = std::accumulate(begin(inputs), end(inputs), 0, [](int a, ItemId b) {
                return a + gw()->game().inventory().get_item(b).def()->item_level * 25;
            });
            break;
        }
        default: {
            gain = 0;
            break;
        }
    }

    if (activity.action() != Eating && activity.action() != Defiling) {
        gain -= base_spirit_cost();
    }

    gain -= std::accumulate(begin(m_effects), end(m_effects), 0, [](int a, const Item &item) {
        return a + item.def()->properties[PersistentSpiritPenalty];
    });

    return gain;
}

std::vector<ItemCode> Character::smithable_items() {
    ItemDefinitionPtr smithing_def = gw()->game().inventory().get_item(tool_id(SmithingTool)).def();
    double heritage_resource_boost = heritage_properties()[HeritageMaterialValueBonus] / 100.0;

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
            int resource_budget = std::accumulate(
                begin(external_items().at(Material)),
                end(external_items().at(Material)),
                0,
                [=](int a, ItemId b) {
                    int item_resource = gw()->game().inventory().get_item(b).def()->properties[resource_prop];
                    item_resource += (item_resource * heritage_resource_boost);
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
            int total_cost_a = 0;
            int total_cost_b = 0;

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
            resources.map[resource_prop] += gw()->game().inventory().get_item(material_id).def()->properties[resource_prop];
        });
    }

    return resources;
}

bool Character::push_effect(const Item &effect) {
    if (effect.id == EMPTY_ID) {
        return false;
    }

    if (std::all_of(begin(gw()->game().history()), end(gw()->game().history()), [=](ItemCode code) {
        return !(Item::def_of(code)->type & Effect);
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
    gw()->game().history().insert(effect.code);

    int current_effects = std::count_if(begin(m_effects), end(m_effects), [](const Item &item) {
        return item.id != EMPTY_ID;
    });

    if (current_effects == EFFECT_SLOTS - 1) {
        gw()->notify(Warning, QString("%1 has been lost to the world.")
            .arg(m_name)
        );
        m_dead = true;
    }

    for (int i = 0; i < EFFECT_SLOTS; i++) {
        if (m_effects[i].id == EMPTY_ID) {
            m_effects[i] = effect;
            return true;
        }
    }

    return false;
}

bool Character::clear_last_effect() {
    for (int i = EFFECT_SLOTS - 1; i >= 0; i--) {
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

void Character::serialize(QIODevice *dev) const {
    IO::write_short(dev, m_id);
    IO::write_string(dev, m_name);
    IO::write_short(dev, m_partner);
    IO::write_bool(dev, m_dead);
    IO::write_bool(dev, m_can_couple);
    IO::write_short(dev, m_energy);
    IO::write_short(dev, m_spirit);

    IO::write_short(dev, m_heritage.size());
    for (Color c : m_heritage) {
        IO::write_short(dev, c);
    }

    for (int i = 0; i < MAX_ARRAY_SIZE; i++) {
        IO::write_long(dev, m_external_item_ids.at(Material)[i]);
        IO::write_long(dev, m_external_item_ids.at(Artifact)[i]);
    }

    for (const Item &effect : m_effects) {
        IO::write_item(dev, effect);
    }

    IO::write_short(dev, m_activities.size());
    for (const CharacterActivity &activity : m_activities) {
        activity.serialize(dev);
    }

    IO::write_long(dev, m_tool_ids.at(SmithingTool));
    IO::write_long(dev, m_tool_ids.at(ForagingTool));
    IO::write_long(dev, m_tool_ids.at(MiningTool));
}

// Transfers ownership
Character *Character::deserialize(QIODevice *dev) {
    Character *c = new Character;

    c->m_id = IO::read_short(dev);
    c->m_name = IO::read_string(dev);
    c->m_partner = IO::read_short(dev);
    c->m_dead = IO::read_bool(dev);
    c->m_can_couple = IO::read_bool(dev);
    c->m_energy = IO::read_short(dev);
    c->m_spirit = IO::read_short(dev);

    quint16 heritage_size = IO::read_short(dev);
    for (quint16 i = 0; i < heritage_size; i++) {
        c->m_heritage.insert((Color) IO::read_short(dev));
    }

    for (int i = 0; i < MAX_ARRAY_SIZE; i++) {
        c->m_external_item_ids[Material][i] = IO::read_long(dev);
        c->m_external_item_ids[Artifact][i] = IO::read_long(dev);
    }

    for (int i = 0; i < EFFECT_SLOTS; i++) {
        c->m_effects[i] = IO::read_item(dev);
    }

    quint16 activities_size = IO::read_short(dev);
    for (quint16 i = 0; i < activities_size; i++) {
        CharacterActivity *a = CharacterActivity::deserialize(dev);
        c->m_activities.push_back(*a);
        delete a;
    }

    c->m_tool_ids[SmithingTool] = IO::read_long(dev);
    c->m_tool_ids[ForagingTool] = IO::read_long(dev);
    c->m_tool_ids[MiningTool] = IO::read_long(dev);

    return c;
}
