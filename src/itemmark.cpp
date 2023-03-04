#include "itemmark.h"
#include "items.h"

std::vector<QString> ItemMark::marked_names;

std::vector<ItemDefinition> ItemMark::resolve_markers(std::vector<ItemDefinition> defs) {
    for (ItemDefinition &def : defs) {
        for (auto &pair : def.properties.map) {
            if (pair.first & HoldsItemCode) {
                QString item_name = marked_names[pair.second];
                auto result = std::find_if(defs.begin(), defs.end(), [&](const ItemDefinition &d) {
                    return d.internal_name == item_name;
                });

                if (result == defs.end()) {
                    // We can't bugcheck this early!
                    qFatal("Item definitions are invalid: item \"%s\" was marked, but it doesn't exist!", item_name.toStdString().c_str());
                }

                pair.second = result->code;
            }
        }
    }

    return defs;
}

quint16 ItemMark::marker(const QString &name) {
    auto existing_mark = std::find_if(marked_names.begin(), marked_names.end(), [&](const QString &n) {
        return name == n;
    });

    if (existing_mark != marked_names.end()) {
        return marked_names.size() - (marked_names.end() - existing_mark);
    }

    marked_names.push_back(name);
    return marked_names.size() - 1;
}
