#include "game.h"
#include "gamewindow.h"
#include "encyclopedia.h"

Game::Game()
    : m_game_id(Generators::game_id()), m_tribe_name(Generators::tribe_name())
{
    add_character(Generators::yokin_name(), { Generators::color() });
    add_character(Generators::yokin_name(), { Generators::color() });

    add_item(Item("globfruit"));
    add_item(Item("globfruit"));

    m_tribes[NOBODY];
}

Characters &Game::characters() {
    return m_explorers;
}

Inventory &Game::inventory() {
    return m_inventory;
}

ForeignTribes &Game::tribes() {
    return m_tribes;
}

GameId Game::game_id() {
    return m_game_id;
}

QString &Game::tribe_name() {
    return m_tribe_name;
}

ItemHistory &Game::history() {
    return m_history;
}

Offer &Game::trade_offer() {
    return m_trade_offer;
}

bool &Game::accepting_trade() {
    return m_accepting_trade;
}

GameId &Game::trade_partner() {
    return m_trade_partner;
}

quint64 &Game::actions_done() {
    return m_actions_done;
}

bool &Game::dead() {
    return m_dead;
}

bool Game::add_character(const QString &name, const std::multiset<Color> &heritage) {
    CharacterId max_id = 0;
    while (m_explorers[max_id].id() != NOBODY && max_id < MAX_EXPLORERS) {
        max_id++;
    }

    if (max_id == MAX_EXPLORERS) {
        return false;
    }

    m_explorers[max_id] = Character(max_id, name, heritage);
    return true;
}

bool Game::add_item(const Item &item) {
    if (m_inventory.add_item(item)) {
        m_history.insert(item.code);
        if (gw()->encyclopedia()) {
            gw()->encyclopedia()->refresh();
        }
        return true;
    } else {
        return false;
    }
}

void Game::check_hatch() {
    for (const Item &item : inventory().items()) {
        if (item.code == Item::code_of("fennahian_egg")) {
            if (actions_done() - item.instance_properties[InstanceEggFoundActionstamp] > ACTIONS_TO_HATCH) {
                Heritage heritage = m_explorers.at(item.instance_properties[InstanceEggParent1]).heritage();
                for (Color c : m_explorers.at(item.instance_properties[InstanceEggParent2]).heritage()) {
                    heritage.insert(c);
                }

                if (add_character(Generators::yokin_name(), heritage)) {
                    gw()->notify(Discovery, "A new Fennahian was born!");
                }
                inventory().remove_item(item.id);

                gw()->refresh_ui();
            }
        }
    }
}

void Game::refresh_ui_bars(QProgressBar *activity, QProgressBar *morale, QProgressBar *energy, CharacterId char_id) {
    Character &character = m_explorers.at(char_id);

    activity->setMaximum(100);
    activity->setValue(character.activity().percent_complete() * 100);

    double morale_gain = character.morale_to_gain() * character.activity().percent_complete();
    morale->setMaximum(character.max_morale());
    morale->setValue(character.morale() + morale_gain);

    double energy_gain = character.energy_to_gain() * character.activity().percent_complete();
    energy->setMaximum(character.max_energy());
    energy->setValue(character.energy() + energy_gain);
}

void Game::serialize(QIODevice *) {
}

Game *Game::deserialize(QIODevice *) {
    return nullptr;
}
