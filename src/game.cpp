#include "game.h"

Game::Game() {
    add_character(Generators::yokin_name());
    add_character(Generators::yokin_name());
    add_character(Generators::yokin_name());
    add_character(Generators::yokin_name());
    add_character(Generators::yokin_name());
    add_character(Generators::yokin_name());
    add_character(Generators::yokin_name());
    add_character(Generators::yokin_name());

    add_item(Item("globfruit"));
    add_item(Item("globfruit"));
    add_item(Item("globfruit"));
}

Characters &Game::characters() {
    return m_explorers;
}

Inventory &Game::inventory() {
    return m_inventory;
}

void Game::add_character(const QString &name) {
    CharacterId max_id;
    auto max_id_search = std::max_element(begin(m_explorers), end(m_explorers), [&](const auto &a, const auto &b) {
        return a.first < b.first;
    });

    if (max_id_search == end(m_explorers)) {
        m_explorers.emplace(0, Character(0, name));
    } else {
        max_id = max_id_search->first;
        m_explorers.emplace(max_id + 1, Character(max_id + 1, name));
    }

}

void Game::add_item(const Item &item) {
    m_inventory.add_item(item);
}

bool Game::character_can_perform_action(CharacterId char_id, ItemDomain domain) {
    Character &character = m_explorers.at(char_id);
    switch (domain) {
        case Eating: {
            return !character.activity_ongoing();
        }
        case Smithing: {
            const auto &materials = character.external_items()[Material];
            bool enough_materials = std::all_of(begin(materials), begin(materials) + SMITHING_SLOTS, [&](ItemId a) {
                return a != EMPTY_ID;
            });

            return enough_materials && !character.activity_ongoing();
        }
        case Praying: {
            const auto &offerings = character.external_items().at(Offering);
            bool enough_offerings = std::any_of(begin(offerings), begin(offerings) + PRAYER_SLOTS, [&](ItemId a) {
                return a != EMPTY_ID;
            });

            return enough_offerings && !character.activity_ongoing();
        }
        case Foraging:
        case Mining: {
            Item tool = m_inventory.get_item(character.tool_id(domain));
            return character.energy() >= tool.def()->properties[ToolEnergyCost] && !character.activity_ongoing();
        }
        default: {
            qFatal("Tried to assess whether character (%d) can do invalid action domain (%d)", char_id, domain);
        }
    }
}

int Game::energy_to_gain_from_current_activity(CharacterId char_id) {
    Character &character = m_explorers.at(char_id);
    int gain;

    switch (character.activity().action) {
        case Eating: {
            std::vector<Item> inputs = m_inventory.items_of_intent(char_id, Eating);
            gain = std::accumulate(begin(inputs), end(inputs), 0, [](int a, const Item &b) {
                return a + b.def()->properties[ConsumableEnergyBoost];
            });
            break;
        }
        case Smithing:
        case Foraging:
        case Mining: {
            Item tool = m_inventory.get_item(character.tool_id());
            gain = tool.def()->properties[ToolEnergyCost];
            break;
        }
        default: {
            gain = 0;
            break;
        }
    }

    if (character.morale() > (double) character.max_morale() * 0.5) {
        if (gain < 0) {
            gain /= 2;
        }
    }

    return gain;
}

int Game::morale_to_gain_from_current_activity(CharacterId char_id) {
    Character &character = m_explorers.at(char_id);
    int gain;

    switch (character.activity().action) {
        case Eating: {
            std::vector<Item> inputs = m_inventory.items_of_intent(char_id, Eating);
            gain = std::accumulate(begin(inputs), end(inputs), 0, [](int a, const Item &b) {
                return a + b.def()->properties[ConsumableMoraleBoost];
            });
            break;
        }
        case Praying: {
            std::vector<Item> offerings = m_inventory.items_of_intent(char_id, Offering);
            gain = std::accumulate(begin(offerings), end(offerings), 0, [](int a, const Item &b) {
                return b.def()->item_level * 10 + a;
            });
            break;
        }
        default: {
            gain = -character.base_morale_cost();
            break;
        }
    }

    return gain;
}

std::vector<Item> Game::input_items_for_current_activity(CharacterId char_id) {
    Character &character = characters().at(char_id);

    switch (character.activity().action) {
        case Eating: {
            return inventory().items_of_intent(char_id, Eating);
        }
        case Smithing: {
            return inventory().items_of_intent(char_id, Material);
        }
        case Praying: {
            return inventory().items_of_intent(char_id, Offering);
        }
        default: {
            return {};
        }
    }
}

void Game::refresh_ui_bars(QProgressBar *activity, QProgressBar *morale, QProgressBar *energy, CharacterId char_id) {
    Character &character = m_explorers.at(char_id);

    activity->setMaximum(100);
    activity->setValue(character.activity_percent_complete() * 100);

    double morale_gain = morale_to_gain_from_current_activity(char_id) * character.activity_percent_complete();
    morale->setMaximum(character.max_morale());
    morale->setValue(character.morale() + morale_gain);

    double energy_gain = energy_to_gain_from_current_activity(char_id) * character.activity_percent_complete();
    energy->setMaximum(character.max_energy());
    energy->setValue(character.energy() + energy_gain);
}
