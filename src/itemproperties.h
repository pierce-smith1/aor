// Copyright 2023 Pierce Smith
// This software is licensed under the terms of the Lesser GNU General Public License.

#pragma once

#include <QtCore>

#include "hooks.h"
#include "serialize.h"

class Character;

enum ItemProperty : AorUInt {
    NoProperty = 0,
    ItemLevel,
    ToolEnergyCost,
    ConsumableEnergyBoost,
    ConsumableSpiritBoost,
    ConsumableClearsNumEffects,
    ConsumableMakesCouplable,
    ConsumableRegeneratesLocation,
    ConsumableKills,
    ConsumableGeneratesRandomItems,
    ConsumableCopiesRandomItems,
    PersistentMaxEnergyBoost,
    PersistentMaxSpiritBoost,
    PersistentSpeedBonus,
    PersistentSpeedPenalty,
    PersistentEnergyPenalty,
    PersistentSpiritPenalty,
    PersistentRandomConsumableProducts,
    PersistentDiscoveryNotRandom,
    PersistentForageBonusChance,
    PersistentThreatDecrease,
    PersistentCannotDie,
    PersistentChaoticCalculations,
    PersistentEggsHaveColor,
    PersistentLoreMultiplier,
    PersistentEggPowerBoost, // JANK TODO: This only works on artifacts!
    PersistentInjuryPercentChance,
    PersistentDeathGivesLore,
    HeritageMaxEnergyBoost,
    HeritageMaxSpiritBoost,
    HeritageConsumableEnergyBoost,
    HeritageSmithProductUsageBoost,
    HeritageInjuryResilience,
    HeritageMaterialValueBonus,
    HeritageActivitySpeedBonus,
    HeritageItemDoubleChance,
    HeritageSpiritRetention,
    InjurySmithing,
    InjuryForaging,
    InjuryMining,
    InjuryTradinge,
    InjuryEating,
    InjuryDefiling,
    InjuryTrading,
    InjuryCoupling, // COCK INJURED
    InjuryTravelling,
    PropertyIfLore,
    PropertyLoreRequirement,
    PropertyIfLoreValue,
    ToolDiscoverWeight1,
    ToolDiscoverWeight2,
    ToolDiscoverWeight3,
    ToolDiscoverWeight4,
    ToolDiscoverWeight5,
    ToolDiscoverWeight6,
    ToolDiscoverWeight7,
    ToolDiscoverWeight8,
    ToolDiscoverWeight9,
    HoldsItemCode = 0x0800, // indicates that the property holds an item code, see itemmark.h
    ConsumableGivesEffect,
    ConsumableGivesEffectToAll,
    ToolCanDiscover1,
    ToolCanDiscover2,
    ToolCanDiscover3,
    ToolCanDiscover4,
    ToolCanDiscover5,
    ToolCanDiscover6,
    ToolCanDiscover7,
    ToolCanDiscover8,
    ToolCanDiscover9,
    InstanceEggParent1 = 0x1000,
    InstanceEggParent2,
    InstanceEggFoundThreatstamp,
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
    SkillProperty = 0x10000,
    SkillDraggableTo,
    SkillActionCooldown,
    SkillPropertyUpgrades1,
    SkillPropertyUpgrades2,
    SkillPropertyUpgrades3,
    SkillPropertyUpgrades4,
    SkillPropertyUpgrades5,
    SkillPropertyUpgrades6,
    SkillPropertyUpgrades7,
    SkillPropertyUpgrades8,
    SkillPropertyUpgrades9,
    SkillUpgradeIncrement1,
    SkillUpgradeIncrement2,
    SkillUpgradeIncrement3,
    SkillUpgradeIncrement4,
    SkillUpgradeIncrement5,
    SkillUpgradeIncrement6,
    SkillUpgradeIncrement7,
    SkillUpgradeIncrement8,
    SkillUpgradeIncrement9,
    SkillUpgradeDecrement1,
    SkillUpgradeDecrement2,
    SkillUpgradeDecrement3,
    SkillUpgradeDecrement4,
    SkillUpgradeDecrement5,
    SkillUpgradeDecrement6,
    SkillUpgradeDecrement7,
    SkillUpgradeDecrement8,
    SkillUpgradeDecrement9,
    SkillClearInjury,
    WeatherProperty = 0x20000,
    WeatherEffect1 = WeatherProperty | HoldsItemCode | 1,
    WeatherEffect2 = WeatherProperty | HoldsItemCode | 2,
    LocationProperty = 0x40000,
    LocationSpiritCost,
    LocationEnergyCost,
    LocationPartyRequirement,
    LocationResourceRequirement,
    LocationSignatureItem1 = LocationProperty | HoldsItemCode | 1,
    LocationSignatureItem2 = LocationProperty | HoldsItemCode | 2,
    LocationSignatureItem3 = LocationProperty | HoldsItemCode | 3,
    LocationSignatureItem4 = LocationProperty | HoldsItemCode | 4,
    LocationSignatureItem5 = LocationProperty | HoldsItemCode | 5,
    LocationSignatureItem6 = LocationProperty | HoldsItemCode | 6,
    LocationSignatureItem7 = LocationProperty | HoldsItemCode | 7,
    LocationSignatureItem8 = LocationProperty | HoldsItemCode | 8,
    LocationSignatureItem9 = LocationProperty | HoldsItemCode | 9,
    InventoryProperty = 0x80000,
    InventoryInfectsItems,
    InventoryMaxSpiritBoost,
    InventoryMaxEnergyBoost,
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
struct ItemProperties : public Serializable {
    ItemProperties() = default;
    ItemProperties(std::initializer_list<std::pair<const ItemProperty, AorUInt>> map);
    ItemProperties(const std::map<ItemProperty, AorUInt> &map);
    AorUInt operator[](ItemProperty prop) const;
    std::map<ItemProperty, AorUInt>::const_iterator begin() const;
    std::map<ItemProperty, AorUInt>::const_iterator end() const;
    void call_hooks(
        HookType type,
        const HookPayload &payload,
        Item caller,
        AorUInt int_domain = 0,
        ItemProperty allowed_prop_type = (ItemProperty) ~InventoryProperty
    ) const;

    std::map<ItemProperty, AorUInt> map;

    void serialize(QIODevice *dev) const;
    void deserialize(QIODevice *dev);
};
