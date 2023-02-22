#pragma once

#include <QtCore>

#include "hooks.h"

class Character;

enum ItemProperty : quint16 {
    NoProperty,
    ItemLevel,
    ToolEnergyCost,
    // WARNING: Item generation behavior in actions.cpp requires that there
    // are exactly 9 ToolCanDiscovers, exactly 9 ToolDiscoverWeights, and
    // that the ToolDiscoverWeights comes directly after the ToolCanDiscovers!
    ToolCanDiscover1,
    ToolCanDiscover2,
    ToolCanDiscover3,
    ToolCanDiscover4,
    ToolCanDiscover5,
    ToolCanDiscover6,
    ToolCanDiscover7,
    ToolCanDiscover8,
    ToolCanDiscover9,
    ToolDiscoverWeight1,
    ToolDiscoverWeight2,
    ToolDiscoverWeight3,
    ToolDiscoverWeight4,
    ToolDiscoverWeight5,
    ToolDiscoverWeight6,
    ToolDiscoverWeight7,
    ToolDiscoverWeight8,
    ToolDiscoverWeight9,
    ConsumableEnergyBoost,
    ConsumableSpiritBoost,
    ConsumableGivesEffect,
    ConsumableClearsNumEffects,
    ConsumableMakesCouplable,
    PersistentMaxEnergyBoost,
    PersistentMaxSpiritBoost,
    PersistentSpeedBonus,
    PersistentSpeedPenalty,
    PersistentEnergyPenalty,
    PersistentSpiritPenalty,
    InjurySmithing,
    InjuryForaging,
    InjuryMining,
    InjuryTradinge,
    InjuryEating,
    InjuryDefiling,
    InjuryTrading,
    InjuryCoupling, // COCK INJURED
    HeritageMaxEnergyBoost,
    HeritageMaxSpiritBoost,
    HeritageConsumableEnergyBoost,
    HeritageSmithProductUsageBoost,
    HeritageInjuryResilience,
    HeritageMaterialValueBonus,
    HeritageActivitySpeedBonus,
    HeritageItemDoubleChance,
    InstanceEggParent1 = 0x1000,
    InstanceEggParent2,
    InstanceEggFoundActionstamp,
    InstanceEggFoundFlavor,
    Cost = 0x2000,
    CostStone = 0x2001,
    CostMetallic = 0x2002,
    CostCrystalline = 0x2003,
    CostRuinc = 0x2004,
    CostLeafy = 0x2005,
    ToolMaximum = 0x4000,
    ToolMaximumStone = 0x4001,
    ToolMaximumMetallic = 0x4002,
    ToolMaximumCrystalline = 0x4003,
    ToolMaximumRunic = 0x4004,
    ToolMaximumLeafy = 0x4005,
    Resource = 0x8000,
    StoneResource = 0x8001,
    MetallicResource = 0x8002,
    CrystallineResource = 0x8003,
    RunicResource = 0x8004,
    LeafyResource = 0x8005,
};

struct PropertyDefinition {
    QString description;
    std::map<HookType, Hook> hooks;
};

// Definitions are made in itemproperties.cpp.
const std::map<ItemProperty, PropertyDefinition> &property_definitions();

// This basically just wraps a std::map<ItemPropety, int>,
// with the crucial change that operator[] is const while retaining the
// behavior of returning zero-initialized values for non-existant keys.
class ItemProperties {
public:
    ItemProperties() = default;
    ItemProperties(std::initializer_list<std::pair<const ItemProperty, quint16>> map);
    quint16 operator[](ItemProperty prop) const;
    std::map<ItemProperty, quint16>::const_iterator begin() const;
    std::map<ItemProperty, quint16>::const_iterator end() const;
    void call_hooks(HookType type, const HookPayload &payload, quint16 int_domain = 0) const;

    std::map<ItemProperty, quint16> map;
};
