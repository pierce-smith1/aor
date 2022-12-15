#include "character.h"
#include "game.h"
#include "gamewindow.h"

Character::Character()
    : m_id(NOBODY),
      m_name("Nobody"),
      m_activity(NOBODY, None) { }

Character::Character(CharacterId id, const QString &name)
    : m_id(id),
      m_name(name),
      m_activity(m_id, None)
{
    m_heritage.insert(Generators::color());
    m_heritage.insert(Generators::color());
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

void Character::start_activity(ItemDomain domain) {
    if (domain == None) {
        m_activity = CharacterActivity(m_id, None);
        return;
    }

    //qint64 activity_ms = 1000 * 120;
    qint64 activity_ms = 120;
    if (m_tool_ids.find(domain) != end(m_tool_ids) && m_tool_ids.at(domain) != EMPTY_ID) {
        activity_ms *= gw()->game().inventory().get_item(m_tool_ids.at(domain)).def()->item_level;
    }

    m_activity = CharacterActivity(m_id, domain, activity_ms, activity_ms);
    gw()->refresh_ui_buttons();
    gw()->refresh_slots();
}

quint16 &Character::energy() {
    if (m_energy > max_energy()) {
        m_energy = max_energy();
    }

    return m_energy;
}

quint16 &Character::morale() {
    if (m_morale > max_morale()) {
        m_morale = max_morale();
    }

    return m_morale;
}

int Character::max_energy() {
    const auto &artifacts = external_items().at(Artifact);

    return BASE_MAX_ENERGY + std::accumulate(begin(artifacts), end(artifacts), 0, [this](int a, ItemId b) {
        return a + gw()->game().inventory().get_item(b).def()->properties[ArtifactMaxEnergyBoost];
    });
}

int Character::max_morale() {
    const auto &artifacts = external_items().at(Artifact);

    return BASE_MAX_ENERGY + std::accumulate(begin(artifacts), end(artifacts), 0, [this](int a, ItemId b) {
        return a + gw()->game().inventory().get_item(b).def()->properties[ArtifactMaxMoraleBoost];
    });
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

bool Character::can_perform_action(ItemDomain domain) {
    switch (domain) {
        case Eating: {
            return !m_activity.ongoing();
        }
        case Smithing: {
            const auto &materials = external_items()[Material];
            bool enough_materials = std::any_of(begin(materials), begin(materials) + SMITHING_SLOTS, [&](ItemId a) {
                return a != EMPTY_ID;
            });

            return enough_materials && !m_activity.ongoing();
        }
        case Foraging:
        case Mining: {
            Item tool = gw()->game().inventory().get_item(tool_id(domain));
            return energy() >= tool.def()->properties[ToolEnergyCost] && !m_activity.ongoing();
        }
        default: {
            qFatal("Tried to assess whether character (%s) can do invalid action domain (%d)", m_name.toStdString().c_str(), domain);
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
            break;
        }
        case Smithing:
        case Foraging:
        case Mining: {
            Item tool = gw()->game().inventory().get_item(tool_id());
            gain = tool.def()->properties[ToolEnergyCost];
            break;
        }
        default: {
            gain = 0;
            break;
        }
    }

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

    return gain;
}

std::vector<ItemCode> Character::smithable_items() {
    ItemDefinitionPtr smithing_def = gw()->game().inventory().get_item(tool_id(SmithingTool)).def();

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
                    return gw()->game().inventory().get_item(b).def()->properties[resource_prop] + a;
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
        return CT_EMPTY;
    } else {
        return *result;
    }
}

bool Character::push_effect(const Item &effect) {
    if (effect.id == EMPTY_ID) {
        return false;
    }

    for (int i = 0; i < EFFECT_SLOTS; i++) {
        if (effects()[i].id == EMPTY_ID) {
            effects()[i] = effect;
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

void Character::serialize(QIODevice *) const {
}

// Transfers ownership
Character *Character::deserialize(QIODevice *) {
    return nullptr;
}
