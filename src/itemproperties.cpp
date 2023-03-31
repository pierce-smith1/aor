#include "itemproperties.h"
#include "items.h"
#include "character.h"
#include "choicedialog.h"

#pragma GCC diagnostic ignored "-Wunused-parameter"
#pragma GCC diagnostic ignored "-Wunused-variable"

#define HOOK_0 [](const HookPayload &, AorUInt prop_value) {}

#define HOOK_1(t1, n1) [](const HookPayload &payload, AorUInt prop_value, AorUInt item_domain, Item caller) { \
    t1 * n1 = extract_payload<t1 *>(payload, 0);

#define HOOK_2(t1, n1, t2, n2) [](const HookPayload &payload, AorUInt prop_value, AorUInt item_domain, Item caller) { \
    t1 * n1 = extract_payload<t1 *>(payload, 0); \
    t2 * n2 = extract_payload<t2 *>(payload, 1);

#define HOOK_3(t1, n1, t2, n2, t3, n3) [](const HookPayload &payload, AorUInt prop_value, AorUInt item_domain, Item caller) { \
    t1 * n1 = extract_payload<t1 *>(payload, 0); \
    t2 * n2 = extract_payload<t2 *>(payload, 1); \
    t3 * n3 = extract_payload<t3 *>(payload, 2);

#define HOOK_4(t1, n1, t2, n2, t3, n3, t4, n4) [](const HookPayload &payload, AorUInt prop_value, AorUInt item_domain, Item caller) { \
    t1 * n1 = extract_payload<t1 *>(payload, 0); \
    t2 * n2 = extract_payload<t2 *>(payload, 1); \
    t3 * n3 = extract_payload<t3 *>(payload, 2); \
    t4 * n4 = extract_payload<t4 *>(payload, 3);

void geometric_shift(AorInt *value, AorInt extra_noise) {
    AorUInt seed = gw()->game()->game_id() * gw()->game()->game_id() + extra_noise;
    bool decrease = seed & 1;
    for (AorUInt i = 4; i < 64; i += 4) {
        if (((seed & (0xf << (i - 4))) >> (i - 4)) > 4) {
            *value += decrease ? -1 : 1;
        } else {
            break;
        }
    }
}

const std::map<ItemProperty, PropertyDefinition> &property_definitions() {
    const static std::map<ItemProperty, PropertyDefinition> PROPERTY_DESCRIPTIONS = {
        { ItemLevel, {
            "", // ItemLevel has specific code to handle its unique display.
            {{ HookCalcSpiritGain, HOOK_1(AorInt, spirit_gain)
                if (item_domain & Material) {
                    *spirit_gain += prop_value * 25;
                }
            }}, { HookCalcActivityTime, HOOK_1(AorInt, activity_ms)
                if (item_domain & Tool) {
                    *activity_ms *= prop_value == 0 ? 1 : prop_value;
                }
            }}}
        }},
        { ToolEnergyCost, {
            "Requires <b>%1 energy</b> per use.",
            {{ HookCanDoActionCheck, HOOK_2(bool, can_do, ClampedResource, current_energy)
                *can_do = *can_do && (current_energy->amount() >= static_cast<AorInt>(prop_value));
            }}, { HookCalcEnergyGain, HOOK_1(AorInt, energy_gain)
                *energy_gain -= prop_value;
            }}}
        }},
        { ConsumableEnergyBoost, {
            "Gives <b>+%1 energy</b>.",
            {{ HookCalcEnergyGain, HOOK_1(AorInt, energy_gain)
                *energy_gain += prop_value;
            }}}
        }},
        { ConsumableSpiritBoost, {
            "Gives <b>+%1 spirit</b>.",
            {{ HookCalcSpiritGain, HOOK_1(AorInt, spirit_gain)
                *spirit_gain += prop_value;
            }}}
        }},
        { ConsumableClearsNumEffects, {
            "Fully clears up to <b>%1 injury(ies)</b>, starting with the rightmost.",
            {{ HookPostEat, HOOK_1(Character, character)
                for (AorUInt i = 0; i < prop_value; i++) {
                    character->clear_last_effect();
                }
            }}}
        }},
        { ConsumableMakesCouplable, {
            "Gives the ability to <b><font color=purple>have a child</font></b> with another explorer.",
            {{ HookPostEat, HOOK_1(Character, character)
                character->can_couple() = true;
            }}}
        }},
        { ConsumableRegeneratesLocation, {
            "Regenerates <b>%1 forageables</b> and <b>the signature item(s)</b> wherever it is eaten.",
            {{ HookPostEat, HOOK_1(Character, character)
                LocationId here_id = gw()->game()->current_location_id();
                LocationDefinition here = LocationDefinition::get_def(here_id);

                gw()->game()->forageable_waste()[here_id] -= prop_value;

                AorUInt waste_left = gw()->game()->forageables_left() + gw()->game()->mineables_left();
                AorUInt current_actions = gw()->game()->waste_action_counts()[here_id];
                for (AorUInt i = 0; i < 9; i++) {
                    if (here.properties[(ItemProperty) (LocationSignatureItem1 + i)] == 0) {
                        continue;
                    }

                    gw()->game()->signature_requirements()[here_id][i] = current_actions + Generators::uint() % waste_left;
                }
            }}}
        }},
        { ConsumableKills, {
            "<b>Lethal when consumed.</b>",
            {{ HookPostEat, HOOK_1(Character, character)
                bool should_die = true;
                character->call_hooks(HookCalcShouldDie, { &should_die });

                if (should_die) {
                    character->die();
                }
            }}}
        }},
        { ConsumableGeneratesRandomItems, {
            "Creates <b>%1 random item(s)</b> when consumed.",
            {{ HookPostEat, HOOK_1(Character, character)
                std::vector<ItemDefinition> possible_items;

                auto &defs = ITEM_DEFINITIONS;
                auto inserter = std::back_inserter(possible_items);
                std::copy_if(defs.begin(), defs.end(), inserter, [=](const ItemDefinition &def) {
                    bool type_ok = (def.code & CT_CONSUMABLE) || (def.code & CT_MATERIAL) || (def.code & CT_TOOL) || (def.code & CT_ARTIFACT);
                    bool can_generate = def.properties[ConsumableGeneratesRandomItems];

                    return type_ok && !can_generate;
                });

                for (AorUInt i = 0; i < prop_value; i++) {
                    std::shuffle(possible_items.begin(), possible_items.end(), *Generators::rng());
                    character->discover(Item(possible_items[0]));
                }
            }}}
        }},
        { ConsumableCopiesRandomItems, {
            "Creates a copy of <b>%1 random item(s)</b> in the inventory when consumed.",
            {{ HookPostEat, HOOK_1(Character, character)
                std::vector<Item> inventory_items;

                auto &inventory = gw()->game()->inventory().items();
                auto inserter = std::back_inserter(inventory_items);
                std::copy_if(inventory.begin(), inventory.end(), inserter, [=](Item &item) {
                    return item.id != EMPTY_ID;
                });

                std::shuffle(inventory_items.begin(), inventory_items.end(), *Generators::rng());
                character->discover(inventory_items[0]);
            }}}
        }},
        { PersistentMaxEnergyBoost, {
            "I have <b>+%1 max energy</b>.",
            {{ HookCalcMaxEnergy, HOOK_1(AorInt, max_energy)
                *max_energy += prop_value;
            }}}
        }},
        { PersistentMaxSpiritBoost, {
            "I have <b>+%1 max spirit</b>.",
            {{ HookCalcMaxSpirit, HOOK_1(AorInt, max_spirit)
                *max_spirit += prop_value;
            }}}
        }},
        { PersistentSpeedBonus, {
            "My actions complete <b>%1x faster</b>.",
            {{ HookCalcActivityTime, HOOK_1(AorInt, activity_ms)
                *activity_ms -= *activity_ms * (prop_value / 100.0);
            }}}
        }},
        { PersistentSpeedPenalty, {
            "My actions complete <b>%1% slower</b>.",
            {{ HookCalcActivityTime, HOOK_1(AorInt, activity_ms)
                *activity_ms += *activity_ms * (prop_value / 100.0);
            }}}
        }},
        { PersistentEnergyPenalty, {
            "My actions cost an additional <b>%1 energy</b>." ,
            {{ HookCalcEnergyGain, HOOK_1(AorInt, energy_gain)
                *energy_gain -= prop_value;
            }}}
        }},
        { PersistentSpiritPenalty, {
            "My actions cost an additional <b>%1 spirit</b>." ,
            {{ HookCalcSpiritGain, HOOK_1(AorInt, spirit_gain)
                *spirit_gain -= prop_value;
            }}}
        }},
        { PersistentRandomConsumableProducts, {
            "Whenever I eat a consumable, it is <b>replaced with a random item of the same level.</b>",
            {{ HookDecideProducts, HOOK_2(std::vector<WeightedVector<Item>>, discoverables, Character, character)
                if (character->activity().explorer_subtype() != Eating) {
                    return;
                }

                for (const Item &consumable : character->activity().owned_items()) {
                    AorUInt level = consumable.def()->properties[ItemLevel];

                    if (level == 0) {
                        return;
                    }

                    std::vector<ItemDefinition> defs_of_level;
                    auto defs_inserter = std::inserter(defs_of_level, defs_of_level.end());
                    std::copy_if(ITEM_DEFINITIONS.begin(), ITEM_DEFINITIONS.end(), defs_inserter, [&](const ItemDefinition &def) {
                        return def.properties[ItemLevel] == level && (def.type & (Consumable | Material | Tool | Artifact));
                    });

                    std::vector<Item> items_of_level;
                    auto items_inserter = std::inserter(items_of_level, items_of_level.end());
                    std::transform(defs_of_level.begin(), defs_of_level.end(), items_inserter, [&](const ItemDefinition &def) {
                        return Item(def);
                    });

                    discoverables->push_back(Generators::with_trivial_weights(items_of_level));
                }
            }}}
        }},
        { PersistentDiscoveryNotRandom, {
            "<b>I decide what items I recieve from my actions</b>.",
            {{ HookPostDecideProducts, HOOK_2(std::vector<WeightedVector<Item>>, discoverables, Character, character)
                for (WeightedVector<Item> &choices : *discoverables) {
                    if (choices.size() < 2) {
                        continue;
                    }

                    ChoiceDialog choice_dialog(choices);

                    auto choice = choices[choice_dialog.exec()];
                    choices.clear();
                    choices.push_back(choice);
                }
            }}}
        }},
        { PersistentForageBonusChance, {
            "I have a <b>%1% chance</b> of discovering an additional item when foraging.",
            {{ HookDecideProducts, HOOK_2(std::vector<WeightedVector<Item>>, discoverables, Character, character)
                if (character->activity().explorer_subtype() == Foraging && Generators::percent_chance(prop_value)) {
                    CharacterActivity::discover_waste_item(*character, Foraging, *discoverables);
                }
            }}}
        }},
        { PersistentThreatDecrease, {
            "My actions generate <b>%1 less threat</b> (but not less than 1).",
            {{ HookCalcThreatGain, HOOK_1(AorInt, threat)
                *threat -= prop_value;
                if (*threat < 1) {
                    *threat = 1;
                }
            }}}
        }},
        { PersistentCannotDie, {
            "<b>I cannot die.</b>",
            {{ HookCalcShouldDie, HOOK_1(bool, should_die)
                *should_die = false;
            }}}
        }},
        { PersistentChaoticCalculations, {
            "Game calculations are <b>slightly wrong</b>.",
            {{ HookCalcMaxEnergy, HOOK_1(AorInt, energy_gain)
                geometric_shift(energy_gain, 0x197420);
            }}, { HookCalcMaxSpirit, HOOK_1(AorInt, spirit_gain)
                geometric_shift(spirit_gain, 0x22705);
            }}, { HookCalcEnergyGain, HOOK_1(AorInt, energy_gain)
                geometric_shift(energy_gain, 0x265404);
            }}, { HookCalcSpiritGain, HOOK_1(AorInt, spirit_gain)
                geometric_shift(spirit_gain, 0x343);
            }}, { HookCalcBonusConsumableEnergy, HOOK_1(AorInt, energy_gain)
                geometric_shift(energy_gain, 0x1);
            }}}
        }},
        { PersistentEggsHaveColor, {
            "Eggs hatch into <b>%1 Fennahians</b> (regardless of their parents.)",
            {{ HookJustHatched, HOOK_2(Character, character, Item, egg)
                character->heritage() = { (Color) prop_value };
            }}}
        }},
        { PersistentLoreMultiplier, {
            "We gain <b>%1x more lore</b> from studying items.",
            {{ HookCalcLoreGain, HOOK_1(AorInt, lore_gain)
                *lore_gain *= prop_value;
            }}}
        }},
        { PersistentEggPowerBoost, {
            "My children hatch with <b>%1x more powerful color traits.</b>",
            {{ HookJustHatched, HOOK_2(Character, child, Item, egg)
                auto &characters = gw()->game()->characters();
                auto mother = std::find_if(characters.begin(), characters.end(), [&](Character &c) {
                    auto &artifacts = c.external_items()[Artifact];
                    return std::find_if(artifacts.begin(), artifacts.end(), [&](ItemId id) {
                        return id == caller.id;
                    }) != artifacts.end();
                });

                if (mother == characters.end()) {
                    return;
                }

                if (egg->instance_properties[InstanceEggParent1] == mother->id() || egg->instance_properties[InstanceEggParent2] == mother->id()) {
                    Heritage heritage = child->heritage();
                    for (AorUInt i = 0; i < (prop_value - 1); i++) {
                        std::copy(heritage.begin(), heritage.end(), std::inserter(child->heritage(), child->heritage().end()));
                    }
                }
            }}}
        }},
        { PersistentInjuryPercentChance, {
            "There is a <b>+%1% chance</b> I will <b>suffer an injury</b> after taking an action.",
            {{ HookCalcInjuryChance, HOOK_1(AorInt, injury_percent_chance)
                *injury_percent_chance += prop_value;
            }}}
        }},
        { PersistentDeathGivesLore, {
            "Whenever an explorer dies, gain <b>+%1 lore.</b>",
            {{ HookPostDeath, HOOK_1(Character, character)
                gw()->game()->lore() += prop_value;
            }}}
        }},
        { HeritageMaxEnergyBoost, {
            "I have <b>+%1 max energy</b>.",
            {{ HookCalcMaxEnergy, HOOK_1(AorInt, energy_gain)
                *energy_gain += prop_value;
            }}}
        }},
        { HeritageMaxSpiritBoost, {
            "I have <b>+%1 max spirit</b>.",
            {{ HookCalcMaxSpirit, HOOK_1(AorInt, energy_gain)
                *energy_gain += prop_value;
            }}}
        }},
        { HeritageConsumableEnergyBoost, {
            "I get <b>+%1 bonus energy</b> when I eat something.",
            {{ HookCalcBonusConsumableEnergy, HOOK_1(AorInt, energy_gain)
                *energy_gain += prop_value;
            }}}
        }},
        { HeritageSmithProductUsageBoost, {
            "Items that I craft have <b>+%1 use(s)</b>.",
            {{ HookCalcBonusProductUse, HOOK_1(AorInt, use_bonus)
                *use_bonus += prop_value;
            }}}
        }},
        { HeritageInjuryResilience, {
            "I have a <b>-%1% chance to suffer an injury</b> after taking an action.",
            {{ HookCalcInjuryChance, HOOK_1(AorInt, injury_percent_chance)
                *injury_percent_chance -= prop_value;
            }}}
        }},
        { HeritageMaterialValueBonus, {
            "Materials are <b>worth %1% more</b> when I use them.",
            {{ HookCalcMaterialBonus, HOOK_1(qreal, material_bonus)
                *material_bonus += prop_value / 100.0;
            }}}
        }},
        { HeritageActivitySpeedBonus, {
            "My actions take <b>%1% less time</b>.",
            {{ HookCalcActivityTime, HOOK_1(AorInt, activity_ms)
                *activity_ms -= *activity_ms * (prop_value / 100.0);
            }}}
        }},
        { HeritageItemDoubleChance, {
            "I have a <b>%1% chance</b> to <b>double</b> items recieved from actions.",
            {{ HookCalcItemDoubleChance, HOOK_1(AorInt, item_double_chance)
                *item_double_chance += prop_value;
            }}}
        }},
        { HeritageSpiritRetention, {
            "I lose <b>%1 less spirit</b> from actions (but not less than 1).",
            {{ HookCalcSpiritGain, HOOK_1(AorInt, spirit_gain)
                if (*spirit_gain < 0) {
                    *spirit_gain += prop_value;
                    if (*spirit_gain > -1) {
                        *spirit_gain = -1;
                    }
                }
            }}}
        }},
        { ConsumableGivesEffect, {
            "Gives an injury: <b>%1.</b>",
            {{ HookPostEat, HOOK_1(Character, character)
                character->push_effect(Item(prop_value));
            }}}
        }},
        { ConsumableGivesEffectToAll, {
            "Gives an injury to <b>all</b> explorers: <b>%1.</b>",
            {{ HookPostEat, HOOK_1(Character, character)
                for (Character &c : gw()->game()->characters()) {
                    if (c.id() == NOBODY || c.dead()) {
                        continue;
                    }

                    c.push_effect(Item(prop_value));
                }
            }}}
        }},
        { SkillClearInjury, {
            "Fully clears up to <b>%1 injury(ies)</b>, starting with the rightmost.",
            {}
        }},
        { LocationSpiritCost, {
            "Passage here requires <b>%1 spirit</b> per explorer.",
            {{ HookCalcSpiritGain, HOOK_1(AorInt, spirit_gain)
                *spirit_gain -= prop_value;
            }}, { HookDecideCanTravel, HOOK_2(Character, character, bool, can_travel)
                *can_travel = *can_travel && (AorUInt) character->spirit().amount() >= prop_value;
            }}}
        }},
        { LocationEnergyCost, {
            "Passage here requires <b>%1 energy</b> per explorer.",
            {{ HookCalcEnergyGain, HOOK_1(AorInt, energy_gain)
                *energy_gain -= prop_value;
            }}, { HookDecideCanTravel, HOOK_2(Character, character, bool, can_travel)
                *can_travel = *can_travel && (AorUInt) character->energy().amount() >= prop_value;
            }}}
        }},
        { LocationPartyRequirement, {
            "Passage here requires our expedition have at least <b>%1 live explorers</b>.",
            {{ HookDecideCanTravel, HOOK_2(Character, character, bool, can_travel)
                auto &explorers = gw()->game()->characters();
                AorUInt live_explorers = std::count_if(explorers.begin(), explorers.end(), [=](Character &c) {
                    return c.id() != NOBODY && !c.dead();
                });
                *can_travel = *can_travel && live_explorers >= prop_value;
            }}}
        }},
        { LocationResourceRequirement, {
            "Passage here requires our expedition carry at least <b>%1</b> of each resource.",
            {{ HookDecideCanTravel, HOOK_2(Character, character, bool, can_travel)
                ItemProperties total = gw()->game()->total_resources();
                *can_travel = *can_travel
                    && total[StoneResource] >= prop_value
                    && total[MetallicResource] >= prop_value
                    && total[CrystallineResource] >= prop_value
                    && total[RunicResource] >= prop_value
                    && total[LeafyResource] >= prop_value;
            }}}
        }},
        { InventoryInfectsItems, {
            "After an action is taken, there is a <b>%1% chance</b> the corruption will spread.",
            {{ HookPostActivity, HOOK_1(Character, character)
                if (Generators::percent_chance(prop_value)) {
                    std::vector<Item> non_cursed_items;
                    auto &items = gw()->game()->inventory().items();
                    std::copy_if(items.begin(), items.end(), std::back_inserter(non_cursed_items), [=](const Item &item) {
                        return !(item.def()->type & Curse);
                    });

                    if (non_cursed_items.empty()) {
                        return;
                    }

                    std::shuffle(non_cursed_items.begin(), non_cursed_items.end(), *Generators::rng());

                    Item curse = Item("corrupting_nematode");
                    curse.id = non_cursed_items[0].id; // evil
                    gw()->game()->inventory().get_item_ref(curse.id) = curse;
                }
            }}}
        }},
        { InventoryMaxEnergyBoost, {
            "While this is in the inventory, all explorers have <b>+%1 max energy.</b>",
            {{ HookCalcMaxEnergy, HOOK_1(AorInt, energy_gain)
                *energy_gain += prop_value;
            }}}
        }},
        { InventoryMaxSpiritBoost, {
            "While this is in the inventory, all explorers have <b>+%1 max spirit.</b>",
            {{ HookCalcMaxSpirit, HOOK_1(AorInt, spirit_gain)
                *spirit_gain += prop_value;
            }}}
        }},
        { PropertyIfLore, {
            "If we have at <b>least %1 lore</b>, %2",
            {}
        }},
    };

    return PROPERTY_DESCRIPTIONS;
}

ItemProperties::ItemProperties(std::initializer_list<std::pair<const ItemProperty, AorUInt>> map)
    : map(map) {}

ItemProperties::ItemProperties(const std::map<ItemProperty, AorUInt> &map)
    : map(map) {}

AorUInt ItemProperties::operator[](ItemProperty prop) const {
    try {
        return map.at(prop);
    } catch (std::out_of_range &e) {
        return 0;
    }
}

std::map<ItemProperty, AorUInt>::const_iterator ItemProperties::begin() const {
    return map.begin();
}

std::map<ItemProperty, AorUInt>::const_iterator ItemProperties::end() const {
    return map.end();
}

void ItemProperties::call_hooks(HookType type, const HookPayload &payload, Item caller, AorUInt int_domain, ItemProperty allowed_prop_type) const {
    for (const auto &pair : *this) {
        if (!(pair.first & allowed_prop_type)) {
            continue;
        }

        if (pair.first == PropertyIfLore) {
            if ((AorUInt) gw()->game()->lore() >= (*this)[PropertyLoreRequirement]) {
                ItemProperties({{ (ItemProperty) pair.second, (*this)[PropertyIfLoreValue] }})
                    .call_hooks(type, payload, caller, int_domain, allowed_prop_type);
            }
            continue;
        }

        auto prop_def = property_definitions().find(pair.first);
        if (prop_def == property_definitions().end()) {
            continue;
        }

        auto hook = prop_def->second.hooks.find(type);
        if (hook == prop_def->second.hooks.end()) {
            continue;
        }

        hook->second(payload, pair.second, int_domain, caller);
    }
}

void ItemProperties::serialize(QIODevice *dev) const {
    Serialize::serialize(dev, map);
}

void ItemProperties::deserialize(QIODevice *dev) {
    Serialize::deserialize(dev, map);
}
