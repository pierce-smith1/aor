#pragma once

// This (and itemmark.cpp) are a bunch of ugly bullshit to allow the item
// definitions in items.h to specify a *name* for properties that expect item
// codes rather than the item codes themselves.
// It's been isolated in here because it's ugly bullshit that is mostly
// irrelevant to the game itself; it's purely for the convenience of
// defining items.

#include <vector>

#include <QtCore>
#include <QString>

#include "types.h"

class ItemDefinition;

class ItemMark {
public:
    static std::vector<ItemDefinition> resolve_markers(std::vector<ItemDefinition> defs);
    static AorUInt marker(const QString &item_name);

private:
    static std::vector<QString> marked_names;
};
