#include "inventory.h"
#include "gamewindow.h"
#include "die.h"

Items &Inventory::items() {
    return m_items;
}

Item &Inventory::get_item_ref(ItemId id) {
    if (id == EMPTY_ID) {
        bugcheck(EmptyIdRef);
    }

    auto search_result = std::find_if(begin(m_items), end(m_items), [id](const Item &item) {
        return item.id == id;
    });

    if (search_result == end(m_items)) {
        bugcheck(ItemByIdLookupMiss, "get_ref", id);
    }

    return *search_result;
}

Item Inventory::get_item(ItemId id) const {
    if (id == EMPTY_ID) {
        return Item();
    }

    auto search_result = std::find_if(begin(m_items), end(m_items), [id](const Item &item) {
        return item.id == id;
    });

    if (search_result == end(m_items)) {
        bugcheck(ItemByIdLookupMiss, "get_instance", id);
    }

    return *search_result;
}

Item &Inventory::get_item_ref(int y, int x) {
    return m_items[inventory_index(y, x)];
}

Item Inventory::get_item(int y, int x) const {
    return m_items[inventory_index(y, x)];
}

void Inventory::remove_item(int y, int x) {
    Item &item = m_items[inventory_index(y, x)];

    item = Item();
}

void Inventory::remove_item(ItemId id) {
    for (size_t i = 0; i < INVENTORY_SIZE; i++) {
        if (m_items[i].id == id) {
            m_items[i] = Item();
            gw()->save();

            return;
        }
    }

    bugcheck(ItemByIdLookupMiss, "remove", id);
}

bool Inventory::add_item(const Item &item) {
    for (int y = 0; y < INVENTORY_ROWS; y++) {
        for (int x = 0; x < INVENTORY_COLS; x++) {
            if (m_items[inventory_index(y, x)].id == EMPTY_ID) {
                m_items[inventory_index(y, x)] = item;
                gw()->save();
                return true;
            }
        }
    }

    return false;
}

void Inventory::put_item(const Item &item, int y, int x) {
    m_items[inventory_index(y, x)] = item;
    gw()->save();
}

ItemId Inventory::make_item(ItemDefinitionPtr def, int y, int x) {
    Item new_item = Item(def);
    m_items[inventory_index(y, x)] = new_item;
    gw()->save();

    return new_item.id;
}

bool Inventory::are_yx_coords_oob(int y, int x) {
    return (y < 0 || y >= INVENTORY_ROWS) || (x < 0 || x >= INVENTORY_COLS);
}

size_t Inventory::inventory_index(int y, int x) {
    return y * INVENTORY_COLS + x;
}
