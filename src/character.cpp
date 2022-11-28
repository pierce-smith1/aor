#include "character.h"

Character::Character(CharacterId id, const QString &name)
    : m_name(name), m_color(Generators::color()), m_id(id) { }

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

bool Character::activity_ongoing() {
    return activity().action != None;
}

double Character::activity_percent_complete() {
    if (activity().action == None) {
        return 0.0;
    }

    return ((double) (activity().ms_total - activity().ms_left) / activity().ms_total);
}

std::uint16_t &Character::energy() {
    return m_energy;
}

std::uint16_t &Character::morale() {
    return m_morale;
}

int Character::max_energy() {
    return 50;
}

int Character::max_morale() {
    return 50;
}

void Character::add_energy(int add) {
    if (add > m_energy) {
        m_energy = 0;
        return;
    }

    m_energy += add;
    if (m_energy > max_energy()) {
        m_energy = max_energy();
    }
}

void Character::add_morale(int add) {
    if (add > m_morale) {
        m_morale = 0;
        return;
    }

    m_morale += add;
    if (m_morale > max_morale()) {
        m_morale = max_morale();
    }
}

int Character::base_morale_cost() {
    return 5;
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

