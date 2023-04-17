// Copyright 2023 Pierce Smith
// This software is licensed under the terms of the Lesser GNU General Public License.

#pragma once

#include <variant>

#include <QtCore>

#include "die.h"
#include "types.h"

// WARNING: verbatim from generators.h, which we can't directly include
template <typename T> using WeightedVector = std::vector<std::pair<T, double>>;

class Item;
class Character;
class ClampedResource;

using HookPayload = std::array<std::variant<
    bool *,
    qreal *,
    AorUInt *,
    AorInt *,
    Item *,
    ClampedResource *,
    QString *,
    std::vector<WeightedVector<Item>> *,
    Character *>, 4>;
using Hook = std::function<void(const HookPayload &, AorUInt, AorUInt, Item)>;

template <typename T> T extract_payload(const HookPayload &payload, size_t index) {
    if (!std::holds_alternative<T>(payload[index])) {
        bugcheck(IncorrectHookPayload, typeid(T).name(), payload[index].index());
    }
    return std::get<T>(payload[index]);
}

enum HookType : AorUInt {
    HookNone,
    HookCanDoActionCheck,
    HookPostActivity,
    HookPostEat,
    HookCalcActivityTime,
    HookCalcMaterialBonus,
    HookCalcItemDoubleChance,
    HookCalcBonusProductUse,
    HookCalcInjuryChance,
    HookDecideProducts,
    HookPostDecideProducts,
    HookDecideCanTravel,
    HookCalcThreatGain,
    HookCalcShouldDie,
    HookJustHatched,
    HookCalcLoreGain,
    HookPostDeath,
};
