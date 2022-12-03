#include "character.h"
#include "game.h"

Character::Character(CharacterId id, const QString &name, Game *game)
    : m_name(name), m_color(Generators::color()), m_id(id), m_game(game) { }

QString &Character::name() {
    return m_name;
}

QColor &Character::color() {
    return m_color;
}

CharacterActivity &Character::activity() {
    return m_activity;
}

CharacterId Character::id() {
    return m_id;
}

bool &Character::accepting_trade() {
    return m_accepting_trade;
}

bool Character::activity_ongoing() {
    return activity().action != None;
}

double Character::activity_percent_complete() {
    if (activity().action == None) {
        return 0.0;
    }

    return ((double) (activity().ms_total - activity().ms_left) / activity().ms_total);
}

std::uint64_t Character::activity_time() {
    return 50000 * (1.2 * m_game->inventory().get_item(tools().at(activity().action)).def()->item_level);
}

std::uint16_t &Character::energy() {
    if (m_energy > max_energy()) {
        m_energy = max_energy();
    }

    return m_energy;
}

std::uint16_t &Character::morale() {
    if (m_morale > max_morale()) {
        m_morale = max_morale();
    }

    return m_morale;
}

int Character::max_energy() {
    const auto &artifacts = external_items().at(Artifact);

    return BASE_MAX_ENERGY + std::accumulate(begin(artifacts), end(artifacts), 0, [this](int a, ItemId b) {
        return a + m_game->inventory().get_item(b).def()->properties[ArtifactMaxEnergyBoost];
    });
}

int Character::max_morale() {
    const auto &artifacts = external_items().at(Artifact);

    return BASE_MAX_ENERGY + std::accumulate(begin(artifacts), end(artifacts), 0, [this](int a, ItemId b) {
        return a + m_game->inventory().get_item(b).def()->properties[ArtifactMaxMoraleBoost];
    });
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
            return !activity_ongoing();
        }
        case Smithing: {
            const auto &materials = external_items()[Material];
            bool enough_materials = std::all_of(begin(materials), begin(materials) + SMITHING_SLOTS, [&](ItemId a) {
                return a != EMPTY_ID;
            });

            return enough_materials && !activity_ongoing();
        }
        case Foraging:
        case Mining: {
            Item tool = m_game->inventory().get_item(tool_id(domain));
            return energy() >= tool.def()->properties[ToolEnergyCost] && !activity_ongoing();
        }
        default: {
            qFatal("Tried to assess whether character (%s) can do invalid action domain (%d)", m_name.toStdString().c_str(), domain);
        }
    }
}

int Character::energy_to_gain() {
    int gain;

    switch (activity().action) {
        case Eating: {
            std::vector<Item> inputs = m_game->inventory().items_of_intent(m_id, Eating);
            gain = std::accumulate(begin(inputs), end(inputs), 0, [](int a, const Item &b) {
                return a + b.def()->properties[ConsumableEnergyBoost];
            });
            break;
        }
        case Smithing:
        case Foraging:
        case Mining: {
            Item tool = m_game->inventory().get_item(tool_id());
            gain = tool.def()->properties[ToolEnergyCost];
            break;
        }
        default: {
            gain = 0;
            break;
        }
    }

    if (morale() > (double) max_morale() * 0.5) {
        if (gain < 0) {
            gain /= 2;
        }
    }

    return gain;
}

int Character::morale_to_gain() {
    int gain;

    switch (activity().action) {
        case Eating: {
            std::vector<Item> inputs = m_game->inventory().items_of_intent(m_id, Eating);
            gain = std::accumulate(begin(inputs), end(inputs), 0, [](int a, const Item &b) {
                return a + b.def()->properties[ConsumableMoraleBoost];
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

std::vector<Item> Character::input_items() {
    switch (activity().action) {
        case Eating: {
            return m_game->inventory().items_of_intent(m_id, Eating);
        }
        case Smithing: {
            return m_game->inventory().items_of_intent(m_id, Material);
        }
        case Trading: {
            return m_game->inventory().items_of_intent(m_id, Offering);
        }
        default: {
            return {};
        }
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
    return m_tool_ids[m_activity.action];
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

