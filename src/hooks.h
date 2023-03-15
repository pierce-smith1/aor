#pragma once

#include <variant>

#include <QtCore>

#include "die.h"
#include "types.h"

// WARNING: verbatim from generators.h, which we can't directly include
template <typename T> using WeightedVector = std::vector<std::pair<T, double>>;

class Item;
class Character;

using HookPayload = std::array<std::variant<
    bool *,
    qreal *,
    AorUInt *,
    AorInt *,
    QString *,
    std::vector<WeightedVector<Item>> *,
    Character *>, 4>;
using Hook = std::function<void(const HookPayload &, AorUInt, AorUInt)>;

template <typename T> T extract_payload(const HookPayload &payload, size_t index) {
    if (!std::holds_alternative<T>(payload[index])) {
        bugcheck(IncorrectHookPayload, typeid(T).name(), payload[index].index());
    }
    return std::get<T>(payload[index]);
}

enum HookType : AorUInt {
    HookNone,
    HookCanDoActionCheck,
    HookCalcEnergyGain,
    HookCalcSpiritGain,
    HookPostActivity,
    HookCalcMaxEnergy,
    HookCalcMaxSpirit,
    HookCalcBonusConsumableEnergy,
    HookPostEat,
    HookCalcActivityTime,
    HookCalcMaterialBonus,
    HookCalcItemDoubleChance,
    HookCalcBonusProductUse,
    HookCalcInjuryChance,
    HookDecideProducts,
    HookPostDecideProducts,
};
