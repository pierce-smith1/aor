#pragma once

#include <QtCore>

#include "hooks.h"

class Character;

enum ItemProperty : AorUInt {
    NoProperty = 0,
    ItemLevel,
    ToolEnergyCost,
    ConsumableEnergyBoost,
    ConsumableSpiritBoost,
    ConsumableClearsNumEffects,
    ConsumableMakesCouplable,
    PersistentMaxEnergyBoost,
    PersistentMaxSpiritBoost,
    PersistentSpeedBonus,
    PersistentSpeedPenalty,
    PersistentEnergyPenalty,
    PersistentSpiritPenalty,
    PersistentRandomConsumableProducts,
    PersistentDiscoveryNotRandom,
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
    HoldsItemCode = 0x0800, // indicates that the property holds an item code, see itemmark.h
    ConsumableGivesEffect,
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
    InstanceEggParent1 = 0x1000,
    InstanceEggParent2,
    InstanceEggFoundActionstamp,
    InstanceEggFoundFlavor,
    Cost = 0x2000,
    CostStone,
    CostMetallic,
    CostCrystalline,
    CostRuinc,
    CostLeafy,
    ToolMaximum = 0x4000,
    ToolMaximumStone,
    ToolMaximumMetallic,
    ToolMaximumCrystalline,
    ToolMaximumRunic,
    ToolMaximumLeafy,
    Resource = 0x8000,
    StoneResource,
    MetallicResource,
    CrystallineResource,
    RunicResource,
    LeafyResource,
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
    ItemProperties(std::initializer_list<std::pair<const ItemProperty, AorUInt>> map);
    ItemProperties(const std::map<ItemProperty, AorUInt> &map);
    AorUInt operator[](ItemProperty prop) const;
    std::map<ItemProperty, AorUInt>::const_iterator begin() const;
    std::map<ItemProperty, AorUInt>::const_iterator end() const;
    void call_hooks(HookType type, const HookPayload &payload, AorUInt int_domain = 0) const;

    std::map<ItemProperty, AorUInt> map;
};
