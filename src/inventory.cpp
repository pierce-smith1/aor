#include "inventory.h"
#include "gamewindow.h"
#include "die.h"

Items &Inventory::items() {
    return m_items;
}

Item &Inventory::get_item_ref(ItemId id) {
    if (id == EMPTY_ID) {
        mock_item = Item();
        return mock_item;
    }

    auto search_result = std::find_if(begin(m_items), end(m_items), [id](const Item &item) {
        return item.id == id;
    });

    if (search_result == end(m_items)) {
        mock_item = Item();
        return mock_item;
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

Item &Inventory::get_item_ref(AorUInt y, AorUInt x) {
    return m_items[inventory_index(y, x)];
}

Item Inventory::get_item(AorUInt y, AorUInt x) const {
    return m_items[inventory_index(y, x)];
}

void Inventory::remove_item(AorUInt y, AorUInt x) {
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
    for (AorUInt y = 0; y < INVENTORY_ROWS; y++) {
        for (AorUInt x = 0; x < INVENTORY_COLS; x++) {
            if (m_items[inventory_index(y, x)].id == EMPTY_ID) {
                m_items[inventory_index(y, x)] = item;
                gw()->save();
                return true;
            }
        }
    }

    return false;
}

void Inventory::put_item(const Item &item, AorUInt y, AorUInt x) {
    m_items[inventory_index(y, x)] = item;
    gw()->save();
}

ItemId Inventory::make_item(ItemDefinitionPtr def, AorUInt y, AorUInt x) {
    Item new_item = Item(def);
    m_items[inventory_index(y, x)] = new_item;
    gw()->save();

    return new_item.id;
}

std::pair<size_t, size_t> Inventory::coordinates_of(ItemId id) {
    auto inventory_pos = std::find_if(m_items.begin(), m_items.end(), [=](const Item &item) {
        return item.id == id;
    });

    if (inventory_pos == m_items.end()) {
        bugcheck(ItemByIdLookupMiss, "coordinates_of", id);
        return {};
    }

    size_t pos = inventory_pos - m_items.begin();
    return { pos / INVENTORY_COLS, pos % INVENTORY_COLS };
}

bool Inventory::are_yx_coords_oob(AorUInt y, AorUInt x) {
    return (y >= INVENTORY_ROWS) || (x >= INVENTORY_COLS);
}

size_t Inventory::inventory_index(AorUInt y, AorUInt x) {
    return y * INVENTORY_COLS + x;
}

void Inventory::serialize(QIODevice *dev) const {
    Serialize::serialize(dev, m_items);
}

void Inventory::deserialize(QIODevice *dev) {
    Serialize::deserialize(dev, m_items);
}
