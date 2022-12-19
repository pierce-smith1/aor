#include "character.h"
#include "game.h"
#include "gamewindow.h"
#include "die.h"

Character::Character()
    : m_id(NOBODY),
      m_name("Nobody"),
      m_activity(NOBODY, None) { }

Character::Character(CharacterId id, const QString &name, const Heritage &heritage)
    : m_id(id),
      m_name(name),
      m_activity(m_id, None)
{
    m_heritage = heritage;
}

QString &Character::name() {
    return m_name;
}

Heritage &Character::heritage() {
    return m_heritage;
}

CharacterActivity &Character::activity() {
    return m_activity;
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

void Character::start_activity(ItemDomain domain) {
    if (domain == None) {
        m_activity = CharacterActivity(m_id, None);
        return;
    }

    qint64 activity_ms;
    if (gw()->game().fast_actions()) {
        activity_ms = 10 * 120;
    } else {
        activity_ms = 1000 * 120;
    }

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

    m_activity = CharacterActivity(m_id, domain, activity_ms, activity_ms);
    gw()->refresh_ui_buttons();
    gw()->refresh_slots();
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

quint16 &Character::morale() {
    if (m_morale > max_morale()) {
        m_morale = max_morale();
    }

    if (m_dead) {
        m_morale = 0;
    }

    return m_morale;
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

int Character::max_morale() {
    int morale = BASE_MAX_MORALE;

    const auto &artifacts = external_items().at(Artifact);
    int artifact_boost = std::accumulate(begin(artifacts), end(artifacts), 0, [this](int a, ItemId b) {
        return a + gw()->game().inventory().get_item(b).def()->properties[PersistentMaxMoraleBoost];
    });
    morale += artifact_boost;

    int heritage_boost = heritage_properties()[HeritageMaxMoraleBoost];
    morale += heritage_boost;

    return morale;
}

int Character::base_morale_cost() {
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

void Character::add_morale(int add) {
    if (-add > m_morale) {
        m_morale = 0;
        return;
    }

    m_morale += add;
    if (m_morale > max_morale()) {
        m_morale = max_morale();
    }
}

quint16 Character::egg_find_percent_chance() {
    return 5;
}

bool Character::can_perform_action(ItemDomain domain) {
    if (m_dead) {
        return false;
    }

    switch (domain) {
        case Eating: {
            return !m_activity.ongoing();
        }
        case Smithing: {
            return smithing_result() != 0 && !m_activity.ongoing();
        }
        case Foraging:
        case Mining: {
            Item tool = gw()->game().inventory().get_item(tool_id(domain));
            return energy() >= tool.def()->properties[ToolEnergyCost] && !m_activity.ongoing();
        }
        default: {
            bugcheck(AssessmentForUnknownDomain, m_id, domain);
            return false;
        }
    }
}

int Character::energy_to_gain() {
    int gain;

    switch (m_activity.action()) {
        case Eating: {
            std::vector<Item> inputs = gw()->game().inventory().items_of_intent(m_id, Eating);
            gain = std::accumulate(begin(inputs), end(inputs), 0, [](int a, const Item &b) {
                return a + b.def()->properties[ConsumableEnergyBoost];
            });
            gain += heritage_properties()[HeritageConsumableEnergyBoost];
            break;
        }
        case Smithing:
        case Foraging:
        case Mining: {
            Item tool = gw()->game().inventory().get_item(tool_id());
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

int Character::morale_to_gain() {
    int gain;

    switch (m_activity.action()) {
        case Eating: {
            std::vector<Item> inputs = gw()->game().inventory().items_of_intent(m_id, Eating);
            gain = std::accumulate(begin(inputs), end(inputs), 0, [](int a, const Item &b) {
                return a + b.def()->properties[ConsumableMoraleBoost];
            });
            break;
        }
        case Defiling: {
            std::vector<Item> inputs = gw()->game().inventory().items_of_intent(m_id, Defiling);
            gain = std::accumulate(begin(inputs), end(inputs), 0, [](int a, const Item &b) {
                return a + b.def()->item_level * 25;
            });
            break;
        }
        default: {
            gain = 0;
            break;
        }
    }

    if (m_activity.action() != Eating && m_activity.action() != Defiling) {
        gain -= base_morale_cost();
    }

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

    int current_effects = std::count_if(begin(m_effects), end(m_effects), [](const Item &item) {
        return item.id != EMPTY_ID;
    });

    if (current_effects == EFFECT_SLOTS - 1) {
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

ItemId Character::tool_id() {
    return m_tool_ids[m_activity.action()];
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
    IO::write_short(dev, m_morale);

    IO::write_short(dev, m_heritage.size());
    for (Color c : m_heritage) {
        IO::write_short(dev, c);
    }

    IO::write_long(dev, m_activity.m_ms_left);
    IO::write_long(dev, m_activity.m_ms_total);
    IO::write_short(dev, m_activity.m_action);

    for (int i = 0; i < MAX_ARRAY_SIZE; i++) {
        IO::write_long(dev, m_external_item_ids.at(Material)[i]);
        IO::write_long(dev, m_external_item_ids.at(Artifact)[i]);
    }

    for (const Item &effect : m_effects) {
        IO::write_item(dev, effect);
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
    c->m_morale = IO::read_short(dev);

    quint16 size = IO::read_short(dev);
    for (quint16 i = 0; i < size; i++) {
        c->m_heritage.insert((Color) IO::read_short(dev));
    }

    quint64 ms_left = IO::read_long(dev);
    quint64 ms_total = IO::read_long(dev);
    ItemDomain action = (ItemDomain) IO::read_short(dev);
    c->m_activity = CharacterActivity(c->m_id, action, ms_left, ms_total);

    for (int i = 0; i < MAX_ARRAY_SIZE; i++) {
        c->m_external_item_ids[Material][i] = IO::read_long(dev);
        c->m_external_item_ids[Artifact][i] = IO::read_long(dev);
    }

    for (int i = 0; i < EFFECT_SLOTS; i++) {
        c->m_effects[i] = IO::read_item(dev);
    }

    c->m_tool_ids[SmithingTool] = IO::read_long(dev);
    c->m_tool_ids[ForagingTool] = IO::read_long(dev);
    c->m_tool_ids[MiningTool] = IO::read_long(dev);

    return c;
}
