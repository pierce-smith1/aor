#include "game.h"
#include "gamewindow.h"
#include "encyclopedia.h"

Game::Game()
    : m_game_id(Generators::game_id()), m_tribe_name(Generators::tribe_name()) { }

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

RemoteOffer &Game::accepted_offer() {
    return m_accepted_offer;
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

bool &Game::fast_actions() {
    return m_fast_actions;
}

bool Game::add_character(const QString &name, const std::multiset<Color> &heritage) {
    int max = 0;
    while (m_explorers[max].id() != NOBODY && max < MAX_EXPLORERS) {
        max++;
    }

    if (max == MAX_EXPLORERS) {
        return false;
    }

    m_explorers[max] = Character(Generators::char_id(), name, heritage);
    return true;
}

bool Game::add_item(const Item &item) {
    if (m_inventory.add_item(item)) {
        m_history.insert(item.code);
        gw()->encyclopedia()->refresh();

        return true;
    } else {
        return false;
    }
}

void Game::check_hatch() {
    for (const Item &item : inventory().items()) {
        if (item.code == Item::code_of("fennahian_egg")) {
            if (actions_done() - item.instance_properties[InstanceEggFoundActionstamp] > ACTIONS_TO_HATCH) {
                Heritage heritage;

                if (item.instance_properties[InstanceEggFoundFlavor]) {
                    heritage.insert((Color) item.instance_properties[InstanceEggFoundFlavor]);
                } else {
                    heritage = character(item.instance_properties[InstanceEggParent1]).heritage();
                    for (Color c : character(item.instance_properties[InstanceEggParent2]).heritage()) {
                        heritage.insert(c);
                    }
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

int Game::trade_level() {
    return std::accumulate(begin(m_trade_offer), end(m_trade_offer), 0, [this](int a, ItemId id) {
        return a + m_inventory.get_item(id).def()->item_level;
    });
}

int Game::foreign_trade_level(GameId tribe_id) {
    auto &offer = m_tribes.at(tribe_id).offer;
    return std::accumulate(begin(offer), end(offer), 0, [](int a, const Item &item) {
        return a + item.def()->item_level;
    });
}

ItemProperties Game::total_resources() {
    ItemProperties resources;

    for (const Item &item : m_inventory.items()) {
        Item::for_each_resource_type([&](ItemProperty, ItemProperty, ItemProperty resource_prop) {
            resources.map[resource_prop] += item.def()->properties[resource_prop];
        });
    }

    return resources;
}

Character &Game::character(CharacterId id) {
    auto result = std::find_if(begin(m_explorers), end(m_explorers), [=](Character &c) {
        return c.id() == id;
    });

    if (result == end(m_explorers)) {
        bugcheck(CharacterByIdLookupMiss, id);
    }

    return *result;
}

void Game::refresh_ui_bars(QProgressBar *activity, QProgressBar *spirit, QProgressBar *energy, CharacterId char_id) {
    Character &character = Game::character(char_id);

    activity->setMaximum(100);
    activity->setValue(character.activity().percent_complete() * 100);

    double spirit_gain = character.spirit_to_gain() * character.activity().percent_complete();
    spirit->setMaximum(character.max_spirit());
    spirit->setValue(character.spirit() + spirit_gain);

    double energy_gain = character.energy_to_gain() * character.activity().percent_complete();
    energy->setMaximum(character.max_energy());
    energy->setValue(character.energy() + energy_gain);
}

void Game::serialize(QIODevice *dev) {
    IO::write_bool(dev, m_accepting_trade);
    IO::write_string(dev, m_tribe_name);
    IO::write_long(dev, m_trade_partner);
    IO::write_long(dev, m_game_id);
    IO::write_long(dev, m_actions_done);

    for (int i = 0; i < INVENTORY_SIZE; i++) {
        IO::write_item(dev, m_inventory.items()[i]);
    }

    for (int i = 0; i < MAX_EXPLORERS; i++) {
        m_explorers[i].serialize(dev);
    }

    for (int i = 0; i < TRADE_SLOTS; i++) {
        IO::write_long(dev, m_trade_offer[i]);
        IO::write_item(dev, m_accepted_offer[i]);
    }

    IO::write_short(dev, m_history.size());
    for (ItemCode code : m_history) {
        IO::write_short(dev, code);
    }
}

Game *Game::deserialize(QIODevice *dev) {
    Game *g = new Game;

    g->m_accepting_trade = IO::read_bool(dev);
    g->m_tribe_name = IO::read_string(dev);
    g->m_trade_partner = IO::read_long(dev);
    g->m_game_id = IO::read_long(dev);
    g->m_actions_done = IO::read_long(dev);

    for (size_t i = 0; i < INVENTORY_SIZE; i++) {
        g->m_inventory.items()[i] = IO::read_item(dev);
    }

    for (int i = 0; i < MAX_EXPLORERS; i++) {
        Character *c = Character::deserialize(dev);
        g->m_explorers[i] = *c;
        delete c;
    }

    for (int i = 0; i < TRADE_SLOTS; i++) {
        g->m_trade_offer[i] = IO::read_long(dev);
        g->m_accepted_offer[i] = IO::read_item(dev);
    }

    quint16 size = IO::read_short(dev);
    for (quint16 i = 0; i < size; i++) {
        g->m_history.insert(IO::read_short(dev));
    }

    g->m_tribes[NOBODY];
    return g;
}

Game *Game::new_game() {
    Game *g = new Game;

    g->add_character(Generators::yokin_name(), { Generators::color() });
    g->add_character(Generators::yokin_name(), { Generators::color() });

    g->inventory().items()[0] = Item("globfruit");
    g->inventory().items()[1] = Item("globfruit");
    g->inventory().items()[2] = Item("pipeapple");

    g->m_tribes[NOBODY];

    return g;
}
