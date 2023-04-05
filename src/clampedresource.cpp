// Copyright 2023 Pierce Smith
// This software is licensed under the terms of the Lesser GNU General Public License.

#include "clampedresource.h"
#include "gamewindow.h"

ClampedResource::ClampedResource(AorInt amount, AorInt max, HookType calc_max_hook)
    : m_amount(amount), m_max(max), m_calc_max_hook(calc_max_hook) {}

AorInt ClampedResource::amount() const {
    return m_amount;
}

void ClampedResource::serialize(QIODevice *dev) const {
    Serialize::serialize(dev, m_amount);
}

void ClampedResource::deserialize(QIODevice *dev) {
    Serialize::deserialize(dev, m_amount);
}

AllCharacters AllCharacters::instance;

void AllCharacters::call_hooks(HookType type, const HookPayload &payload) {
    for (Character &character : gw()->game()->characters()) {
        character.call_hooks(type, payload);
    }
}
