#include "itemproperties.h"
#include "items.h"

#define HOOK_0 [](const HookPayload &, quint16 prop_value) {}

#define HOOK_1(t1, n1) [](const HookPayload &payload, quint16 prop_value, quint16 item_domain) { \
    t1 * n1 = extract_payload<t1 *>(payload, 0);

#define HOOK_2(t1, n1, t2, n2) [](const HookPayload &payload, quint16 prop_value, quint16 item_domain) { \
    t1 * n1 = extract_payload<t1 *>(payload, 0); \
    t2 * n2 = extract_payload<t2 *>(payload, 1);

#define HOOK_3(t1, n1, t2, n2, t3, n3) [](const HookPayload &payload, quint16 prop_value, quint16 item_domain) { \
    t1 * n1 = extract_payload<t1 *>(payload, 0); \
    t2 * n2 = extract_payload<t2 *>(payload, 1); \
    t3 * n3 = extract_payload<t3 *>(payload, 2);

#define HOOK_4(t1, n1, t2, n2, t3, n3, t4, n4) [](const HookPayload &payload, quint16 prop_value, quint16 item_domain) { \
    t1 * n1 = extract_payload<t1 *>(payload, 0); \
    t2 * n2 = extract_payload<t2 *>(payload, 1); \
    t3 * n3 = extract_payload<t3 *>(payload, 2); \
    t4 * n4 = extract_payload<t4 *>(payload, 3);

#define HOOK_END }

const std::map<ItemProperty, PropertyDefinition> &property_definitions() {
    const static std::map<ItemProperty, PropertyDefinition> PROPERTY_DESCRIPTIONS = {
        { ItemLevel, {
            "", // ItemLevel has specific code to handle its unique display.
            {{ HookCalcSpiritGain, HOOK_1(qint32, spirit_gain)
                if (item_domain & Material) {
                    *spirit_gain += prop_value * 25;
                }
            }}}
        }},
        { ToolEnergyCost, {
            "Requires <b>%1 energy</b> per use.",
            {{ HookCanDoActionCheck, HOOK_2(bool, can_do, quint16, current_energy)
                *can_do = *can_do && (*current_energy >= prop_value);
            }},
            { HookCalcEnergyGain, HOOK_1(qint32, energy_gain)
                *energy_gain -= prop_value;
            }}}
        }},
        { ConsumableEnergyBoost, {
            "Gives <b>+%1 energy</b>.",
            {{ HookCalcEnergyGain, HOOK_1(qint32, energy_gain)
                *energy_gain += prop_value;
            }}}
        }},
        { ConsumableSpiritBoost, {
            "Gives <b>+%1 spirit</b>.",
            {{ HookCalcSpiritGain, HOOK_1(qint32, spirit_gain)
                *spirit_gain += prop_value;
            }}}
        }},
        { ConsumableClearsNumEffects, {
            "Fully clears up to <b>%1 injury(ies)</b>, starting with the rightmost.",
            {}
        }},
        { ConsumableMakesCouplable, {
            "Gives the ability to <b><font color=purple>have a child</font></b> with another explorer.",
            {}
        }},
        { PersistentMaxEnergyBoost, {
            "I have <b>+%1 max energy</b>.",
            {{ HookCalcMaxEnergy, HOOK_1(qint32, max_energy)
                *max_energy += prop_value;
            }}}
        }},
        { PersistentMaxSpiritBoost, {
            "I have <b>+%1 max spirit</b>.",
            {{ HookCalcMaxSpirit, HOOK_1(qint32, max_spirit)
                *max_spirit += prop_value;
            }}}
        }},
        { PersistentSpeedBonus, {
            "My actions complete <b>%1x faster</b>.",
            {}
        }},
        { PersistentSpeedPenalty, {
            "My actions complete <b>%1% slower</b>.",
            {}
        }},
        { PersistentEnergyPenalty, {
            "My actions cost an additional <b>%1 energy</b>." ,
            {{ HookCalcEnergyGain, HOOK_1(qint32, energy_gain)
                *energy_gain -= prop_value;
            }}}
        }},
        { PersistentSpiritPenalty, {
            "My actions cost an additional <b>%1 spirit</b>." ,
            {{ HookCalcSpiritGain, HOOK_1(qint32, spirit_gain)
                *spirit_gain -= prop_value;
            }}}
        }},
        { HeritageMaxEnergyBoost, {
            "I have <b>+%1 max energy</b>.",
            {}
        }},
        { HeritageMaxSpiritBoost, {
            "I have <b>+%1 max spirit</b>.",
            {}
        }},
        { HeritageConsumableEnergyBoost, {
            "I get <b>+%1 bonus energy</b> when I eat something.",
            {}
        }},
        { HeritageSmithProductUsageBoost, {
            "Items that I craft have <b>+%1 use(s)</b>.",
            {}
        }},
        { HeritageInjuryResilience, {
            "I have a <b>-%1% chance to suffer an injury</b> after taking an action.",
            {}
        }},
        { HeritageMaterialValueBonus, {
            "Materials are <b>worth %1% more</b> when I use them.",
            {}
        }},
        { HeritageActivitySpeedBonus, {
            "My actions take <b>%1% less time</b>.",
            {}
        }},
        { HeritageItemDoubleChance, {
            "I have a <b>%1% chance</b> to <b>double</b> items recieved from actions.",
            {}
        }},
    };

    return PROPERTY_DESCRIPTIONS;
}

ItemProperties::ItemProperties(std::initializer_list<std::pair<const ItemProperty, quint16>> map)
    : map(map) { }

quint16 ItemProperties::operator[](ItemProperty prop) const {
    try {
        return map.at(prop);
    } catch (std::out_of_range &e) {
        return 0;
    }
}

std::map<ItemProperty, quint16>::const_iterator ItemProperties::begin() const {
    return map.begin();
}

std::map<ItemProperty, quint16>::const_iterator ItemProperties::end() const {
    return map.end();
}

void ItemProperties::call_hooks(HookType type, const HookPayload &payload, quint16 int_domain) const {
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
