#pragma once

#include <variant>

#include <QtCore>

#include "die.h"

using HookPayload = std::array<std::variant<bool *, quint32 *, qint32 *, quint16 *, qint16 *, QString *>, 4>;
using Hook = std::function<void(const HookPayload &, quint16, quint16)>;

template <typename T> T extract_payload(const HookPayload &payload, size_t index) {
    if (!std::holds_alternative<T>(payload[index])) {
        bugcheck(IncorrectHookPayload, typeid(T).name(), payload[index].index());
    }
    return std::get<T>(payload[index]);
}

enum HookType : quint16 {
    HookNone,
    HookCanDoActionCheck,
    HookCalcEnergyGain,
    HookCalcSpiritGain,
    HookPostActivity,
    HookCalcMaxEnergy,
    HookCalcMaxSpirit,
};
