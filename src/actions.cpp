#include "actions.h"
#include "gamewindow.h"

CharacterActivity::CharacterActivity(CharacterId id, ItemDomain action, qint64 ms_total, qint64 ms_left)
    : m_action(action),
      m_ms_left(ms_total),
      m_ms_total(ms_left),
      m_char_id(id)
{
    if (m_action != None) {
        m_timer_id = gw()->startTimer(ACTIVITY_TICK_RATE_MS);
    }
}

ItemDomain &CharacterActivity::action() {
    return m_action;
}

qint64 &CharacterActivity::ms_left() {
    return m_ms_left;
}

qint64 &CharacterActivity::ms_total() {
    return m_ms_total;
}

int CharacterActivity::timer_id() {
    return m_timer_id;
}

double CharacterActivity::percent_complete() {
    if (m_action == None) {
        return 0.0;
    }

    return ((double) (m_ms_total - m_ms_left) / m_ms_total);
}

bool CharacterActivity::ongoing() {
    return m_action != None && m_ms_left > 0;
}

void CharacterActivity::progress(qint64 ms) {
    m_ms_left -= ms;

    if (m_ms_left <= 0) {
        complete();
    }
}

void CharacterActivity::complete() {
    gw()->killTimer(m_timer_id);

    m_action = None;
    m_ms_total = 0;

    switch (m_action) {
        case Smithing: { give(complete_smithing()); break; }
        case Foraging: { give(complete_foraging()); break; }
        case Mining:   { give(complete_mining()); break; }
        case Eating:   { give(complete_eating()); break; }
        case Trading:  { give(complete_trading()); break; }
        default: { qFatal("Tried to complete unknown activity %d", m_action); };
    }

    gw()->refresh_ui();
}

std::vector<Item> CharacterActivity::complete_smithing() {
    Character &character = gw()->game().characters().at(m_char_id);
    auto &materials = character.external_items().at(Material);

    ItemProperties resources;
    for (ItemId id : materials) {
        Item material = gw()->game().inventory().get_item(id);

        Item::for_each_resource_type([&](ItemProperty, ItemProperty, ItemProperty resource_prop) {
            resources.map[resource_prop] += material.def()->properties[resource_prop];
        });
    }

    qDebug("smithed with (stone: %d, metal: %d, crystal: %d, runic: %d, leafy: %d)",
        resources[StoneResource],
        resources[MetallicResource],
        resources[CrystallineResource],
        resources[RunicResource],
        resources[LeafyResource]
    );

    // Determine which crafts are possible.
    std::vector<ItemDefinition> possible_crafts;
    for (const ItemDefinition &def : ITEM_DEFINITIONS) {
        bool have_enough_resources = true;
        bool item_is_craftable = false;

        Item::for_each_resource_type([&](ItemProperty cost_prop, ItemProperty, ItemProperty resource_prop) {
            if (def.properties[cost_prop] > 0) {
                item_is_craftable = true;
            }

            if (def.properties[cost_prop] > resources[resource_prop]) {
                have_enough_resources = false;
            }
        });

        if (item_is_craftable && have_enough_resources) {
            possible_crafts.push_back(def);
        }
    }

    // Choose the highest-costing one.
    auto result = std::max_element(begin(possible_crafts), end(possible_crafts), [](const ItemDefinition &a, const ItemDefinition &b) {
        int total_cost_a = 0;
        int total_cost_b = 0;

        Item::for_each_resource_type([&](ItemProperty cost_prop, ItemProperty, ItemProperty) {
            total_cost_a += a.properties[cost_prop];
            total_cost_b += b.properties[cost_prop];
        });

        return total_cost_a < total_cost_b;
    });

    if (result == end(possible_crafts)) {
        gw()->notify(Warning, QString("%1 whiffed crafting...").arg(gw()->game().characters().at(m_char_id).name()));
        return {};
    } else {
        return { Item(result) };
    }
}

void CharacterActivity::complete_foraging() {
    //Character &character = gw()->game().characters().at(m_char_id);
}

void CharacterActivity::complete_mining() {
    //Character &character = gw()->game().characters().at(m_char_id);
}

void CharacterActivity::complete_eating() {
    //Character &character = gw()->game().characters().at(m_char_id);
}

void CharacterActivity::complete_trading() {
    //Character &character = gw()->game().characters().at(m_char_id);
}
