#include "game.h"

Game::Game()
    : m_game_id(Generators::game_id()), m_tribe_name(Generators::tribe_name())
{
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
    add_item(Item("byteberry"));
    add_item(Item("byteberry"));
    add_item(Item("byteberry"));
    add_item(Item("oolite"));
    add_item(Item("obsilicon"));
    add_item(Item("maven_mallet"));
    add_item(Item("basalt_destructor"));

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
    m_history.insert(item.code);
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

void Game::serialize(QIODevice *dev) {
    IO::write_short(dev, m_explorers.size());
    for (const auto &pair : m_explorers) {
        IO::write_short(dev, pair.first);
        pair.second.serialize(dev);
    }

    m_inventory.serialize(dev);

    // Do not serialize m_tribes, since that is populated by the network

    IO::write_short(dev, m_trade_offer.size());
    for (size_t i = 0; i < m_trade_offer.size(); i++) {
        IO::write_long(dev, m_trade_offer[i]);
    }

    IO::write_bool(dev, m_accepting_trade);

    IO::write_long(dev, m_trade_partner);

    IO::write_long(dev, m_game_id);

    IO::write_string(dev, m_tribe_name);
}

Game *Game::deserialize(QIODevice *dev) {
    Game *g = new Game;

    quint16 size = IO::read_short(dev);
    for (size_t i = 0; i < size; i++) {
        Character *c = Character::deserialize(dev);
        g->m_explorers[i] = std::forward<Character>(*c);
        delete c;
    }

    g->m_inventory = Inventory::deserialize(dev);

    size = IO::read_short(dev);
    for (size_t i = 0; i < size; i++) {
        g->m_trade_offer[i] = IO::read_long(dev);
    }

    g->m_accepting_trade = IO::read_bool(dev);

    g->m_trade_partner = IO::read_long(dev);

    g->m_game_id = IO::read_long(dev);

    g->m_tribe_name = IO::read_string(dev);

    return g;
}
