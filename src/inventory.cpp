#include "inventory.h"

Item &Inventory::get_item_ref(ItemId id) {
    if (id == EMPTY_ID) {
        qFatal("Tried to get reference for the empty id");
    }

    auto match_id {[id](const Item &item) -> bool { return item.id == id; }};
    auto search_result {std::find_if(begin(m_items), end(m_items), match_id)};

    if (search_result == end(m_items)) {
        qFatal("Searching for an item by id turned up nothing (%lx)", id);
    }

    return *search_result;
}

Item Inventory::get_item(ItemId id) const {
    if (id == EMPTY_ID) {
        return Item();
    }

    auto match_id {[id](const Item &item) -> bool { return item.id == id; }};
    auto search_result {std::find_if(begin(m_items), end(m_items), match_id)};

    if (search_result == end(m_items)) {
        qFatal("Searching for an item by id turned up nothing (%lx)", id);
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
    Item &item {m_items[inventory_index(y, x)]};

    item = Item();
}

void Inventory::remove_item(ItemId id) {
    for (size_t i {0}; i < INVENTORY_SIZE; i++) {
        if (m_items[i].id == id) {
            m_items[i] = Item();

            return;
        }
    }

    qWarning("Tried to remove item by id, but it didn't exist (%ld)", id);
}

bool Inventory::add_item(const Item &item) {
    for (int y {0}; y < INVENTORY_ROWS; y++) {
        for (int x {0}; x < INVENTORY_COLS; x++) {
            if (m_items[inventory_index(y, x)].id == EMPTY_ID) {
                m_items[inventory_index(y, x)] = item;
                return true;
            }
        }
    }

    qWarning(
        "Tried to add item (code %d, id %ld) to items, but there was no open spot",
        item.code,
        item.id
    );
    return false;
}

void Inventory::put_item(const Item &item, int y, int x) {
    m_items[inventory_index(y, x)] = item;
}

ItemId Inventory::make_item(ItemDefinitionPtr def, int y, int x) {
    Item new_item {Item(def)};
    m_items[inventory_index(y, x)] = new_item;

    return new_item.id;
}

std::vector<Item> Inventory::items_of_intent(CharacterId char_id, ItemDomain intent) {
    std::vector<Item> items;

    for (const Item &item : m_items) {
        if (item.intent == intent && item.intent_holder == char_id) {
            items.push_back(item);
        }
    }

    return items;
}

bool Inventory::are_yx_coords_oob(int y, int x) {
    return (y < 0 || y >= INVENTORY_ROWS) || (x < 0 || x >= INVENTORY_COLS);
}

size_t Inventory::inventory_index(int y, int x) {
    return y * INVENTORY_COLS + x;
}
