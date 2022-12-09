#pragma once

#include "items.h"
#include "io.h"

const static int INVENTORY_ROWS = 4;
const static int INVENTORY_COLS = 7;
const static int INVENTORY_SIZE = INVENTORY_COLS * INVENTORY_ROWS;

class Inventory {
public:
    Item &get_item_ref(ItemId id);
    Item &get_item_ref(int y, int x);
    Item get_item(ItemId id) const;
    Item get_item(int y, int x) const;
    void remove_item(int y, int x);
    void remove_item(ItemId id);
    bool add_item(const Item &item);
    void put_item(const Item &item, int y, int x);
    ItemId make_item(ItemDefinitionPtr def, int y, int x);
    std::vector<Item> items_of_intent(CharacterId char_id, ItemDomain intent);

    static bool are_yx_coords_oob(int y, int x);
    static size_t inventory_index(int y, int x);

    void serialize(QIODevice *dev);
    static Inventory deserialize(QIODevice *dev);

private:
    std::array<Item, INVENTORY_SIZE> m_items;
};
