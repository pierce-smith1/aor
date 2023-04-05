// Copyright 2023 Pierce Smith
// This software is licensed under the terms of the Lesser GNU General Public License.

#pragma once

// This is a bunch of ugly bullshit to allow definitions
// (e.g. items.h, locations.h) to specify a *name* for properties that expect item
// codes rather than the item codes themselves.
// It's been isolated in here because it's ugly bullshit that is mostly
// irrelevant to the game itself; it's purely for the convenience of
// defining items.

#include <vector>

#include <QtCore>
#include <QString>

#include "itemproperties.h"
#include "types.h"

template <typename SrcDef, typename DestDef> class DefinitionMark {
public:
    static std::vector<DestDef> resolve_markers(std::vector<DestDef> defs) {
        return resolve_markers(defs, defs);
    }

    static std::vector<DestDef> resolve_markers(const std::vector<SrcDef> &src_defs, std::vector<DestDef> dest_defs) {
        for (DestDef &def : dest_defs) {
            for (auto &pair : def.properties.map) {
                if (pair.first & HoldsItemCode) {
                    QString item_name = marked_names[pair.second];
                    auto result = std::find_if(src_defs.begin(), src_defs.end(), [&](const SrcDef &d) {
                        return d.internal_name == item_name;
                    });

                    if (result == src_defs.end()) {
                        // We can't bugcheck this early!
                        qFatal("Definitions are invalid: name \"%s\" was marked, but it doesn't exist!", item_name.toStdString().c_str());
                    }

                    pair.second = result->code;
                }
            }
        }

        return dest_defs;
    }

    static AorUInt marker(const QString &name) {
        auto existing_mark = std::find_if(marked_names.begin(), marked_names.end(), [&](const QString &n) {
            return name == n;
        });

        if (existing_mark != marked_names.end()) {
            return marked_names.size() - (marked_names.end() - existing_mark);
        }

        marked_names.push_back(name);
        return marked_names.size() - 1;
    }

private:
    inline static std::vector<QString> marked_names;
};

struct ItemDefinition;
struct LocationDefinition;

using ItemMark = DefinitionMark<ItemDefinition, ItemDefinition>;
using WeatherMark = DefinitionMark<ItemDefinition, LocationDefinition>;
