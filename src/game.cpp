#include "game.h"
#include "gamewindow.h"
#include "encyclopedia.h"
#include "scanactivity.h"

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

RunningActivities &Game::running_activities() {
    return m_running_activities;
}

WorldMap &Game::map() {
    return m_map;
}

ConsumableWaste &Game::forageable_waste() {
    return m_consumable_waste;
}

MineableWaste &Game::mineable_waste() {
    return m_mineable_waste;
}

StudiedItems &Game::studied_items() {
    return m_studied_items;
}

ClampedResource &Game::lore() {
    return m_lore;
}

Settings &Game::settings() {
    return m_settings;
}

LocationId &Game::current_location_id() {
    return m_current_location_id;
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

void Game::register_activity(TimedActivity *activity) {
    m_running_activities.push_back(activity);
}

void Game::unregister_activity(TimedActivity *activity) {
    auto &acts = m_running_activities;

    auto activity_to_unregister = std::find_if(acts.begin(), acts.end(), [=](TimedActivity *a) {
        return a == activity;
    });

    if (activity_to_unregister == acts.end()) {
        bugcheck(UnregisterUnknownActivity, activity);
    }

    acts.erase(activity_to_unregister);
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
    bool unseen = std::all_of(m_history.begin(), m_history.end(), [=](ItemCode code) {
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
    return std::accumulate(m_trade_offer.begin(), m_trade_offer.end(), 0, [this](AorInt a, ItemId id) {
        return a + m_inventory.get_item(id).def()->properties[ItemLevel];
    });
}

AorInt Game::foreign_trade_level(GameId tribe_id) {
    auto &offer = m_tribes.at(tribe_id).offer;
    return std::accumulate(offer.begin(), offer.end(), 0, [](AorInt a, const Item &item) {
        return a + item.def()->properties[ItemLevel];
    });
}

ItemProperties Game::total_smithing_resources(CharacterId character_id) {
    ItemProperties resources;

    for (ItemId id : character(character_id).external_items()[Material]) {
        Item::for_each_resource_type([&](ItemProperty, ItemProperty, ItemProperty resource_prop) {
            resources.map[resource_prop] += inventory().get_item(id).def()->properties[resource_prop];
        });
    }

    return resources;
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

    if (std::any_of(m_studied_items.begin(), m_studied_items.end(), [=](auto &pair) {
        return std::find(pair.second.begin(), pair.second.end(), item_id) != pair.second.end();
    })) {
        intent |= Study;
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

        auto &activity_items = character.activity()->owned_item_ids();
        if (std::find(activity_items.begin(), activity_items.end(), item_id) != activity_items.end()) {
            intent |= character.activity()->action();
        }
    }

    return static_cast<ItemDomain>(intent);
}

void Game::start_scan() {
    ScanActivity *activity = new ScanActivity(16000);
    activity->start();
}

bool Game::can_scan() {
    auto &acts = m_running_activities;
    bool is_scanning = std::find_if(acts.begin(), acts.end(), [=](TimedActivity *activity) {
        return activity->type() == Scan && activity->isActive();
    }) != acts.end();

    return !is_scanning && m_lore.amount() >= LORE_PER_SCAN;
}

bool Game::can_travel(LocationId id) {
    return true;
}

void Game::start_travel(LocationId id) {
    m_current_location_id = id;
}

AorInt Game::forageables_left(LocationId id) {
    return LocationDefinition::get_def(id).forageables - m_consumable_waste[id];
}

AorInt Game::forageables_left() {
    return forageables_left(m_current_location_id);
}

AorInt Game::mineables_left(LocationId id) {
    return LocationDefinition::get_def(id).mineables - m_mineable_waste[id];
}

AorInt Game::mineables_left() {
    return mineables_left(m_current_location_id);
}

Character &Game::character(CharacterId id) {
    auto result = std::find_if(m_explorers.begin(), m_explorers.end(), [=](Character &c) {
        return c.id() == id;
    });

    if (result == m_explorers.end()) {
        bugcheck(CharacterByIdLookupMiss, id);
    }

    return *result;
}

CharacterActivity *Game::activity(ActivityId id) {
    auto result = std::find_if(m_explorers.begin(), m_explorers.end(), [=](Character &c) {
        return std::any_of(c.activities().begin(), c.activities().end(), [=](CharacterActivity *a) {
            return a->id() == id;
        });
    });

    if (result == end(m_explorers)) {
        bugcheck(ActivityByIdLookupMiss, id);
    }

    return *std::find_if(result->activities().begin(), result->activities().end(), [=](CharacterActivity *a) {
        return a->id() == id;
    });
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

void Game::deserialize(QIODevice *dev) {
    m_accepting_trade = IO::read_uint(dev);
    m_tribe_name = IO::read_string(dev);
    m_trade_partner = IO::read_uint(dev);
    m_game_id = IO::read_uint(dev);
    m_actions_done = IO::read_uint(dev);

    for (size_t i = 0; i < INVENTORY_SIZE; i++) {
        m_inventory.items()[i] = IO::read_item(dev);
    }

    for (AorUInt i = 0; i < MAX_EXPLORERS; i++) {
        Character *c = new Character();
        c->deserialize(dev);
        m_explorers[i] = *c;
    }

    for (AorUInt i = 0; i < TRADE_SLOTS; i++) {
        m_trade_offer[i] = IO::read_uint(dev);
        m_accepted_offer[i] = IO::read_item(dev);
    }

    AorUInt size = IO::read_uint(dev);
    for (AorUInt i = 0; i < size; i++) {
        m_history.insert(IO::read_uint(dev));
    }

    m_tribes[NO_TRIBE];
}

Game *Game::new_game() {
    Game *g = new Game;

    g->add_character(Generators::yokin_name(), { Generators::color(), Generators::color() });
    g->add_character(Generators::yokin_name(), { Generators::color() });

    g->m_tribes[NO_TRIBE];

    return g;
}
