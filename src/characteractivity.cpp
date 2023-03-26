#include "characteractivity.h"
#include "slot/externalslot.h"
#include "gamewindow.h"
#include "die.h"
#include "sounds.h"

CharacterActivity::CharacterActivity()
    : TimedActivity(0, 0),
      m_id(NO_ACTION),
      m_action(None),
      m_owned_items(),
      m_char_id(NOBODY) {}

CharacterActivity::CharacterActivity(
    CharacterId id,
    ItemDomain action,
    const std::vector<ItemId> &owned_items,
    AorInt ms_total,
    AorInt ms_left
)
    : TimedActivity(ms_total, ms_left),
      m_id(Generators::activity_id()),
      m_action(action),
      m_owned_items(owned_items),
      m_char_id(id)
{
    if (m_action == None) {
        m_ms_total = 0;
    }
}

ActivityId CharacterActivity::id() {
    return m_id;
}

ItemDomain &CharacterActivity::action() {
    return m_action;
}

const std::vector<ItemId> &CharacterActivity::owned_item_ids() {
    return m_owned_items;
}

const std::vector<Item> CharacterActivity::owned_items() {
    std::vector<Item> items;
    for (ItemId id : m_owned_items) {
        items.push_back(gw()->game()->inventory().get_item(id));
    }
    return items;
}

QString CharacterActivity::domain_to_action_string(ItemDomain domain) {
    switch (domain) {
        case Eating: { return "Eating"; }
        case Smithing: { return "Smithing"; }
        case Mining: { return "Mining"; }
        case Foraging: { return "Foraging"; }
        case Trading: { return "Trading"; }
        case Defiling: { return "Defiling"; }
        case Coupling: { return "Coupling"; }
        case Travelling: { return "Traveling"; }
        default: {}
    }

    return "";
}

Character &CharacterActivity::character() {
    return gw()->game()->character(m_char_id);
}

void CharacterActivity::start() {
    TimedActivity::start();

    if (m_action != None && gw()->game()->settings().sounds_on) {
        Sounds::activity_sounds().at(m_action)->play();
    }
}

void CharacterActivity::complete() {
    if (m_action == None) {
        return;
    }

    gw()->notify(ActionComplete, QString("%1 finished %2.")
        .arg(character().name())
        .arg(domain_to_action_string(m_action).toCaseFolded())
    );

    if (m_action == Trading) {
        gw()->connection().agreement_changed(gw()->game()->trade_partner(), false);
        for (AorUInt i = 0; i < TRADE_SLOTS; i++) {
            gw()->connection().offer_changed(Item(), i);
        }
        gw()->game()->accepting_trade() = false;
        gw()->game()->trade_partner() = NO_TRIBE;
    }

    exhaust_character();
    give_bonuses();
    give_injuries();
    std::vector<Item> items = products();
    add_bonus_items(items);
    give(items);
    exhaust_reagents();
    clear_injuries();

    gw()->game()->threat() += 5;

    start_next_action();

    gw()->game()->check_hatch();

    if (m_action == Foraging) {
        gw()->game()->forageable_waste()[gw()->game()->current_location_id()]++;
    } else if (m_action == Mining) {
        gw()->game()->mineable_waste()[gw()->game()->current_location_id()]++;
    } else if (m_action == Travelling) {
        auto &characters = gw()->game()->characters();
        bool no_others_travelling = std::none_of(characters.begin(), characters.end(), [=](Character &c) {
            if (c.id() == character().id()) {
                return false;
            }

            return std::any_of(c.activities().begin(), c.activities().end(), [=](CharacterActivity *a) {
                return a->m_action == Travelling;
            });
        });

        if (no_others_travelling) {
            gw()->game()->current_location_id() = gw()->game()->next_location_id();
            gw()->game()->next_location_id() = NOWHERE;
        }
    }

    gw()->refresh_ui();
    update_ui();

    TimedActivity::complete();
}

void CharacterActivity::update_ui() {
    if (gw()->selected_char_id() == character().id()) {
        refresh_ui_bars(character());
    }
}

void CharacterActivity::refresh_ui_bars(Character &character) {
    auto clamp = [](AorInt min, AorInt value, AorInt max) -> AorInt {
        return value < min ? min : (value > max ? max : value);
    };

    QProgressBar *activity_bar = gw()->window().activity_time_bar;
    QProgressBar *spirit_bar = gw()->window().spirit_bar;
    QProgressBar *energy_bar = gw()->window().energy_bar;

    activity_bar->setMaximum(100);
    activity_bar->setValue(character.activity()->percent_complete());

    // We have to be very particular about clamping values here, since if we
    // pass a number to QProgressBar::setValue that is < minValue or > maxValue,
    // nothing happens - leading to UI inconsistencies.
    double spirit_gain = character.spirit_to_gain() * (character.activity()->percent_complete() / 100.0);
    spirit_bar->setMaximum(character.spirit().max(&character));
    spirit_bar->setValue(clamp(0, character.spirit().amount() + spirit_gain, character.spirit().max(&character)));

    double energy_gain = character.energy_to_gain() * (character.activity()->percent_complete() / 100.0);
    energy_bar->setMaximum(character.energy().max(&character));
    energy_bar->setValue(clamp(0, character.energy().amount() + energy_gain, character.energy().max(&character)));
}

std::vector<Item> CharacterActivity::products() {
    std::vector<WeightedVector<Item>> discoverable_set;

    switch (m_action) {
        case Smithing: {
            ItemCode smithing_result = character().smithing_result();

            if (smithing_result == EMPTY_CODE) {
                break;
            }

            Item result = Item(smithing_result);

            AorInt use_bonus = 0;
            character().call_hooks(HookCalcBonusProductUse, { &use_bonus });
            result.uses_left += use_bonus;

            discoverable_set.push_back({{ result, 1.0 }});
            break;
        } case Foraging:
          case Mining: {
            Item tool = gw()->game()->inventory().get_item(character().tool_id(m_action));
            const ItemProperties &tool_props = tool.def()->properties;

            if (tool.id == EMPTY_ID) {
                if (m_action == Foraging) {
                    discoverable_set.push_back({{ Item("globfruit"), 1.0 }, { Item("byteberry"), 1.0 }});
                } else if (m_action == Mining) {
                    discoverable_set.push_back({{ Item("oolite"), 1.0 }, { Item("obsilicon"), 1.0 }});
                }
                break;
            }

            WeightedVector<Item> possible_items;

            Item::for_each_tool_discover([&](ItemProperty product_prop, ItemProperty weight_prop) {
                if (tool_props[weight_prop] != 0) {
                    possible_items.push_back({ Item(tool_props[product_prop]), tool_props[weight_prop] });
                }
            });

            discoverable_set.push_back(possible_items);

            if (m_action == Foraging && Generators::percent_chance(character().egg_find_percent_chance())) {
                discoverable_set.push_back({{ Item::make_egg(), 1.0 }});
            }

            break;
        } case Coupling: {
            auto &characters = gw()->game()->characters();

            // Coupling actions always end in pairs.
            // To avoid making two eggs, the first one that finishes their
            // coupling action resets their partner's partner member.
            // (That happens later in this very function.)
            // Here, we check to see if our partner has already made an egg
            // and break if so.
            auto partner = std::find_if(begin(characters), end(characters), [&](Character &other) {
                return other.partner() == character().id();
            });

            if (partner == end(characters)) {
                break;
            }

            discoverable_set.push_back({{ Item::make_egg(m_char_id, partner->id()), 1.0 }});

            partner->partner() = NOBODY;
            character().partner() = NOBODY;

            break;
        } case Trading: {
            for (const Item &item : gw()->game()->accepted_offer()) {
                discoverable_set.push_back({{ item, 1.0 }});
            }
            break;
        } default: {
            break;
        }
    }

    character().call_hooks(HookDecideProducts, { &discoverable_set, &character() });
    character().call_hooks(HookPostDecideProducts, { &discoverable_set, &character() });

    std::vector<Item> final_items;

    for (const WeightedVector<Item> &weighted_discoverables : discoverable_set) {
        final_items.push_back(Generators::sample_with_weights<Item>(weighted_discoverables));
    }

    return final_items;
}

void CharacterActivity::exhaust_reagents() {
    if (m_action == Smithing) {
        for (ItemId id : character().external_items().at(Material)) {
            exhaust_item(id);
        }
    } else if (m_action == Trading) {
        for (ItemId &id : gw()->game()->trade_offer()) {
            gw()->game()->inventory().remove_item(id);
            id = EMPTY_ID;
        }
    } else if (m_action == Eating || m_action == Defiling) {
        for (const ItemId id : m_owned_items) {
            exhaust_item(id);
        }
    }

    exhaust_item(character().tool_id(m_action));
}

void CharacterActivity::exhaust_character() {
    Item tool = gw()->game()->inventory().get_item(character().tool_id(m_action));

    character().energy().add(character().energy_to_gain(), &character());
    character().spirit().add(character().spirit_to_gain(), &character());

    for (Item &effect : character().effects()) {
        if (effect.id == EMPTY_ID) {
            continue;
        }

        if (m_action == Eating) {
            effect.uses_left -= effect.uses_left == 1 ? 1 : 2;
        } else {
            effect.uses_left -= 1;
        }

        // We do NOT clear the effect here, since we may need it later
    }

    if (character().energy().amount() == 0) {
        character().push_effect(Item("starving"));
    }

    if (character().spirit().amount() == 0) {
        character().push_effect(Item("weakness"));
    }

}

void CharacterActivity::exhaust_item(ItemId id) {
    if (id == EMPTY_ID) {
        return;
    }

    Item &item = gw()->game()->inventory().get_item_ref(id);

    if (item.uses_left > 0) {
        item.uses_left -= 1;
        if (item.uses_left == 0 && item.code & CT_TOOL) {
            character().tools().at(m_action) = EMPTY_ID;
            gw()->game()->inventory().remove_item(id);
        } else if (item.uses_left == 0) {
            gw()->game()->inventory().remove_item(id);
        }
    }

    if (m_action == Smithing) {
        for (ItemId &mid : character().external_items().at(Material)) {
            if (id == mid) {
                mid = EMPTY_ID;
            }
        }
    } else if (m_action == Trading) {
        for (ItemId &oid : gw()->game()->trade_offer()) {
            if (id == oid) {
                oid = EMPTY_ID;
            }
        }
    }

    item.owning_action = NO_ACTION;
}

void CharacterActivity::give(const std::vector<Item> &items) {
    for (const Item &item : items) {
        character().discover(item);
    }
}

void CharacterActivity::give_bonuses() {
    if (m_action == Eating) {
        for (const ItemId id : m_owned_items) {
            Item item = gw()->game()->inventory().get_item(id);
            character().call_hooks(HookPostEat, { &character() }, BASE_HOOK_DOMAINS, { item });
        }
    }
}

void CharacterActivity::give_injuries() {
    bool welchian = false;

    AorInt injury_percent_chance = 0;
    character().call_hooks(HookCalcInjuryChance, { &injury_percent_chance }, BASE_HOOK_DOMAINS | m_action);

    if (gw()->game()->threat() > AEGIS_THREAT) {
        injury_percent_chance += ((gw()->game()->threat() - AEGIS_THREAT) / 2);
        welchian = true;
    }

    if (!Generators::percent_chance(injury_percent_chance)) {
        return;
    }

    if (welchian) {
        character().push_effect(Item("welchian_fever"));
        return;
    }

    std::vector<std::pair<ItemCode, double>> possible_weighted_injuries;
    for (const ItemDefinition &def : ITEM_DEFINITIONS) {
        if (!(def.type & Effect)) {
            continue;
        }

        switch (m_action) {
            case Smithing: { if (!def.properties[InjurySmithing]) { continue; } break; }
            case Foraging: { if (!def.properties[InjuryForaging]) { continue; } break; }
            case Mining: { if (!def.properties[InjuryMining]) { continue; } break; }
            case Eating: { if (!def.properties[InjuryEating]) { continue; } break; }
            case Defiling: { if (!def.properties[InjuryDefiling]) { continue; } break; }
            case Trading: { if (!def.properties[InjuryTrading]) { continue; } break; }
            case Coupling: { if (!def.properties[InjuryCoupling]) { continue; } break; }
            default: { bugcheck(InjuriesForUnknownDomain, m_char_id, m_action); }
        }

        possible_weighted_injuries.push_back({ def.code, 1 });
    }

    if (possible_weighted_injuries.empty()) {
        return;
    }

    Item final_effect = Item(Generators::sample_with_weights(possible_weighted_injuries));
    character().push_effect(final_effect);
}

void CharacterActivity::clear_injuries() {
    for (Item &effect : character().effects()) {
        if (effect.uses_left == 0) {
            effect = Item();
        }
    }
}

void CharacterActivity::start_next_action() {
    character().activities().pop_front();
    CharacterActivity *next = character().activities().front();
    while (next->action() != None) {
        if (character().can_perform_action(next->action())) {
            next->start();
            break;
        }
        character().activities().pop_front();
        next = character().activities().front();
    }
}

void CharacterActivity::add_bonus_items(std::vector<Item> &items) {
    AorInt item_double_chance = 0;
    character().call_hooks(HookCalcItemDoubleChance, { &item_double_chance });

    if (Generators::percent_chance(item_double_chance)) {
        std::vector<Item> items_copy = items;
        items.insert(end(items), begin(items_copy), end(items_copy));
    }
}


void CharacterActivity::serialize(QIODevice *dev) const {
    TimedActivity::serialize(dev);

    IO::write_uint(dev, m_id);
    IO::write_uint(dev, m_action);
    IO::write_uint(dev, m_char_id);

    IO::write_uint(dev, m_owned_items.size());
    for (ItemId id : m_owned_items) {
        IO::write_uint(dev, id);
    }
}

void CharacterActivity::deserialize(QIODevice *dev) {
    TimedActivity::deserialize(dev);

    m_id = IO::read_uint(dev);
    m_action = (ItemDomain) IO::read_uint(dev);
    m_char_id = IO::read_uint(dev);

    AorUInt owned_size = IO::read_uint(dev);
    for (AorUInt i = 0; i < owned_size; i++) {
        m_owned_items.push_back(IO::read_uint(dev));
    }
}

Activities::reference Activities::front() {
    if (empty()) {
        return CharacterActivity::empty_activity;
    } else {
        return std::deque<CharacterActivity *>::front();
    }
}
