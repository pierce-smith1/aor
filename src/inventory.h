#pragma once

#include <array>

#include "items.h"
#include "io.h"
#include "serialize.h"

const static AorUInt INVENTORY_ROWS = 7;
const static AorUInt INVENTORY_COLS = 11;
const static AorUInt INVENTORY_SIZE = INVENTORY_COLS * INVENTORY_ROWS;

static Item mock_item = Item();

using Items = std::array<Item, INVENTORY_SIZE>;

class Inventory : public Serializable {
public:
    Items &items();

    Item &get_item_ref(ItemId id);
    Item &get_item_ref(AorUInt y, AorUInt x);
    Item get_item(ItemId id) const;
    Item get_item(AorUInt y, AorUInt x) const;
    void remove_item(AorUInt y, AorUInt x);
    void remove_item(ItemId id);
    bool add_item(const Item &item);
    void put_item(const Item &item, AorUInt y, AorUInt x);
    ItemId make_item(ItemDefinitionPtr def, AorUInt y, AorUInt x);

    static bool are_yx_coords_oob(AorUInt y, AorUInt x);
    static size_t inventory_index(AorUInt y, AorUInt x);

    void serialize(QIODevice *dev) const;

private:
    Items m_items;
};
