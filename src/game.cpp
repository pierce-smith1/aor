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

AorUInt &Game::actions_done() {
    return m_actions_done;
}

bool &Game::fast_actions() {
    return m_fast_actions;
}

bool &Game::no_exhaustion() {
    return m_no_exhaustion;
}

bool Game::add_character(const QString &name, const std::multiset<Color> &heritage) {
    AorUInt max = 0;
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
        check_tutorial((ItemDomain) item.def()->type.n);
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

void Game::check_tutorial(ItemDomain domain) {
    bool unseen = std::all_of(begin(m_history), end(m_history), [=](ItemCode code) {
        return !(Item::def_of(code)->type & domain);
    });

    if (!unseen) {
        return;
    }

    switch (domain) {
        case Consumable: {
            gw()->tutorial(
                "<b>I just found a consumable!</b><br>"
                "<br>"
                "<b>Consumables</b> are used to restore our explorers' energy and health.</b><br>"
                "When eaten, consumables will usually refill some amount of <b>energy.</b> They will also <b>reduce the timer of all active injuries by 1 action.</b><br>"
                "Many will also have a bonus effect on top of this.<br>"
                "Drag a consumable to an explorer's <b>portrait</b> to have her eat it.</b><br>"
            );
            break;
        } case Material: {
            gw()->tutorial(
                "<b>I just found a material!</b><br>"
                "<br>"
                "<b>Materials</b> are used to craft new items.</b><br>"
                "All materials have some amount of value in one of the <b>five resource types</b>.<br>"
                "Materials can be dragged AorInto an explorer's <b>smithing slots</b> to use them for crafting - the total value of all materials in her smithing slots determines what item she will make.<br>"
                "Materials can also be dragged to an explorer's <b>portrait</b> to have her <b>defile</b> it, which will destroy the item to restore <b>25 spirit per item level</b>.<br>"
                "<br>"
                "Why don't I check the <b>encyclopedia</b> and see if I can smith <b>a new tool</b>?"
            );
            break;
        }
        case SmithingTool: {
            gw()->tutorial(
                "<b>I just made a smithing tool!</b><br>"
                "<br>"
                "When equipped, <b>smithing tools</b> increase the amount of resources I can put AorInto <b>smithing slots</b>.<br>"
                "The maximum amount of resources a smithing tool can support is referred to as its <b>power</b>.<br>"
                "Regardless of tool, I can always support at least <b>10 of each resource type</b>.<br>"
                "<br>"
                "Like all tools, smithing tools can <b>only be used a limited number of times</b> before they break."
            );
            break;
        }
        case ForagingTool: {
            gw()->tutorial(
                "<b>I just made a foraging tool!</b><br>"
                "<br>"
                "When equipped, <b>foraging tools</b> allow me to find new consumables when I forage.<br>"
                "Each foraging tool has a unique pool of items it can discover; some potentially rarer than others.<br>"
                "While I hold a foraging tool, I also have a chance to find <b>loose eggs</b> that can <b>hatch AorInto new explorers!</b><br>"
                "<br>"
                "Like all tools, foraging tools can <b>only be used a limited number of times</b> before they break."
            );
            break;
        }
        case MiningTool: {
            gw()->tutorial(
                "<b>I just made a mining tool!</b><br>"
                "<br>"
                "When equipped, <b>mining tools</b> allow me to find new materials when I mine.<br>"
                "Each mining tool has a unique pool of items it can discover; some potentially rarer than others.<br>"
                "<br>"
                "Like all tools, mining tools can <b>only be used a limited number of times</b> before they break."
            );
            break;
        }
        case Artifact: {
            gw()->tutorial(
                "<b>I just found an artifact!</b><br>"
                "<br>"
                "Artifacts provide <b>constant bonuses</b> when equipped.<br>"
                "Each explorer can hold up to 3."
            );
            break;
        }
        default: {
            break;
        }
    }
}

AorInt Game::trade_level() {
    return std::accumulate(begin(m_trade_offer), end(m_trade_offer), 0, [this](AorInt a, ItemId id) {
        return a + m_inventory.get_item(id).def()->properties[ItemLevel];
    });
}

AorInt Game::foreign_trade_level(GameId tribe_id) {
    auto &offer = m_tribes.at(tribe_id).offer;
    return std::accumulate(begin(offer), end(offer), 0, [](AorInt a, const Item &item) {
        return a + item.def()->properties[ItemLevel];
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

ItemDomain Game::intent_of(ItemId item_id) {
    if (item_id == EMPTY_ID) {
        return None;
    }

    AorUInt intent = None;

    if (std::find(m_trade_offer.begin(), m_trade_offer.end(), item_id) != m_trade_offer.end()) {
        intent |= Offering;
    }

    for (Character &character : m_explorers) {
        auto &materials = character.external_items()[Material];
        if (std::find(materials.begin(), materials.end(), item_id) != materials.end()) {
            intent |= Material;
        }

        auto &artifacts = character.external_items()[Artifact];
        if (std::find(artifacts.begin(), artifacts.end(), item_id) != artifacts.end()) {
            intent |= Artifact;
        }

        auto &tools = character.tools();
        if (tools[SmithingTool] == item_id || tools[ForagingTool] == item_id || tools[MiningTool] == item_id) {
            intent |= Tool;
        }

        auto &activity_items = character.activity().owned_item_ids();
        if (std::find(activity_items.begin(), activity_items.end(), item_id) != activity_items.end()) {
            intent |= character.activity().action();
        }
    }

    return static_cast<ItemDomain>(intent);
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

CharacterActivity &Game::activity(ActivityId id) {
    auto result = std::find_if(begin(m_explorers), end(m_explorers), [=](Character &c) {
        return std::any_of(begin(c.activities()), end(c.activities()), [=](CharacterActivity &a) {
            return a.id() == id;
        });
    });

    if (result == end(m_explorers)) {
        bugcheck(ActivityByIdLookupMiss, id);
    }

    return *std::find_if(begin(result->activities()), end(result->activities()), [=](CharacterActivity &a) {
        return a.id() == id;
    });
}

void Game::refresh_ui_bars(QProgressBar *activity, QProgressBar *spirit, QProgressBar *energy, CharacterId char_id) {
    auto clamp = [](AorInt min, AorInt value, AorInt max) -> AorInt {
        return value < min ? min : (value > max ? max : value);
    };

    Character &character = Game::character(char_id);

    activity->setMaximum(100);
    activity->setValue(character.activity().percent_complete() * 100);

    // We have to be very particular about clamping values here, since if we
    // pass a number to QProgressBar::setValue that is < minValue or > maxValue,
    // nothing happens - leading to UI inconsistencies.
    double spirit_gain = character.spirit_to_gain() * character.activity().percent_complete();
    spirit->setMaximum(character.max_spirit());
    spirit->setValue(clamp(0, character.spirit() + spirit_gain, character.max_spirit()));

    double energy_gain = character.energy_to_gain() * character.activity().percent_complete();
    energy->setMaximum(character.max_energy());
    energy->setValue(clamp(0, character.energy() + energy_gain, character.max_energy()));
}

void Game::serialize(QIODevice *dev) {
    IO::write_uint(dev, m_accepting_trade);
    IO::write_string(dev, m_tribe_name);
    IO::write_uint(dev, m_trade_partner);
    IO::write_uint(dev, m_game_id);
    IO::write_uint(dev, m_actions_done);

    for (AorUInt i = 0; i < INVENTORY_SIZE; i++) {
        IO::write_item(dev, m_inventory.items()[i]);
    }

    for (AorUInt i = 0; i < MAX_EXPLORERS; i++) {
        m_explorers[i].serialize(dev);
    }

    for (AorUInt i = 0; i < TRADE_SLOTS; i++) {
        IO::write_uint(dev, m_trade_offer[i]);
        IO::write_item(dev, m_accepted_offer[i]);
    }

    IO::write_uint(dev, m_history.size());
    for (ItemCode code : m_history) {
        IO::write_uint(dev, code);
    }
}

Game *Game::deserialize(QIODevice *dev) {
    Game *g = new Game;

    g->m_accepting_trade = IO::read_uint(dev);
    g->m_tribe_name = IO::read_string(dev);
    g->m_trade_partner = IO::read_uint(dev);
    g->m_game_id = IO::read_uint(dev);
    g->m_actions_done = IO::read_uint(dev);

    for (size_t i = 0; i < INVENTORY_SIZE; i++) {
        g->m_inventory.items()[i] = IO::read_item(dev);
    }

    for (AorUInt i = 0; i < MAX_EXPLORERS; i++) {
        Character *c = Character::deserialize(dev);
        g->m_explorers[i] = *c;
        delete c;
    }

    for (AorUInt i = 0; i < TRADE_SLOTS; i++) {
        g->m_trade_offer[i] = IO::read_uint(dev);
        g->m_accepted_offer[i] = IO::read_item(dev);
    }

    AorUInt size = IO::read_uint(dev);
    for (AorUInt i = 0; i < size; i++) {
        g->m_history.insert(IO::read_uint(dev));
    }

    g->m_tribes[NO_TRIBE];
    return g;
}

Game *Game::new_game() {
    Game *g = new Game;

    g->add_character(Generators::yokin_name(), { Generators::color() });

    g->m_tribes[NO_TRIBE];

    return g;
}
