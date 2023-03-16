#include "itemproperties.h"
#include "items.h"
#include "character.h"
#include "choicedialog.h"

#pragma GCC diagnostic ignored "-Wunused-parameter"
#pragma GCC diagnostic ignored "-Wunused-variable"

#define HOOK_0 [](const HookPayload &, AorUInt prop_value) {}

#define HOOK_1(t1, n1) [](const HookPayload &payload, AorUInt prop_value, AorUInt item_domain) { \
    t1 * n1 = extract_payload<t1 *>(payload, 0);

#define HOOK_2(t1, n1, t2, n2) [](const HookPayload &payload, AorUInt prop_value, AorUInt item_domain) { \
    t1 * n1 = extract_payload<t1 *>(payload, 0); \
    t2 * n2 = extract_payload<t2 *>(payload, 1);

#define HOOK_3(t1, n1, t2, n2, t3, n3) [](const HookPayload &payload, AorUInt prop_value, AorUInt item_domain) { \
    t1 * n1 = extract_payload<t1 *>(payload, 0); \
    t2 * n2 = extract_payload<t2 *>(payload, 1); \
    t3 * n3 = extract_payload<t3 *>(payload, 2);

#define HOOK_4(t1, n1, t2, n2, t3, n3, t4, n4) [](const HookPayload &payload, AorUInt prop_value, AorUInt item_domain) { \
    t1 * n1 = extract_payload<t1 *>(payload, 0); \
    t2 * n2 = extract_payload<t2 *>(payload, 1); \
    t3 * n3 = extract_payload<t3 *>(payload, 2); \
    t4 * n4 = extract_payload<t4 *>(payload, 3);

const std::map<ItemProperty, PropertyDefinition> &property_definitions() {
    const static std::map<ItemProperty, PropertyDefinition> PROPERTY_DESCRIPTIONS = {
        { ItemLevel, {
            "", // ItemLevel has specific code to handle its unique display.
            {{ HookCalcSpiritGain, HOOK_1(AorInt, spirit_gain)
                if (item_domain & Material) {
                    *spirit_gain += prop_value * 25;
                }
            }},
            { HookCalcActivityTime, HOOK_1(AorInt, activity_ms)
                if (item_domain & Tool) {
                    *activity_ms *= prop_value == 0 ? 1 : prop_value;
                }
            }},
            { HookCalcInjuryChance, HOOK_1(AorInt, injury_percent_chance)
                if (item_domain & Tool) {
                    *injury_percent_chance += 3 + (prop_value * 6);
                }
            }}}
        }},
        { ToolEnergyCost, {
            "Requires <b>%1 energy</b> per use.",
            {{ HookCanDoActionCheck, HOOK_2(bool, can_do, AorInt, current_energy)
                *can_do = *can_do && (*current_energy >= static_cast<AorInt>(prop_value));
            }},
            { HookCalcEnergyGain, HOOK_1(AorInt, energy_gain)
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
                if (character->activity().action() != Eating) {
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
        { ConsumableGivesEffect, {
            "<b>This doesn't look very good for me...</b>",
            {{ HookPostEat, HOOK_1(Character, character)
                character->push_effect(Item(prop_value));
            }}}
        }},
        { SkillClearInjury, {
            "Fully clears up to <b>%1 injury(ies)</b>, starting with the rightmost.",
            {}
        }}
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

void ItemProperties::call_hooks(HookType type, const HookPayload &payload, AorUInt int_domain) const {
    for (const auto &pair : *this) {
        auto prop_def = property_definitions().find(pair.first);
        if (prop_def == property_definitions().end()) {
            continue;
        }

        auto hook = prop_def->second.hooks.find(type);
        if (hook == prop_def->second.hooks.end()) {
            continue;
        }

        hook->second(payload, pair.second, int_domain);
    }
}
